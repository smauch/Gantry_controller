#include "Gantry.h"

#include <chrono>
#include <thread>

CML_NAMESPACE_USE();

//Initialization of Fix points
const int Gantry::LURK_POS[Gantry::NUM_AMP] = { 138000, 0, 15000 };
const int Gantry::HOME_POS[Gantry::NUM_AMP] = { 0,0,0 };
const int Gantry::DROP_POS[Gantry::NUM_AMP] = { 0, 46350, 0 };
const int Gantry::DISC_CENTER_POS[Gantry::NUM_AMP] = { 110000, 46350, 20000 };
const int Gantry::CATCH_Z_HEIGHT = 25250;
const int Gantry::DISC_RADIUS[2] = { 8000, 46350 };



static void showerr(const Error* err, const char* msg) {
	if (!err) return;
	printf("Error: %s - %s\n", msg, err->toString());
	exit(1);
};

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
	err = link.SetMoveLimits(profConf.vel, profConf.acc*0.75, profConf.dec*0.75, profConf.jrk*0.5);
	showerr(err, "Setting move limits");

	angular = PI;
	angularVel = 0;

	return true;
}



bool Gantry::catchCandy(double angularVel, double angular, double myRadius)
{
	double radius = 46350 * (myRadius / 540.0);
	//Time measurement to catch
	auto start_move = std::chrono::high_resolution_clock::now();

	if (radius < DISC_RADIUS[0] || radius > DISC_RADIUS[1])
	{
		printf("radius out of operation area");
		exit(1);
	}
	pos[0] = radius * sin(angular) + DISC_CENTER_POS[0];
	pos[1] = radius * -cos(angular) + DISC_CENTER_POS[1];
	pos[2] = CATCH_Z_HEIGHT;

	auto stop_clac = std::chrono::high_resolution_clock::now();
	this->setValve(false);
	this->setPump(true);
	err = this->link.MoveTo(pos);
	showerr(err, "Starting move");
	err = this->link.WaitMoveDone(20000);
	
	/*pos[2] = CATCH_Z_HEIGHT;
	err = this->link.MoveTo(pos);
	err = this->link.WaitMoveDone(20000);*/

	auto stop_move= std::chrono::high_resolution_clock::now();
	pos[2] = pos[2] - 10000;
	err = this->link.MoveTo(pos);
	showerr(err, "Starting move");
	err = this->link.WaitMoveDone(20000);
	auto duration_calc = std::chrono::duration_cast<std::chrono::milliseconds>(stop_clac - start_move);
	auto duration_move = std::chrono::duration_cast<std::chrono::milliseconds>(stop_move - stop_clac);
	// To get the value of duration use the count() 
	// member function on the duration object 
	
	//std::cout << "Calculation lasts" << duration_calc.count() << " microsenconds" << std::endl;
	//std::cout << "Movement lasts " << duration_move.count() << " microsenconds" << std::endl;
	if (getCatched())
	{
		//driveback
		this->setPump(false);
		//Drive to Output
		this->ptpMove(DROP_POS);
		this->setValve(true);
		this->ptpMove(LURK_POS);
	}
	else
	{
		this->setPump(false);
		this->ptpMove(LURK_POS);
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

bool Gantry::outputCandy()
{
	//move to output 
	//check pressure
	return false;
}

void Gantry::waitingProgram(int times)
{
	//waiting program time in seconds
	//rand to choose which program
	//switch case for programs
	for (int i = 0; i < times; i++)
	{
		ptpMove(LURK_POS);
		ptpMove(DISC_CENTER_POS);
	}
	ptpMove(LURK_POS);
}

bool Gantry::pvtMove()
{
	//Upload trajectory to buffer and start movement
	err = link.SendTrajectory(trj);
	showerr(err, "Loading trajectory to Buffer");
	err = link.StartMove();
	showerr(err, "Starting trj in Buffer");
	err = link.WaitMoveDone(20000);
	showerr(err, "Waiting for trj done");
	//Check presure of z-axis to make sure that candy has been catched
	return false;
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




void Gantry::ptpMove(const int arr[3])
{
	for (short i = 0; i < 3; i++)
	{
		pos[i] = arr[i];
	}

	err = this->link.MoveTo(pos);
	showerr(err, "Starting move");

	err = this->link.WaitMoveDone(20000);
	showerr(err, "Waiting for move to finish");
}

