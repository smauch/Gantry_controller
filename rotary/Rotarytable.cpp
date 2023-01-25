#ifdef __cplusplus
extern "C"
{
#endif
#if _MSC_VER <= 1200
#include"objbase.h"
#include"initguid.h"
#else
#define INITGUID
#include"guiddef.h"
#undef  INITGUID
#endif
#ifdef __cplusplus
}
#endif

#include "vciguid.h"
#include "XatBrds.h"
#include "objbase.h"
#include "initguid.h"
#include "CopError.h"
#include "copcmd.h"
#include "Cop.h"
#include <math.h>
#define _USE_MATH_DEFINES

#include "rotarytable.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <bitset>


RotaryTable::RotaryTable()
{
}

RotaryTable::~RotaryTable() {
	this->stopMovement(1000);
}

bool RotaryTable::initNetwork()
{
	//-----------This Constructor initalizes the can-board, the interface and adding the node to the can-network and prepares the motor for the rotations---------------------

	//Initializing the board; in case of any error, checking the canopen masterapi manual 2.2
	m_Board_Hdl = 1;
	m_boardtype = GUID_USB2CANV2_DEVICE;
	m_boardID = COP_1stBOARD;
	m_canline = COP_FIRSTLINE;
	m_res = 0;

	//Initializing the interface
	m_baudtable = COP_k_BAUD_CIA;
	m_baudrate = COP_k_1000_KB;
	m_node_no_master = 0;
	m_hbtime = 0;
	m_AddFeatures = COP_k_NO_FEATURES;

	//Adding node to can network
	m_NgOrHb = COP_k_HEARTBEAT;
	m_GuardHeartbeatTime = 500;
	m_lifetimefactor = 0;
	m_node_no_slave = 127;
	m_sdo_no = COP_k_DEFAULT_SDO;
	m_mode = COP_k_NO_BLOCKTRANSFER;

	//Initializsation of board
	// TODO what happens if board not connected? Raise exception
	m_res = COP_InitBoard(&m_Board_Hdl, &m_boardtype, &m_boardID, m_canline);
	if (m_res != COP_k_OK) {
		return true;
	}
	m_res = COP_InitInterface(m_Board_Hdl, m_baudtable, m_baudrate, m_node_no_master, m_hbtime, m_AddFeatures);
	
	if (m_res != COP_k_OK) {
		return true;
	}
	m_res = COP_AddNode(m_Board_Hdl, m_node_no_slave, m_NgOrHb, m_GuardHeartbeatTime + 100, m_lifetimefactor);
	if (m_res != COP_k_OK) {
		return true;
	}
	return false;
}

bool RotaryTable::initMotor()
{
	try
	{
		// Stop motor and disable amplifier
		writeSDO(0x4000, 0x02, 0x72);
		// Restore default parameters
		writeSDO(0x4000, 0x01, 0x82);
		//Set Pos to 0
		writeSDO(0x4762, 0x1, 0x0);
	}
	catch (const CanOpenErr& e)
	{
		return false;
	}
	return checkErr();
}

void RotaryTable::cleanErr() {
	// Clean error register
	writeSDO(0x4000, 0x02, 0x01);
}

bool RotaryTable::checkErr()
{
	int err = 0;
	readSDO(0x4001, 0x01, &err);
	// Mask due to error register is only 16bit
	err &= 0xFFFF;
	if (err) {
		std::cerr << "BG Motor Error: " << std::hex << err << std::endl;
		cleanErr();
		return true;
	}
	else
	{
		return false;
	}
}

/*************************Function for setting the velocity of the servo motor in U/min*************************************************/
bool RotaryTable::startVelMode(int velocity, int acceleration, int decceleration) {
	cleanErr();
	try
	{
		writeSDO(0x4000, 0x11, velocity);
		writeSDO(0x4341, 1, acceleration);
		writeSDO(0x4343, 1, decceleration);
		writeSDO(0x4000, 0x02, 0x74);
	}
	catch (const CanOpenErr& e)
	{
		return false;
	}
	return checkErr();
}

bool RotaryTable::startRelMove(double angular, int velocity)
{
	cleanErr();
	int increment = int(4096.0 * angular * 36.0 / (2.0 * M_PI));
	try
	{
		writeSDO(0x4000, 0x12, increment);
		writeSDO(0x4000, 0x11, velocity);
		writeSDO(0x4000, 0x02, 0x77);
	}
	catch (const CanOpenErr& e)
	{
		return false;
	}
	return checkErr();
}


bool RotaryTable::stopMovement(int timeout) {
	// Stop motor and disable amplifier
	auto begin = std::chrono::high_resolution_clock::now();
	startVelMode(0);
	while (isMoving() || (getMotorVel() > 1)) {
		auto end = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() > timeout) {
			std::cerr << "Timeout while waiting for BG 75 to stop." << std::endl;
			if (!writeSDO(0x4000, 0x02, 0x72)) {
				return false;
			}
			break;
		}
	}
	return checkErr();
}

double RotaryTable::getTableAngVel()
{
	int actRPM = 0;
	int targetRPM = 0;
	readSDO(0x4A04, 0x02, &actRPM);
	readSDO(0x4000, 0x11, &targetRPM);
	double angVel = (((actRPM / 36.0) * 2.0 * M_PI) / 60.0);
	std::cerr << "Target Velocity: " << targetRPM << "actual RPM: " << actRPM << std::endl;
	return angVel;
}

int RotaryTable::getMotorVel()
{
	int actRPM = 0;
	readSDO(0x4A04, 0x02, &actRPM);
	return actRPM;
}


bool RotaryTable::isMoving()
{
	int status = 0;
	readSDO(0x4002, 0x01, &status);
	bool moving = status & 0x08;
	return moving;
}

bool RotaryTable::startRandMove(int maxVel)
{
	double rand = ((double)std::rand() / (RAND_MAX));
	int vel = (rand - 0.5) * maxVel * 2;
	if (abs(vel) < 200) {
		if (vel < 0) {
			vel -= 400;
		}
		else {
			vel += 400;
		}
	}
	bool sucess = startVelMode(vel);
	return sucess;
}

bool RotaryTable::waitTargetReached(int timeout)
{
	// Make sure intitial value of actRPM and targetRPM are not similar
	// TODO solve this with wait_until
	auto begin = std::chrono::high_resolution_clock::now();

	int devMod = 0;
	readSDO(0x4003, 0x1, &devMod);
	devMod &= 0xFF;
	if (devMod == 3) {
		int actRPM = 1000;
		int targetRPM = -1000;
		while (abs(actRPM - targetRPM) >= 1) {
			readSDO(0x4A04, 0x02, &actRPM);
			readSDO(0x4000, 0x11, &targetRPM);
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			auto end = std::chrono::high_resolution_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() > timeout) {
				std::cerr << "Timeout while waiting for BG 75 target velocity reached." << std::endl;
				break;
			}
		}
	}
	else if(devMod == 7)
	{
		int actPos = 0;
		int targetPos = 10;
		while (abs(actPos - targetPos) >= 10)
		{
			readSDO(0x4760, 0x1, &targetPos);
			readSDO(0x4762, 0x1, &actPos);
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			auto end = std::chrono::high_resolution_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() > timeout) {
				std::cerr << "Timeout while waiting for BG 75 target position reached." << std::endl;
				break;
			}
		}
		return true;
	}
	return checkErr();
}

bool RotaryTable::writeSDO(WORD index, BYTE subindex, int value)
{
	int failedTrys = 0;
	while (failedTrys < 3)
	{
		COP_SetSDOTimeOut(this->m_Board_Hdl, 10000);
		short err = 0;
		DWORD txlen = sizeof(value);
		int txdata = value;
		DWORD abortcode = 0;
		err = COP_WriteSDO(this->m_Board_Hdl, this->m_node_no_slave, this->m_sdo_no, this->m_mode, index, subindex, txlen, (PBYTE)&txdata, &abortcode);

		if (err || abortcode) {
			std::cerr << "Failed to write SDO to BG Motor. Ixxat err:" << err << " abortcode:" << abortcode << std::endl;
			failedTrys++;
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			continue;
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool RotaryTable::readSDO(WORD index, BYTE subindex, int *value)
{
	int failedTrys = 0;
	while (failedTrys < 3)
	{
		COP_SetSDOTimeOut(this->m_Board_Hdl, 10000);
		short err;
		int rxdata = 0;
		DWORD txlen;
		DWORD abortcode;
		err = COP_ReadSDO(this->m_Board_Hdl, this->m_node_no_slave, this->m_sdo_no, this->m_mode, index, subindex, &txlen, (PBYTE)&rxdata, &abortcode);
		if (err || abortcode) {
			std::cerr << "Failed to read SDO from BG Motor. err:" << err << " abortcode:" << abortcode << std::endl;
			failedTrys++;
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			continue;
		}
		else
		{
			*value = int(rxdata);
			return true;
		}
	}
	return false;
}

