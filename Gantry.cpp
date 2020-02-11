#include "Gantry.h"
#include <chrono>
#include <thread>
#include <direct.h>
#include <fstream>


CML_NAMESPACE_USE();

//3D fix position initialization
const std::array<uunit, 3> Gantry::LURK_POS = { 138000, 0, 10000 };
const std::array<uunit, 3> Gantry::HOME_POS = { 0,0,0 };
const std::array<uunit, 3> Gantry::DROP_POS = { 0, 46350, 10000 };
const std::array<uunit, 3> Gantry::DISC_CENTER_POS = { 110000, 46350, 20000 };
const std::array<uunit, 3> Gantry::DISC_DROP = { 60000, 46350, 25000 };
const std::array<uunit, 3> Gantry::STORAGE_BASE = { 9100, 0, 18000 };
//2D fix position initialization
//@inner radius, @outer radius
const std::array<uunit, 2> Gantry::DISC_RADIUS = { 8000, 46350 };
//1D fix position initialization
const uunit Gantry::CATCH_Z_HEIGHT = 25250;
const std::map<Colors, uunit> Gantry::Y_STORAGE = {{GREEN, 69800}, {RED, 77100}, {YELLOW,84600}};
//Half of Camera resolution
double const Gantry::PIXEL_RADIUS = 540.0;

//static void showerr(const Error* err, const char* msg) {
//	if (!err) return;
//	
//	printf("Error: %s - %s\n", msg, err->toString());
//	//exit(1);
//};

Gantry::Gantry() {

}



Gantry::~Gantry()
{
	const Error* err;
	err = ptpMove(HOME_POS);
	for (int i = 0; i < NUM_AMP; i++)
	{
		this->link[i].Disable(true);
	}
}

void Gantry::showerr(const Error* err, const char* msg)
{
	if (!err) return;
	printf("Error: %s - %s\n", msg, err->toString());
}



bool Gantry::networkSetup()
{
	const Error* err;
	err = can.Open();
	if (err)
		return false;
	err = canOpen.Open(can);
	if (err)
		return false;
	return true;
}

bool Gantry::attachAmpConifg(std::array<std::string, NUM_AMP> configPaths)
{
	this->ampConfigPath = configPaths;
	return false;
}


/**
Initializes the axes of a gantry.

@param ampConfigPath paths to CME2 amplifier config files.

@returns CML Error object
*/
bool Gantry::initGantry()
{	
	const Error* err;
	for (short i = 0; i < NUM_AMP; i++)
	{
		err = amp[i].Init(canOpen, CAN_AXIS[i]);
		if (err)
			return false;
	}
	if (ampConfigPath.size() != NUM_AMP)
		return false;
	for (short i = 0; i < NUM_AMP; i++)
	{
		int line;
		err = amp[i].LoadFromFile(ampConfigPath[i].c_str(), line);
		if (err)
			return false;
	}
	err = link.Init(NUM_AMP, amp);
	if (err)
		return false;
	err = homeAxis(30000);
	if (err)
		return false;
	//Operation that will be performed if window is exceeded
	linkCfg.haltOnPosWarn = true;
	linkCfg.haltOnVelWin = true;
	err = link.Configure(linkCfg);
	if (err)
		return false;
	for (short i = 0; i < NUM_AMP; i++)
	{
		err = link[i].SetHaltMode(HALT_QUICKSTOP);
		if (err)
			return false;
	}
	//Movement limitations setup
	ProfileConfig profConf;
	err = amp[0].GetProfileConfig(profConf);
	if (err)
		return false;
	err = link.SetMoveLimits(profConf.vel*0.75, profConf.acc*0.5, profConf.dec*0.5, profConf.jrk*0.5);
	if (err)
		return false;
	return true;
}


/**
Homes all axes in reverse order to prevent collisions.

@param maxTime The maximal allowed time (in milliseconds) for homing per axis.

@returns CML Error object
*/
const Error *Gantry::homeAxis(unsigned short maxTime)
{
	const Error* err = 0;
	for (short i = NUM_AMP-1; i >= 0; i--)
	{
		err = link[i].GoHome();
		if (err)
			return err;
	
		err = link[i].WaitMoveDone(maxTime);
		if (err)
			return err;
	}
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
bool Gantry::catchRotary(double ang, double angVel, double pixelRadius, std::array<uunit, 3> dropPos, unsigned short maxTime, bool debugMotion)
{
	const Error* err;
	auto start_move = std::chrono::high_resolution_clock::now();
	//Radius scaling from pixel to uunit
	double radius = DISC_RADIUS[1] * (pixelRadius / PIXEL_RADIUS);
	if (radius < DISC_RADIUS[0] || radius > DISC_RADIUS[1]){
		return false;
	}
	//Activates auto pump at 90% of catch heigh, stay on for 500ms
	err = amp[TOOL_AXIS].SetOutputConfig(PUMP_OUT_PIN, OUTCFG_POSITION_TRIG_LOW2HIGH, CATCH_Z_HEIGHT*0.9, 500);
	if (err) {
		return false;
	}
	setValve(false);
	trj.calcMovement(getPos(), radius, ang, angVel, dropPos, maxTime);
	auto stop_clac = std::chrono::high_resolution_clock::now();
	err = link.SendTrajectory(trj, false);
	if (err) {
		return false;
	}
	if (debugMotion) {
		trj.saveTrj();
	}
	err = link.StartMove();
	if (err) {
		return false;
	}
	err = link.WaitMoveDone(2*maxTime);
	if (err) {
		return false;
	}
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
		setValve(true);
		err = ptpMove(LURK_POS);
		return true;
	}
	else
	{
		setValve(true);
		err = ptpMove(LURK_POS);
		return false;
	}
}


bool Gantry::prepareCatch()
{
	const Error* err;
	this->link.ClearLatchedError();
	err = ptpMove(LURK_POS);
	if (err) {
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
	const Error* err;
	candyPos[TOOL_AXIS] -= 1000;
	err = ptpMove(candyPos);
	if (err){
		return false;
	}
	err = setValve(false);
	if (err) {
		return false;
	}
	err = setPump(true);
	if (err) {
		return false;
	}
	candyPos[TOOL_AXIS] += 1000;
	err = ptpMove(candyPos);
	if (err) {
		return false;
	}
	candyPos[TOOL_AXIS] -= 1000;
	err = ptpMove(candyPos);
	if (err) {
		return false;
	}
	err = setPump(false);
	if (err) {
		return false;
	}
	err = ptpMove(targetPos);
	if (err) {
		return false;
	}
	err = setValve(true);
	if (err) {
		return false;
	}
	return true;
}


/**
*Does a S-Curve move to the target Pos 

@param targetPos Position object that where the gantry should move to.
@param maxTime The maxTime that should not be exceeded.
*/
const Error *Gantry::ptpMove(std::array<uunit, NUM_AMP> targetPos, unsigned short maxTime)
{
	const Error* err;
	Point<targetPos.size()> ampTarget;
	for (size_t i = 0; i < targetPos.size(); i++)
	{
		ampTarget[i] = targetPos[i];
	}
	err = link.MoveTo(ampTarget);
	if (err) {
		return err;
	}
	err = link.WaitMoveDone(maxTime);
	if (err) {
		return err;
	}
	return err;
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
	const Error* err;
	std::array<uunit, 3> targetPos = STORAGE_BASE;
	targetPos[1] = Gantry::Y_STORAGE.at(color);
	err = ptpMove(targetPos);
	if (err) {
		return false;
	}
	ProfileConfigTrap carefulCatch;
	carefulCatch.acc = 5000;
	carefulCatch.dec = 5000;
	carefulCatch.vel = 5000;
	carefulCatch.pos = CATCH_Z_HEIGHT;
	link[TOOL_AXIS].SetupMove(carefulCatch);
	link[TOOL_AXIS].SetPositionWarnWindow(80);
	EventAny posWarn(AMPEVENT_POSWARN);
	link[TOOL_AXIS].StartMove();
	err = link[TOOL_AXIS].WaitEvent(posWarn, 3000);
	if (!err) {
		//std::cout << "performing quickstop" << std::endl;
		link[TOOL_AXIS].HaltMove();
		link[TOOL_AXIS].SetPositionWarnWindow(4096);
		setValve(false);
		setPump(true);
		err = ptpMove(targetPos);
		setPump(false);
		err = ptpMove(DISC_DROP);
		double xRandom = ((double)std::rand() / (RAND_MAX));
		carefulCatch.vel = 15000;
		carefulCatch.pos = DISC_CENTER_POS[0] - DISC_RADIUS[1] / 2.0;
		link[0].SetupMove(carefulCatch);
		link[0].StartMove();
		err = link.WaitMoveDone(10000);
		this->setValve(true);
		err = ptpMove(LURK_POS);

		return true;
	}
	else {
		err = this->link.WaitMoveDone(10000);
		this->link[2].SetPositionWarnWindow(4096);
		showerr(err, "Waiting for move to finish");
		std::cout << "empty" << std::endl;
		err = ptpMove(targetPos);
		return false;
	}	
}

bool Gantry::handleError(const Error* err)
{
	return false;
}



/**
Set the pump to specific state on/off

@return CML Error object
*/
const Error *Gantry::setPump(bool state)
{
	const Error* err;
	if (state)
		err = amp[TOOL_AXIS].SetOutputConfig(PUMP_OUT_PIN, OUTCFG_MANUAL_H);
	else
		err = amp[TOOL_AXIS].SetOutputConfig(PUMP_OUT_PIN, OUTCFG_MANUAL_L);
	return err;
}


/**
Set the valve to specific state on/off

@return CML Error object
*/
const Error* Gantry::setValve(bool state)
{
	const Error* err;
	if (state)
		err = amp[TOOL_AXIS].SetOutputConfig(VALVE_OUT_PIN, OUTCFG_MANUAL_H);
	else
		err = amp[TOOL_AXIS].SetOutputConfig(VALVE_OUT_PIN, OUTCFG_MANUAL_L);
	return err;
}


/**
Check if the tool axis caught something.

@return bool catched or not
*/
bool Gantry::getCatched()
{
	const Error* err;
	uint16 currentInput;
	err = amp[TOOL_AXIS].GetInputs(currentInput);
	currentInput = currentInput & PRESSURE_IN_PIN;
	if (currentInput && !err)
		return true;
	else
		return false;
}

void Gantry::gantryLog(std::vector <std::string> message)
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
		for (auto const& line : message) {
			logFile << line << std::endl;
		}
		logFile.close();
	}
	return;
}



