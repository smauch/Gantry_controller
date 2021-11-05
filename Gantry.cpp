#include "Gantry.h"
#include <chrono>
#include <thread>
#include <direct.h>
#include <fstream>
#include <regex> 
#include <string> 
#include <stdexcept>
#include <wtypes.h>


CML_NAMESPACE_USE();
//3D fix position initialization
const std::array<uunit, 3> Gantry::LURK_POS = { 138000, 0, 10000 };
const std::array<uunit, 3> Gantry::HOME_POS = { 0,0,0 };
const std::array<uunit, 3> Gantry::DROP_POS = { 11000, 22800, 0 };
const std::array<uunit, 3> Gantry::DISC_CENTER_POS = { 97400, 47900, 20000 };
const std::array<uunit, 3> Gantry::DISC_DROP = { 51000, 48000, 25900 };
const std::array<uunit, 3> Gantry::STORAGE_BASE = { 18800, 0, 0 };
const std::array<uunit, 3> Gantry::STORAGE_SAFE_POS = { 26000, 71000, 0 };
const std::array<uunit, 3> Gantry::WAIT_PAT_BASE = { 34000, 30000, 26500 };

const  std::array<uunit, 3> Gantry::CANDY_SIZE = { 8800, 6720, 1500 };
//2D fix position initialization
//@inner radius, @outer radius
const std::array<uunit, 2> Gantry::DISC_RADIUS = { 8000, 48000 };

//lower pos, @upper pos
const std::array<uunit, 2> Gantry::STORAGE_HEIGHT = { 3300, 27100 };
//1D fix position initialization
const uunit Gantry::CATCH_Z_HEIGHT = 26500;

const std::map<Colors, uunit> Gantry::Y_STORAGE = { {GREEN, 91100}, {RED, 83500}, {DARK_BLUE,75900}, {YELLOW, 68200},{BROWN, 60600},{LIGHT_BLUE, 53000} };
//Half of Camera resolution
double const Gantry::PIXEL_RADIUS = 540.0;
//Home order
const std::array<unsigned short, 3> Gantry::HOME_ORDER = { 2,1,0 };


Gantry::Gantry() {

}



Gantry::~Gantry()
{
	this->saftyMove();
	this->homeAxis(30000, HOME_ORDER);
	for (int i = 0; i < NUM_AMP; i++)
	{
		this->link[i].Disable(true);
	}
}

bool Gantry::handleErr(const Error* err)
{
	if (err && ampInitialized) {
		std::cerr << err->toString() << std::endl;
		//Before not every Amp is initialized don't loop over it
		try
		{
			for (int i = 0; i < NUM_AMP; i++)
			{
				amp[i].HaltMove();
				amp[i].Disable();
				amp[i].ClearHaltedMove();
				amp[i].ClearFaults();
			}
		}
		catch (const std::exception&)
		{

		}
		setValve(true, 500);
		const char* errCStr = err->toString();
		std::string errMsg((LPCTSTR)errCStr);
		errDescription = errMsg;
		logMessage.push_back("Error: " + errMsg);
		const int errId = err->GetID();

		//Amplifier under voltag
		if (errId == CMLERR_AmpError_UnderVolt) {
			underVoltage = true;
			throw UndervoltageException();
		}
		// The amplifier is disabled
		else if (errId == CMLERR_AmpError_Disabled) {
			for (int i = 0; i < NUM_AMP; i++)
			{
				amp[i].Enable();
			}
		}
		// Positive software limit is active.
		else if (errId == CMLERR_AmpError_NegSoftLim || errId == CMLERR_AmpError_PosSoftLim) {
			saftyMove();
			prepareCatch();
		}
		// Position tracking warning
		else if (errId == CMLERR_AmpError_VelWin || CMLERR_AmpError_TrackErr) {
			saftyMove();
			prepareCatch();
		}
		// Last trajectory aborted.
		else if (errId == CMLERR_AmpError_Abort) {
			saftyMove();
			prepareCatch();
		}
		// The amplifier did not complete the homing method successfull
		else if (errId == CMLERR_AmpError_HomingError) {
			homeAxis(30000, HOME_ORDER);
		}
		//Latching fault is active
		else if (errId == CMLERR_AmpError_Fault) {
			saftyMove();
			prepareCatch();
		}
		//for (int i = 0; i < NUM_AMP; i++)
		//{
		//	const Error* globalErr = this->amp[i].GetErrorStatus();
		//	const int globalErrId = globalErr->GetID();
		//	if (globalErrId != CMLERR_Error_OK && globalErrId != CMLERR_AmpError_UnderVolt && globalErrId != CMLERR_AmpError_Disabled) {
		//		// Worst case -  Error after error handling
		//		std::cerr << err->toString() << std::endl;
		//		throw std::runtime_error("Copley Error can not be handeled");
		//	}
		//}
		return true;
	}
	else {
		return false;
	}

}


/**
Attaches the amplifier files to the gantry object for initialization.

@param attachAmpConfig std array of file paths to CME2 ccx amp config.

*/
void Gantry::attachAmpConifg(std::array<std::string, NUM_AMP> configPaths)
{
	//Eventually modify paramter sothat this makes sense.
	if (configPaths.size() != NUM_AMP) {
		throw std::length_error("Number of config files do not fit to number of axis");
	}
	std::string CME_NAME = "Amp Name,";
	for (auto const& path : configPaths) {
		std::ifstream ampFile(path);
		std::string line;
		while (std::getline(ampFile, line)) {
			std::size_t pos = line.find(CME_NAME);
			if (pos != std::string::npos) {
				std::string realAmpName = line.substr(pos + CME_NAME.size(), line.size());
				short nrSearched = 0;
				for (auto const& [key, val] : AMP_MAP)
				{
					if (key == realAmpName) {
						this->ampConfigPath[val] = path;
						break;
					}
					else
					{
						nrSearched++;
					}
				}
				if (nrSearched >= NUM_AMP)
				{
					throw std::invalid_argument("Matching axis names not found in files");
				}
			}
		}
	}
}

/**
Initialization of can and high level can open object.

@returns bool true on sucess.
*/
bool Gantry::networkSetup()
{
	const Error* err = NULL;
	err = can.Open();
	if (handleErr(err)) {
		return false;
	}
	err = canOpen.Open(can);
	if (handleErr(err)) {
		return false;
	}
	return true;
}

bool Gantry::initAmps()
{
	const Error* err = NULL;
	for (short i = 0; i < NUM_AMP; i++)
	{
		err = amp[i].Init(canOpen, CAN_AXIS[i]);
		if (handleErr(err)) {
			return false;
		}
		int line = 0;
		err = amp[i].LoadFromFile(ampConfigPath[i].c_str(), line);
		if (handleErr(err)) {
			return false;
		}
		err = amp[i].SetHaltMode(HALT_QUICKSTOP);
		if (handleErr(err)) {
			return false;
		}
		err = amp[i].GetProfileConfig(profConf[i]);
		if (handleErr(err)) {
			return false;
		}
		err = amp[i].GetSoftLimits(posLimit[i]);
		if (handleErr(err)) {
			return false;
		}
	}
	err = link.Init(NUM_AMP, amp);
	if (handleErr(err)) {
		return false;
	}

	//Operation that will be performed if window is exceeded
	linkCfg.haltOnPosWarn = true;
	linkCfg.haltOnVelWin = true;
	err = link.Configure(linkCfg);
	if (handleErr(err)) {
		return false;
	}

	//Movement limitations setup
	uunit maxVel = profConf[0].vel;
	uunit maxAcc = profConf[0].acc;
	uunit maxDec = profConf[0].dec;
	uunit maxJrk = profConf[0].jrk;
	for (auto const& values : profConf) {
		if (values.vel < maxVel) {
			maxVel = values.vel;
		}
		else if (values.acc < maxAcc)
		{
			maxAcc = values.acc;
		}
		else if (values.dec < maxDec)
		{
			maxDec = values.dec;
		}
		else if (values.jrk < maxJrk) {
			maxJrk = values.jrk;
		}
	}
	err = link.SetMoveLimits(maxVel, maxAcc * 0.75, maxDec * 0.75, maxJrk);
	if (handleErr(err)) {
		return false;
	}
	//Everything is fine
	ampInitialized = true;
	return true;
}


/**
Initializes the axes of a gantry.

@param time that should not be exceeded by homing.

@returns CML Error object
*/
bool Gantry::initGantry(unsigned int maxTimeHoming)
{
	const Error* err = NULL;
	if (!ampInitialized) {
		initAmps();
	}
	for (int i = 0; i < NUM_AMP; i++)
	{
		amp[i].Enable();
	}
	err = homeAxis(maxTimeHoming, HOME_ORDER);
	if (handleErr(err)) {
		return false;
	}
	updateFillState();
	return true;
}


/**
Homes all axes in reverse order to prevent collisions.

@param maxTime The maximal allowed time (in milliseconds) for homing per axis.

@returns CML Error object
*/
const Error *Gantry::homeAxis(unsigned int maxTime, std::array<unsigned short, NUM_AMP> axisOrder, bool saftyMove)
{
	const Error* err = NULL;

	if (saftyMove) {
	//	this->saftyMove();
	}

	for (auto const& id : axisOrder) {
		err = amp[id].GoHome();
		if (err) {
			return err;
		}
		err = amp[id].WaitHomeDone(maxTime);
		if (err) {
			return err;
		}
	}
	//Everything is fine
	return err;
}

/**
Catches a Candy that is moving on the rotary table

@param angularVel the angular velocity of the candy in rad/ms
@param angular the angular of the candy relative to the camera perspective
@param radius the radius of the candy relative to the disc center in pixel
@param targetPos the position where the candy should be dropped

@return true if the candy got chatched and is dropped at targePos
*/
bool Gantry::catchRotary(double ang, double angVel, float factorRadius, std::array<uunit, 3> dropPos, unsigned short maxTime, bool debugMotion)
{
	const Error* err = NULL; 
	auto start_move = std::chrono::high_resolution_clock::now();
	//Radius scaling from pixel to uunit
	double radius = DISC_RADIUS[1] * factorRadius;
	if (radius < DISC_RADIUS[0] || radius > DISC_RADIUS[1]){
		throw std::out_of_range("Radius out of range");
	}
	setPump(true, 1);
	setValve(false);

	//Trajectory object
	trj.attachMoveLimits(this->posLimit);

	trj.calcMovement(getPos(), radius, ang, angVel, dropPos, maxTime);

	auto stop_clac = std::chrono::high_resolution_clock::now();
	err = link.SendTrajectory(trj, false);
	if (handleErr(err)) {
		return false;
	}
	if (debugMotion) {
		trj.saveTrj(radius, ang, angVel);
	}
	err = link.StartMove();
	if (handleErr(err)) {
		return false;
	}
	err = link.WaitMoveDone(2*maxTime);
	if (handleErr(err)) {
		return false;
	}
	setPump(false);

	auto stop_move = std::chrono::high_resolution_clock::now();
	auto duration_calc = std::chrono::duration_cast<std::chrono::milliseconds>(stop_clac - start_move);
	auto duration_move = std::chrono::duration_cast<std::chrono::milliseconds>(stop_move - stop_clac);
	if (debugMotion) {
		std::vector <std::string> message;
		message.push_back("Calculation lasts" + std::to_string(duration_calc.count()) + " miliseconds");
		message.push_back("Movement lasts " + std::to_string(duration_move.count()) + " miliseconds");
		gantryLog(message);
	}

	if (getCatched())
	{
		if (dropPos == Gantry::DROP_POS) {
			dropPos[2] += 2000;
			ptpMove(dropPos);
			setValve(true, 500);
			dropPos[2] -= 2000;
			ptpMove(dropPos);
		}
		else {
			setValve(true, 500);
			dropPos[2] -= 5000;
			ptpMove(dropPos);
		}
		return true;
	}
	else
	{
		setValve(true, 1000);
		return false;
	}
}


bool Gantry::prepareCatch()
{
	if (!ptpMove(LURK_POS)) {
		return false;
	}
	return true;
}

/**
* Catching candy from an none moving fix position and drop it at a target position
*
*@param candyPos is the position of the candy in uunits
*@param targetPos is the position where the candy should be dropped of
*/
bool Gantry::catchStatic(std::array<uunit, NUM_AMP> candyPos, std::array<uunit, NUM_AMP> targetPos)
{
	const Error* err = NULL;
	candyPos[TOOL_AXIS] -= 5000;
	if(!ptpMove(candyPos)) {
		return false;
	}
	setValve(false);
	setPump(true);
	candyPos[TOOL_AXIS] += 5000;
	if (!ptpMove(candyPos)) {
		return false;
	}
	candyPos[TOOL_AXIS] -= 5000;
	if (!ptpMove(candyPos)) {
		return false;
	}
	setPump(false);

	targetPos[TOOL_AXIS] -= 5000;
	if (!ptpMove(targetPos)) {
		return false;
	}

	targetPos[TOOL_AXIS] += 5000;
	if (!ptpMove(targetPos)) {
		return false;
	}
	setValve(true,50);


	targetPos[TOOL_AXIS] -= 5000;
	if (!ptpMove(targetPos)) {
		return false;
	}

	return true;
}


/**
*Does a S-Curve move to the target Pos 

@param targetPos Position object that where the gantry should move to.
@param maxTime The maxTime that should not be exceeded.
*/
bool Gantry::ptpMove(std::array<uunit, NUM_AMP> targetPos, unsigned short maxTime)
{
	const Error* err = NULL;
	Point<targetPos.size()> ampTarget;
	for (size_t i = 0; i < targetPos.size(); i++)
	{
		ampTarget[i] = targetPos[i];
	}
	err = link.MoveTo(ampTarget);
	if (handleErr(err)) {
		return false;
	}
	err = link.WaitMoveDone(maxTime);
	if (handleErr(err)) {
		return false;
	}
	return true;
}

std::array<uunit, Gantry::NUM_AMP> Gantry::getPos()
{
	std::array<uunit, NUM_AMP> actPos;
	for (short i = 0; i < NUM_AMP; i++)
	{
		link[i].GetPositionActual(actPos[i]);
	}
	return actPos;
}

bool Gantry::fillTable(Colors color)
{
	const Error* err = NULL;
	if (!ptpMove(STORAGE_SAFE_POS)) {
		return false;
	}
	std::array<uunit, 3> targetPos = STORAGE_BASE;
	targetPos[1] = Gantry::Y_STORAGE.at(color);
	if (!ptpMove(targetPos)) {
		return false;
	}
	ProfileConfigTrap carefulCatch;
	carefulCatch.acc = 50000;
	carefulCatch.dec = 50000;
	carefulCatch.vel = 10000;
	carefulCatch.pos = Gantry::CATCH_Z_HEIGHT;
	link[TOOL_AXIS].SetupMove(carefulCatch);
	link[TOOL_AXIS].SetPositionWarnWindow(35);
	EventAny posWarn(AMPEVENT_POSWARN);
	link[TOOL_AXIS].StartMove();
	err = link[TOOL_AXIS].WaitEvent(posWarn, 6000);
	if (!err) {
		//std::cout << "performing quickstop" << std::endl;
		link[TOOL_AXIS].HaltMove();
		link[TOOL_AXIS].SetPositionWarnWindow(4096);
		setValve(false);
		setPump(true);
		if (!ptpMove(targetPos)) {
			return false;
		}
		setPump(false);
		targetPos[0] = targetPos[0] + 15000;
		if (!ptpMove(targetPos)) {
			return false;
		}
		if (!ptpMove(DISC_DROP)) {
			return false;
		}
		double xRandom = ((double)std::rand() / (RAND_MAX));
		carefulCatch.vel = 50000;
		carefulCatch.pos = DISC_CENTER_POS[0] - DISC_RADIUS[1] / 2.0;
		err = link[0].SetupMove(carefulCatch);
		if (handleErr(err)) {
			return false;
		}
		err = link[0].StartMove();
		if (handleErr(err)) {
			return false;
		}
		err = link.WaitMoveDone(10000);
		if (handleErr(err)) {
			return false;
		}
		// Wait!
		setValve(true, 500);

		if (!ptpMove(LURK_POS)) {
			return false;
		}

		//Save access
		if (fillState.find(color) != fillState.end()) {
			fillState.at(color)--;
		}

		return true;
	}
	else {
		//TODO check if this is needed
		err = link.WaitMoveDone(10000);
		if (handleErr(err)) {
			return false;
		}
		link[2].SetPositionWarnWindow(4096);
		if (handleErr(err)) {
			return false;
		}
		if (!ptpMove(targetPos)) {
			return false;
		}

		targetPos[0] = targetPos[0] + 15000;
		if (!ptpMove(targetPos)) {
			return false;
		}
		//Fill state empty
		fillState[color] = 0;

		prepareCatch();

		return false;
	}	
}

bool Gantry::placeOnTable(std::array<uunit, NUM_AMP> candyPos)
{
	const Error* err = NULL;

	candyPos[TOOL_AXIS] -= 8000;
	if (!ptpMove(candyPos)) {
		return false;
	}
	setValve(false);
	setPump(true);
	candyPos[TOOL_AXIS] += 8000;
	if (!ptpMove(candyPos)) {
		return false;
	}
	candyPos[TOOL_AXIS] -= 8000;
	if (!ptpMove(candyPos)) {
		return false;
	}
	setPump(false);

	if (!getCatched()) {
		return false;
	}

	if (!ptpMove(DISC_DROP)) {
		return false;
	}

	ProfileConfigTrap carefulCatch;
	carefulCatch.acc = 50000;
	carefulCatch.dec = 50000;
	carefulCatch.vel = 10000;
	carefulCatch.pos = DISC_CENTER_POS[0] - DISC_RADIUS[1] / 2.0;
	err = link[0].SetupMove(carefulCatch);
	if (handleErr(err)) {
		return false;
	}
	err = link[0].StartMove();
	if (handleErr(err)) {
		return false;
	}
	err = link.WaitMoveDone(10000);
	if (handleErr(err)) {
		return false;
	}
	setValve(true);
	candyPos = DISC_DROP;
	candyPos[0] = carefulCatch.pos;
	candyPos[2] -= 5000;
	if (!ptpMove(candyPos)) {
		return false;
	}
	return true;
}

bool Gantry::disable()
{
	for (int i = 0; i < NUM_AMP; i++)
	{
		amp[i].HaltMove();
		amp[i].Disable();
	}
	return true;
}

bool Gantry::enable()
{
	for (int i = 0; i < NUM_AMP; i++)
	{
		amp[i].Enable();
	}
	return true;
}

bool Gantry::maintenance()
{
	this->prepareCatch();
	this->disable();
	return true;
}

bool Gantry::resetMaintenance()
{
	this->enable();
	this->homeAxis(30000, HOME_ORDER);
	this->prepareCatch();
	return true;
}

bool Gantry::getUndervoltage()
{
	const Error* err = amp[1].GetErrorStatus();
	const int errId = err->GetID();
	if ( errId == CMLERR_AmpError_UnderVolt || errId == CMLERR_AmpFault_UnderVolt) {
		return true;
	}
	else {
		return false;
	}
}

bool Gantry::updateFillState()
{
	const Error* err = NULL;
	prepareCatch();
	for (auto it = Gantry::Y_STORAGE.begin(); it != Gantry::Y_STORAGE.end(); it++)
	{
		std::array<uunit, 3> targetPos = STORAGE_BASE;
		targetPos[1] = Gantry::Y_STORAGE.at(it->first);
		if (!ptpMove(targetPos)) {
			return false;
		}
		ProfileConfigTrap carefulCatch;
		carefulCatch.acc = 50000;
		carefulCatch.dec = 50000;
		carefulCatch.vel = 10000;
		carefulCatch.pos = (Gantry::STORAGE_HEIGHT[1] - (Gantry::CANDY_SIZE[2] / 2));
		link[TOOL_AXIS].SetupMove(carefulCatch);
		link[TOOL_AXIS].SetPositionWarnWindow(35);
		EventAny posWarn(AMPEVENT_POSWARN);
		link[TOOL_AXIS].StartMove();
		//Be careful with these times, they are linked to the Toolaxis velocity and the Storage height
		err = link[TOOL_AXIS].WaitEvent(posWarn, 4000);
		if (!err) {
		
			link[TOOL_AXIS].HaltMove();

			link[TOOL_AXIS].SetPositionWarnWindow(4096);

			uunit fillHightPosition;
			err = link[TOOL_AXIS].GetPositionActual(fillHightPosition);

			int numCandies = int(STORAGE_HEIGHT[1] - fillHightPosition) / int(CANDY_SIZE[2]);
			fillState[it->first] = numCandies;

			if (!ptpMove(targetPos)) {
				return false;
			}
			
		}
		else {
			//Be careful with these times, they are linked to the Toolaxis velocity and the Storage height
			err = link.WaitMoveDone(5000);
			if (handleErr(err)) {
				return false;
			}
			link[2].SetPositionWarnWindow(4096);
			if (handleErr(err)) {
				return false;
			}
			if (!ptpMove(targetPos)) {
				return false;
			}
			//Fill state empty
			fillState[it->first] = 0;
		}
	}
	prepareCatch();
	return true;
}

int Gantry::getFillState(Colors color)
{
	return this->fillState[color];
}





/**
Set the pump to specific state on/off

@return CML Error object
*/
bool Gantry::setPump(bool state, short mode) {
	const Error* err = NULL;
	if (!state) {
		err = amp[TOOL_AXIS].SetOutputConfig(PUMP_OUT_PIN, OUTCFG_MANUAL_L);
		if (handleErr(err)) {
			return false;
		}
	}
	else {
		if (mode == 0) {
			err = amp[TOOL_AXIS].SetOutputConfig(PUMP_OUT_PIN, OUTCFG_MANUAL_H);
			if (handleErr(err)) {
				return false;
			}
		}
		else if (mode == 1) {
			//Activates auto pump at 90% of catch heigh, stay on for 500ms
			err = amp[2].SetOutputConfig(PUMP_OUT_PIN, OUTCFG_POSITION_TRIG_LOW2HIGH, CATCH_Z_HEIGHT * 0.9, 500);
			if (handleErr(err)) {
				return false;
			}
		}
	}
	return true;
}


/**
Set the valve to specific state on/off

@return CML Error object
*/
const Error* Gantry::setValve(bool state, int waitMs)
{
	const Error* err = NULL;
	if (state)
		err = amp[TOOL_AXIS].SetOutputConfig(VALVE_OUT_PIN, OUTCFG_MANUAL_H);
	else
		err = amp[TOOL_AXIS].SetOutputConfig(VALVE_OUT_PIN, OUTCFG_MANUAL_L);
	if(waitMs)
		std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
	return err;

}


/**
Check if the tool axis caught something.

@return bool catched or not
*/
bool Gantry::getCatched()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	const Error* err = NULL;
	uint16 currentInput;
	err = amp[TOOL_AXIS].GetInputs(currentInput);
	if (handleErr(err)) {
		return false;
	}
	currentInput = currentInput & PRESSURE_IN_PIN;
	if (currentInput)
		return true;
	else
		return false;
}

void Gantry::saftyMove()
{
	const Error* err = NULL;
	ProfileConfigTrap safePosMove;
	safePosMove.acc = 50000;
	safePosMove.dec = 50000;
	safePosMove.vel = 10000;
	safePosMove.pos = Gantry::DISC_DROP[0];

	amp[0].SetupMove(safePosMove);
	//TODO Check this Val
	amp[0].SetPositionWarnWindow(100);
	EventAny posWarn(AMPEVENT_POSWARN);
	amp[0].StartMove();
	err = link[TOOL_AXIS].WaitEvent(posWarn, 6000);
	if (!err) {
		amp[0].HaltMove();
		amp[0].SetPositionWarnWindow(4096);
	}
}

void Gantry::gantryLog(std::vector<std::string> logMessage)
{
	char rootDir[] = ".\\log";
	if (!std::filesystem::exists(rootDir)) {
		if (!_mkdir(rootDir)) {
			return;
		}
	}
	std::ofstream logFile;
	logFile.open(".\\log\\gantry.txt", std::ios::app);
	if (logFile.is_open()) {
		for (auto const& line : logMessage) {
			logFile << line << std::endl;
		}
		logFile.close();
	}
	return;
}



