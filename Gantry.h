#ifndef GANTRY_H 
#define GANTRY_H

#include <string>
#include "dependencies/inc/CML.h"
#include "dependencies/inc/can/can_ixxat_v3.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <filesystem>
#include <math.h>   
#include "GantryTrajectory.h"

#define PI 3.14159265

using namespace CML;

class Gantry
{
	
private:
	static const int numAmp = 3;
	//Error obj to show error that are generated from CML
	const Error* err;
	//IXXAT USB to CAN adapter
	IxxatCANV3 can;
	//Upper level Can object
	CanOpen canOpen;
	// We use three Amp objects to control the three amplifiers in the system.
	Amp amp[numAmp];
	//Create linkage object to handle the three axes simulatanously
	Linkage link;
	//Point Object for Point to point moves
	Point<3> pos;
	//Trajectory object
	GantryTrajectory trj;
	//CAN NodeIDs by the order X,Y,Z axis
	int CAN_AXIS[numAmp] = { 2, 3, 1 };
	//Paths to the CME2 generated config files
	std::string ampConfigPath[numAmp] = { "./parameters/x_axis.ccx", "./parameters/y_axis.ccx", "./parameters/z_axis.ccx" };
	//Home each axis seperatly due to may occuring under voltage
	bool homeAxis();
	//Trajectory planing with given angular velocity and angular of the disc
	void calcMovement(double angularVelOffset, double angularOffset, double radius);
	//Method to perform point to point moves with S-Curve profile
	void ptpMove(const int arr[3]);
	//Position consts
	const int lurkPos[3] = { 138000, 0, 15000 };
	const int homePos[3] = { 0,0,0 };
	const int dropPos[3] = { 0,46350,0 };
	const int discCenterPos[3] = { 110000, 46350, 20000 };
	const int CATCH_Z_HEIGHT = 25250;
	//Minimum radius and maximum radius of the disc
	const int discRadius[2] = {8000, 46350 };
	double angular;
	double angularVel;
	double angularAcc;
	//It is therefore important to ensure that the trajectory object passed here will remain
	//valid(i.e. not be deallocated) until the linkage has called the LinkTrajectory.Finish() method

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