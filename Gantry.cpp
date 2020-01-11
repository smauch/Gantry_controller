#include "Gantry.h"

#include <chrono>
#include <thread>

CML_NAMESPACE_USE();


static void showerr(const Error* err, const char* msg) {
	if (!err) return;
	printf("Error: %s - %s\n", msg, err->toString());
	exit(1);
};



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

Gantry::Gantry()
{
	
}

Gantry::~Gantry()
{

	ptpMove(homePos);
	for (int i = 0; i < numAmp; i++)
	{
		err = this->link[i].Disable(true);
		showerr(err, "Disable Amplifiers");
	}
}
	

bool Gantry::initGantry()
{	
	// It's handy for debugging, but not necessary for a production system.
	//cml.SetDebugLevel(LOG_EVERYTHING);
	// Open the low level CAN port that will be used to communicate to the amplifiers.
	err = can.Open();
	showerr(err, "Opening CAN port");
	// Create the upper level CANopen object
	err = this->canOpen.Open(can);
	showerr(err, "Opening CANopen network");
	// Initialize the amplifier
	for (short i = 0; i < numAmp; i++)
	{
		err = this->amp[i].Init(canOpen, CAN_AXIS[i]);
		showerr(err, "Initting Axis amplifier");
	}
	//Load the amplifier settings from CME2 config


	for (short i = 0; i < numAmp; i++)
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
	err = this->link.Init(numAmp, amp);
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
	for (short i = 0; i < numAmp; i++)
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



bool Gantry::catchCandy(double angularVel, double angular, double radius)
{
	this->calcMovement(angularVel, angular, radius);
	err = this->link.MoveTo(pos);
	showerr(err, "Starting move");
	err = this->link.WaitMoveDone(20000);
	this->setValve(false);
	this->setPump(true);
	pos[2] = CATCH_Z_HEIGHT;
	err = this->link.MoveTo(pos);
	err = this->link.WaitMoveDone(20000);
	pos[2] = pos[2] - 10000;
	err = this->link.MoveTo(pos);
	showerr(err, "Starting move");
	err = this->link.WaitMoveDone(20000);
	if (getCatched())
	{
		//driveback
		this->setPump(false);
		//Drive to Output
		this->ptpMove(dropPos);
		this->setValve(true);
		this->ptpMove(lurkPos);
	}
	else
	{
		this->setPump(false);
		this->ptpMove(lurkPos);
		return false;
	}
	//Upload trajectory to buffer and start movement
	/*for (int i = 0; i < 200; i++)
	{
		std::cout << "Pos " << trj.xPos[i] << "," << trj.yPos[i] << "," << trj.zPos[i] << "," << std::endl;
		std::cout << "Vel " << trj.xVel[i] << "," << trj.yVel[i] << "," << trj.zVel[i] << "," << std::endl;
		std::cout << "Time " << trj.trjTime[i] << std::endl;	

	}

	err = link.SendTrajectory(trj);
	showerr(err, "Loading trajectory to Buffer");
	err = link.StartMove();
	showerr(err, "Starting trj in Buffer");
	err = link.WaitMoveDone(20000);
	showerr(err, "Waiting for trj done");*/
	//Check presure of z-axis to make sure that candy has been catched
	return false;
}




bool Gantry::prepareCatch()
{
	this->link.ClearLatchedError();

	ptpMove(lurkPos);
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
		ptpMove(lurkPos);
		ptpMove(discCenterPos);
	}
	ptpMove(homePos);
}


bool Gantry::homeAxis()
{
	//Check if CME2 also sets up the Homeing options
	//otherwiese
	//HomeConfig hcfg;
	//err = link[i].GoHome( hcfg );
	for (short i = numAmp - 1; i >= 0; i--)
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



void Gantry::calcMovement(double angularVelOffset, double angularOffset, double radius) {
	//get candy object and calc move
	//Check if radius is out of range
	if (radius < discRadius[0] || radius > discRadius[1])
	{
		printf("radius out of operation area");
		exit(1);
	}

	//get current motorPos should be lurk pos
	//first move is ptp


	pos[0] = radius * sin(angularOffset) + discCenterPos[0];
	pos[1] = radius * -cos(angularOffset) + discCenterPos[1];
	pos[2] = 24000;

	//double t = 0.01;
	//double angularVelMax = PI / 4;
	//double angularAccMax = PI / 4;
	//for (int i  = 0; i < NUMBER_POS_CALC; i++)
	//{
	//	if (angularVel < angularVelMax) {
	//		angularAcc = angularAccMax;
	//	}
	//	else
	//	{
	//		angularAcc = 0;
	//	}

	//	angular = angular + angular * t + angularAcc * t * t / 2;
	//	angularVel = angularVel + angularAcc * t;

	//	trj.xPos[i] = cos(angular) * radius + discCenterPos[0];
	//	trj.yPos[i] = sin(angular) * radius + discCenterPos[1];
	//	trj.zPos[i] = 20000;
	//	trj.xVel[i] = -cos(angular) * angularVel * radius;
	//	trj.yVel[i] = sin(angular) * angularVel * radius;
	//	trj.zVel[i] = 0;
	//	if (i < NUMBER_POS_CALC - 1) {
	//		trj.trjTime[i] = t;
	//	}
	//	else
	//	{
	//		trj.trjTime[i] = 0;
	//	}

	//}

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

