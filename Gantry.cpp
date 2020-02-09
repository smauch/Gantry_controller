#include "Gantry.h"
#include <map>
#include <chrono>
#include <thread>

CML_NAMESPACE_USE();

//Initialization of Fix points
uunit Gantry::LURK_POS[Gantry::NUM_AMP] = { 138000, 0, 10000 };
uunit Gantry::HOME_POS[Gantry::NUM_AMP] = { 0,0,0 };
uunit Gantry::DROP_POS[Gantry::NUM_AMP] = { 0, 46350, 10000 };
uunit Gantry::DISC_CENTER_POS[Gantry::NUM_AMP] = { 110000, 46350, 20000 };
uunit Gantry::CATCH_Z_HEIGHT = 25250;
uunit Gantry::DISC_RADIUS[2] = { 8000, 46350 };

uunit X_STORAGE = 9100;
std::map<int, uunit> Y_STORAGE = {{0, 69800}, {1, 77100}, {2,84600}};

double const Gantry::PIXEL_RADIUS = 540.0;



//static void showerr(const Error* err, const char* msg) {
//	if (!err) return;
//	
//	printf("Error: %s - %s\n", msg, err->toString());
//	//exit(1);
//};

Gantry::Gantry()
{

}

Gantry::~Gantry()
{

	ptpMove(HOME_POS);
	for (int i = 0; i < NUM_AMP; i++)
	{
		err = this->link[i].Disable(true);
		showerr(err, "Disable Amplifiers");
	}
}


bool Gantry::setPump(bool state)
{
	if (state) {
		this->amp[2].SetOutputConfig(1, OUTCFG_MANUAL_H);
	}
	else
	{
		this->amp[2].SetOutputConfig(1, OUTCFG_MANUAL_L);
	}
	return false;
}

bool Gantry::setValve(bool state)
{
	if (state) {
		//Close the Valve
		this->amp[2].SetOutputConfig(2, OUTCFG_MANUAL_H);
	}
	else
	{
		//Open the Valve
		this->amp[2].SetOutputConfig(2, OUTCFG_MANUAL_L);
	}
	return false;
}

bool Gantry::getCatched()
{
	uint16 currentInput;
	this->amp[2].GetInputs(currentInput);
	currentInput = currentInput & 0b10;
	if (currentInput)
	{
		return true;
	}
	else
	{
		return false;
	}
}


	

bool Gantry::initGantry()
{	
	// Open the low level CAN port that will be used to communicate to the amplifiers.
	err = can.Open();
	showerr(err, "Opening CAN port");
	// Create the upper level CANopen object
	err = this->canOpen.Open(can);
	showerr(err, "Opening CANopen network");
	// Initialize the amplifier
	for (short i = 0; i < NUM_AMP; i++)
	{
		err = this->amp[i].Init(canOpen, CAN_AXIS[i]);
		showerr(err, "Initting Axis amplifier");
	}
	//Load the amplifier settings from CME2 config


	for (short i = 0; i < NUM_AMP; i++)
	{
		int line;
		if (std::filesystem::exists(ampConfigPath[i].c_str())) {
			std::cout << ampConfigPath[i].c_str() << " exists" << std::endl;
		}
		else {
			std::cout << ampConfigPath[i].c_str() << " doesn't exist" << std::endl;
		}
		err = this->amp[i].LoadFromFile(ampConfigPath[i].c_str(), line);
		showerr(err, "Loading from file\n");
	}
	//Initalize linkit object
	err = this->link.Init(NUM_AMP, amp);
	showerr(err, "Linkage init");
	printf("Finished initting amps and linkage.\n");
	if (homeAxis() == false) {
		return false;
	}
	//Operation that will be performed if window is exceeded
	LinkSettings linkCfg;
	linkCfg.haltOnPosWarn = true;
	linkCfg.haltOnVelWin = true;
	link.Configure(linkCfg);
	//Halt modes
	for (short i = 0; i < NUM_AMP; i++)
	{
		this->link[i].SetHaltMode(HALT_QUICKSTOP);
	}

	//Movement limitations setup
	ProfileConfig profConf;
	err = amp[0].GetProfileConfig(profConf);
	showerr(err, "Read Profile Config");
	err = link.SetMoveLimits(profConf.vel*0.75, profConf.acc*0.5, profConf.dec*0.5, profConf.jrk*0.5);
	showerr(err, "Setting move limits");

	angular = PI;
	angularVel = 0;

	return true;
}

/**
*Catches a Candy that is moving on the rotary table
*
*@param angularVel the angular velocity of the candy in rad/ms
*@param angular the angular of the candy relative to the camera perspective
*@param radius the radius of the candy relative to the disc center in pixel
*@param targetPos the position where the candy should be dropped
*
*@return true if the candy got chatched and is dropped at targePos
*/

bool Gantry::catchRotary(double angularVel, double angular, double pixelRadius, uunit const targetPos[NUM_AMP], bool measureTime )
{
	//Time measurement to catch

	auto start_move = std::chrono::high_resolution_clock::now();

	//Radius scaling from pixel to uunit
	double radius = DISC_RADIUS[1] * (pixelRadius / PIXEL_RADIUS);
	
	if (radius < DISC_RADIUS[0] || radius > DISC_RADIUS[1])
	{
		//Should throw exception
		printf("radius out of operation area");
		return false;
	}

	//Current angular position


	//Upload trajectory to buffer and start movement
	//trj.calcMovement(getPos(), radius, angular, angularVel, targetPos);
	trj.calcMovement(Gantry::LURK_POS, radius, angular, angularVel, targetPos);
	auto stop_clac = std::chrono::high_resolution_clock::now();
	err = link.SendTrajectory(trj, false);
	showerr(err, "Loading trajectory to Buffer");
	this->setValve(false);
	//this->setPump(true);
	err = link.StartMove();
	showerr(err, "Starting trj in Buffer");
	err = link.WaitMoveDone(60000);
	showerr(err, "Waiting for trj done");
	this->setPump(false);
	auto stop_move = std::chrono::high_resolution_clock::now();
	auto duration_calc = std::chrono::duration_cast<std::chrono::milliseconds>(stop_clac - start_move);
	auto duration_move = std::chrono::duration_cast<std::chrono::milliseconds>(stop_move - stop_clac);
	// To get the value of duration use the count() 
	// member function on the duration object 
	std::cout << "Calculation lasts" << duration_calc.count() << " miliseconds" << std::endl;
	std::cout << "Movement lasts " << duration_move.count() << " miliseconds" << std::endl;
	if (getCatched())
	{
		//driveback
		this->setPump(false);
		//Drive to Output
		this->ptpMove(targetPos);
		this->setValve(true);
		this->ptpMove(LURK_POS);
	}
	else
	{
		this->setPump(false);
		//this->ptpMove(HOME_POS);
		return false;
	}
	return false;
}


bool Gantry::prepareCatch()
{
	this->link.ClearLatchedError();
	ptpMove(LURK_POS);
	//Check Motor state
	return false;
}

/**
* Catching candy from an none moving fix position and drop it at a target position
*
*@param candyPos is the position of the candy in uunits
*@param targetPos is the position where the candy should be dropped of
*/
bool Gantry::catchStatic(uunit candyPos[NUM_AMP], uunit targetPos[NUM_AMP])
{
	candyPos[2] -= 1000;
	ptpMove(candyPos);
	this->setValve(false);
	this->setPump(true);
	candyPos[2] += 1000;
	ptpMove(candyPos);
	candyPos[2] -= 1000;
	ptpMove(candyPos);
	this->setPump(false);
	ptpMove(targetPos);
	this->setValve(true);
	return false;
}


/**
*Does a S-Curve move to the target Pos 
*
*@param target psoition as array x,y,z
*/
void Gantry::ptpMove(uunit const targetPos[NUM_AMP])
{
	for (int i = 0; i < NUM_AMP; i++)
	{
		err = this->link[i].ClearFaults();
		showerr(err, "Faults not cleared");
		ampTargetPos[i] = targetPos[i];
	}
	

	err = this->link.MoveTo(ampTargetPos);
	showerr(err, "Starting move");
	err = this->link.WaitMoveDone(20000);
	showerr(err, "Waiting for move to finish");
}

uunit* Gantry::getPos()
{
	uunit actPos[NUM_AMP];
	for (short i = 0; i < NUM_AMP; i++)
	{
		link[i].GetPositionActual(actPos[i]);
	}
	return actPos;
}

bool Gantry::fillTable(int color)
{
	uunit target[3];
	target[0] = X_STORAGE;
	target[1] = Y_STORAGE[color];
	target[2] = 18000;
	ptpMove(target);
	ProfileConfigTrap carefulCatch;
	carefulCatch.acc = 5000;
	carefulCatch.dec = 5000;
	carefulCatch.vel = 5000;
	carefulCatch.pos = CATCH_Z_HEIGHT;
	this->link[2].SetupMove(carefulCatch);
	this->link[2].SetPositionWarnWindow(80);
	EventAny posWarn(AMPEVENT_POSWARN);
	EventNone noWarn(AMPEVENT_POSWARN);
	this->link[2].StartMove();
	err = this->link[2].WaitEvent(posWarn, 3000);
	if (!err) {
		std::cout << "performing quickstop" << std::endl;
		this->link[2].HaltMove();
		this->link[2].SetPositionWarnWindow(4096);
		this->setValve(false);
		this->setPump(true);
		ptpMove(target);
		this->setPump(false);
		ptpMove(DISC_CENTER_POS);
		this->setValve(true);
		return true;
	}
	else {
		err = this->link.WaitMoveDone(10000);
		this->link[2].SetPositionWarnWindow(4096);
		showerr(err, "Waiting for move to finish");
		std::cout << "empty" << std::endl;
		ptpMove(target);
		return false;
	}	
}

void Gantry::showerr(const Error* err, const char* msg)
{
	if (!err) return;
	printf("Error: %s - %s\n", msg, err->toString());
	system("PAUSE");
	exit(1);
}



bool Gantry::homeAxis()
{
	//Check if CME2 also sets up the Homeing options
	//otherwiese
	//HomeConfig hcfg;
	//err = link[i].GoHome( hcfg );
	for (short i = NUM_AMP - 1; i >= 0; i--)
	{
		//Home each amplifier with the default homeing settings
		err = this->link[i].GoHome();
		showerr(err, "Going home");
		// Wait for all amplifiers to finish the home by waiting on the linkage object itself.
		printf("Waiting for home to finish...\n");
		err = this->link[i].WaitMoveDone(20000);
		showerr(err, "waiting on home");
	}	
	char userInput;
	do
	{
		std::cout << "Did the axis home right? [y/n]" << std::endl;
		std::cin >> userInput;
	} while (!std::cin.fail() && userInput != 'y' && userInput != 'n');

	if (userInput == 'y')
	{
		return true;
	}
	else {
		printf("Error: Follow these steps.\n");
		printf("(1) Remove power supply and check if axes are able to move smoothly.\n");
		printf("(2) If this is not the problem, check homeing values in CME2.\n");
		return false;
	}
}




