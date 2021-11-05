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
#include <vector>
#include <tuple>
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
	//Home order
	const static std::array<unsigned short, NUM_AMP> HOME_ORDER;

	//n-D fix Positions
	const static std::array<uunit, NUM_AMP> LURK_POS;
	const static std::array<uunit, NUM_AMP> HOME_POS;
	const static std::array<uunit, NUM_AMP> DROP_POS;
	const static std::array<uunit, NUM_AMP> DISC_CENTER_POS;
	const static std::array<uunit, NUM_AMP> DISC_DROP;
	const static std::array<uunit, NUM_AMP> STORAGE_BASE;
	const static std::array<uunit, NUM_AMP> STORAGE_SAFE_POS;
	const static std::array<uunit, NUM_AMP> WAIT_PAT_BASE;

	const static std::array<uunit, NUM_AMP> CANDY_SIZE;
	//2D fix Positions
	const static std::array<uunit, 2> DISC_RADIUS;
	const static std::array<uunit, 2> STORAGE_HEIGHT;

	//1D fix Positions
	const static uunit CATCH_Z_HEIGHT;
	const static uunit X_STORAGE;
	
	const static std::map<Colors, uunit> Y_STORAGE;

	// Storage fill state
	std::map<Colors, int> fillState;

	//Constructor
	Gantry();
	~Gantry();

	struct UndervoltageException : public std::exception {
		const char* what() const throw() {
			return "Gantry has undervoltage";
		}
	};

	bool networkSetup();

	bool initAmps();

	void attachAmpConifg(std::array<std::string, NUM_AMP> configPaths);
	//Loads amplifier CME2 configuration files and homes gantry
	bool initGantry(unsigned int maxTimeHoming);
	//Moves to lurk position 
	bool prepareCatch();
	//Catch candy start at lurk position
	bool catchRotary(double ang, double angVel, float factorRadius, std::array<uunit, 3> dropPos, unsigned short maxTime=6000, bool debugMotion = false);

	//Catches candy from none moving point
	bool catchStatic (std::array<uunit, NUM_AMP> candyPos, std::array<uunit, NUM_AMP> targetPos);

	std::array<uunit, Gantry::NUM_AMP> getPos();

	bool fillTable(Colors color);

	bool placeOnTable(std::array<uunit, NUM_AMP> candyPos);

	bool disable();

	bool enable();

	bool maintenance();

	bool resetMaintenance();

	bool getUndervoltage();

	bool updateFillState();

	int getFillState(Colors color);

	//Home each axis seperatly due to may occuring under voltage
	const Error* homeAxis(unsigned int maxTime, std::array<unsigned short, NUM_AMP> axisOrder, bool saftyMove = true);

	

private:
	bool ampInitialized = false;
	//Global Status
	bool underVoltage = false;

	bool handleErr(const Error* err);

	std::string errDescription;

	std::vector <std::string> logMessage;

	//

	//IXXAT USB to CAN adapter
	IxxatCANV3 can;
	//Upper level Can object
	CanOpen canOpen;
	// We use three Amp objects to control the three amplifiers in the system.
	Amp amp[NUM_AMP];
	//Create linkage object to handle the three axes simulatanously
	CML::Linkage link;
	//Linkage config
	CML::LinkSettings linkCfg;
	//Profile config
	std::array<ProfileConfig, NUM_AMP> profConf;
	//Software position limits
	std::array<SoftPosLimit, NUM_AMP> posLimit;
	//CAN NodeIDs by the order X,Y,Z axis
	const std::map<std::string, short> AMP_MAP = { {"X-Axis",0}, {"Y-Axis",1}, {"Z-Axis",2} };
	const short CAN_AXIS[NUM_AMP] = { 2, 3, 1 };
	//Paths to the CME2 generated config files
	std::array<std::string, NUM_AMP> ampConfigPath;

	//Trajectory planing with given angular velocity and angular of the disc
	//Method to perform point to point moves with S-Curve profile
	GantryTrajectory trj;
	//The half of the Camera resolution 1080/2
	const static double PIXEL_RADIUS;

	bool setPump(bool state, short mode=0);
	const Error *setValve(bool state, int waitMs=0);
	bool ptpMove(std::array<uunit, NUM_AMP> targetPos, unsigned short maxTime = 5000);
	bool getCatched();
	void saftyMove();

	void gantryLog(std::vector<std::string> logMessage);

};
#endif