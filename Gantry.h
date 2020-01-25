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
#include "GantryTrajectory.h"

#define PI 3.14159265

using namespace CML;


class Gantry
{
public:
	//Number of amplifiers
	const static int NUM_AMP = 3;

	//Position consts
	static uunit LURK_POS[NUM_AMP];
	static uunit HOME_POS[NUM_AMP];
	static uunit DROP_POS[NUM_AMP];
	static uunit DISC_CENTER_POS[NUM_AMP];
	static uunit CATCH_Z_HEIGHT;
	//Minimum radius and maximum radius of the disc
	static uunit DISC_RADIUS[2];


	//Constructor
	Gantry();
	~Gantry();

	//Loads amplifier CME2 configuration files and homes gantry
	bool initGantry();
	//Moves to lurk position 
	bool prepareCatch();
	//Catch candy start at lurk position
	bool catchRotary(double angularVel, double angular, double pixelRadius, Point<3> targetPos, bool measureTime=false);

	//Catches candy from none moving point
	bool catchStatic (Point<3> candyPos, Point<3> targetPos);

	//position to positiion Move in uunit
	void ptpMove(Point<3> targetPos);
	uunit* getPos();

private:

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
	Point<3> ampTargetPos;
	//Trajectory object
	GantryTrajectory trj;
	//CAN NodeIDs by the order X,Y,Z axis
	int CAN_AXIS[NUM_AMP] = { 2, 3, 1 };
	//Paths to the CME2 generated config files
	std::string ampConfigPath[NUM_AMP] = { "./parameters/x_axis.ccx", "./parameters/y_axis.ccx", "./parameters/z_axis.ccx" };
	//Home each axis seperatly due to may occuring under voltage
	bool homeAxis();
	//Trajectory planing with given angular velocity and angular of the disc
	//Method to perform point to point moves with S-Curve profile

	//Variables for motions of candy
	double angular;
	double angularVel;
	double angularAcc;

	//The half of the Camera resolution 1080/2
	const static double PIXEL_RADIUS;


	//Fix positions
	bool setPump(bool state);
	bool setValve(bool state);
	bool getCatched();


};
#endif