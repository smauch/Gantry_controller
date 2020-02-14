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
#include <array>
#include <Color.h>
#include <map>


#define PI 3.14159265

using namespace CML;


class Gantry
{
public:
	//Factor from mm to uunit
	const static short POS_FACTOR = 160;

	//Number of amplifiers
	const static short NUM_AMP = 3;
	const static short TOOL_AXIS = 2;
	const static short VALVE_OUT_PIN = 2;
	const static short PRESSURE_IN_PIN = 2;
	const static short PUMP_OUT_PIN = 1;

	//n-D fix Positions
	const static std::array<uunit, NUM_AMP> LURK_POS;
	const static std::array<uunit, NUM_AMP> HOME_POS;
	const static std::array<uunit, NUM_AMP> DROP_POS;
	const static std::array<uunit, NUM_AMP> DISC_CENTER_POS;
	const static std::array<uunit, NUM_AMP> DISC_DROP;
	const static std::array<uunit, NUM_AMP> STORAGE_BASE;
<<<<<<< HEAD
	const static std::array<uunit, NUM_AMP> WAIT_PAT_BASE;
	const static std::array<uunit, NUM_AMP> WAIT_PAT_BUFFER;
=======
	const static std::array<uunit, NUM_AMP> BUFFER_BASE;
	
>>>>>>> 89993a44bddb2045bdedbce595bf1d483471153b
	//2D fix Positions
	const static std::array<uunit, 2> DISC_RADIUS;

	//1D fix Positions
	const static uunit CATCH_Z_HEIGHT;
	const static uunit X_STORAGE;
	const static std::map<Colors, uunit> Y_STORAGE;

	//Constructor
	Gantry();
	~Gantry();

	bool networkSetup();

	bool attachAmpConifg(std::array<std::string, NUM_AMP> configPaths);
	//Loads amplifier CME2 configuration files and homes gantry
	bool initGantry();
	//Moves to lurk position 
	bool prepareCatch();
	//Catch candy start at lurk position
	bool catchRotary(double ang, double angVel, double pixelRadius, std::array<uunit, 3> dropPos, unsigned short maxTime=3000, bool debugMotion = false);

	//Catches candy from none moving point
	bool catchStatic (std::array<uunit, NUM_AMP> candyPos, std::array<uunit, NUM_AMP> targetPos);

	std::array<uunit, Gantry::NUM_AMP> getPos();

	bool fillTable(Colors color);

	bool handleError(const Error* err);

private:
	void showerr(const Error* err, const char* msg);
	//IXXAT USB to CAN adapter
	IxxatCANV3 can;
	//Upper level Can object
	CanOpen canOpen;
	// We use three Amp objects to control the three amplifiers in the system.
	Amp amp[NUM_AMP];
	//Create linkage object to handle the three axes simulatanously
	
		//Point Object for Point to point moves
	//Point<3> ampTargetPos;

	Linkage link;
	//Linkage config
	LinkSettings linkCfg;
	//Trajectory object
	GantryTrajectory trj;
	//CAN NodeIDs by the order X,Y,Z axis
	const short CAN_AXIS[NUM_AMP] = { 2, 3, 1 };
	//Paths to the CME2 generated config files
	std::array<std::string, NUM_AMP> ampConfigPath;
	//Home each axis seperatly due to may occuring under voltage
	const Error *homeAxis(unsigned short maxTime);
	//Trajectory planing with given angular velocity and angular of the disc
	//Method to perform point to point moves with S-Curve profile

	//The half of the Camera resolution 1080/2
	const static double PIXEL_RADIUS;

	const Error *setPump(bool state);
	const Error *setValve(bool state);
	const Error *ptpMove(std::array<uunit, NUM_AMP> targetPos, unsigned short maxTime = 5000);
	bool getCatched();

	void gantryLog(std::vector <std::string> message);

};
#endif