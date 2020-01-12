#ifndef GANTRY_H 
#define GANTRY_H

#include <string>
#include <CML.h>
#include <can/can_ixxat_v3.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <filesystem>
#include <math.h>   
//#include "GantryTrajectory.h"

#define PI 3.14159265

using namespace CML;

class GantryTrajectory : public LinkTrajectory
{
	//It is therefore important to ensure that the trajectory object passed here will remain
	//valid(i.e. not be deallocated) until the linkage has called the LinkTrajectory.Finish() method
public:
	uunit xPos[200];
	uunit yPos[200];
	uunit zPos[200];
	uunit xVel[200];
	uunit yVel[200];
	uunit zVel[200];
	uunit trjTime[200];
	int posCounter = 0;

	virtual const Error* StartNew(void) { return 0; }

	virtual void Finish(void) {
		std::cout << "fertisch" << std::endl;
	}

	virtual int GetDim(void) {
		return 3;
	}

	virtual bool UseVelocityInfo(void) { return false; }

	virtual const Error* NextSegment(uunit pos[], uunit vel[], uint8& time) {
		//Samuels pure Virtual override

		pos[0] = xPos[posCounter];
		pos[1] = 0;
		pos[2] = 0;
		time = trjTime[posCounter];
		posCounter++;
		return 0;
	}
};

class Gantry
{
	
private:
	//Position consts
	const int LURK_POS[3] = { 138000, 0, 15000 };
	const int HOME_POS[3] = { 0,0,0 };
	const int DROP_POS[3] = { 0,46350,0 };
	const int DISC_CENTER_POS[3] = { 110000, 46350, 20000 };
	const int CATCH_Z_HEIGHT = 25250;
	//Minimum radius and maximum radius of the disc
	const int DISC_RADIUS[2] = { 8000, 46350 };
	//Number of amplifiers
	static const int NUM_AMP = 3;

	//Error obj to show error that are generated from CML
	const Error* err;
	//IXXAT USB to CAN adapter
	IxxatCANV3 can;
	//Upper level Can object
	CanOpen canOpen;
	// We use three Amp objects to control the three amplifiers in the system.
	Amp amp[NUM_AMP];
	//Create linkage object to handle the three axes simulatanously
	Linkage link;
	//Point Object for Point to point moves
	Point<3> pos;
	//Trajectory object
	GantryTrajectory trj;
	//CAN NodeIDs by the order X,Y,Z axis
	int CAN_AXIS[NUM_AMP] = { 2, 3, 1 };
	//Paths to the CME2 generated config files
	std::string ampConfigPath[NUM_AMP] = { "./parameters/x_axis.ccx", "./parameters/y_axis.ccx", "./parameters/z_axis.ccx" };
	//Home each axis seperatly due to may occuring under voltage
	bool homeAxis();
	//Trajectory planing with given angular velocity and angular of the disc
	void calcMovement(double angularVelOffset, double angularOffset, double radius);
	//Method to perform point to point moves with S-Curve profile
	void ptpMove(const int arr[3]);

	//Variables for motions of candy
	double angular;
	double angularVel;
	double angularAcc;

	//Fix positions
	bool setPump(bool state);
	bool setValve(bool state);
	bool getCatched();

public:
	const int NUMBER_POS_CALC = 200;
	//Constructor
	Gantry();
	~Gantry();

	//Loads amplifier CME2 configuration files and homes gantry
	bool initGantry();
	//Moves to lurk position 
	bool prepareCatch();
	//Catch candy start at lurk position
	bool catchCandy(double angularVel, double angularOffset, double radius);
	//Moves to output and drop Gantry
	bool outputCandy();
	//Programm that performes wait patterns
	void waitingProgram(int times);
};
#endif