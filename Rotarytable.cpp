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
//#include <QtCore/QDebug>



RotaryTable::RotaryTable()
{
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
	m_res = COP_InitBoard(&m_Board_Hdl, &m_boardtype, &m_boardID, m_canline);
	if (m_res) {
		return false;
	}
	m_res = COP_InitInterface(m_Board_Hdl, m_baudtable, m_baudrate, m_node_no_master, m_hbtime, m_AddFeatures);
	if (m_res) {
		return false;
	}
	m_res = COP_AddNode(m_Board_Hdl, m_node_no_slave, m_NgOrHb, m_GuardHeartbeatTime + 100, m_lifetimefactor);
	if (m_res) {
		return false;
	}
	return true;
}

bool RotaryTable::initMotor()
{
	if (!writeSDO(0x4000, 0x02, 0x72)) {
		return false;
	}
	// Clear error
	if (! writeSDO(0x4000, 0x02, 0x01)) {
		return false;
	}
	// Check Error register
	int err = 0;
	if (! checkErr(err)) {
		return false;
	}
	if (err) {
		return false;
	}
	// Restore default parameters
	if (! writeSDO(0x4000, 0x01, 0x82)) {
		return false;
	}
	return true;
}

bool RotaryTable::checkErr(int &err)
{
	if (!readSDO(0x4001, 0x01, err)) {
		return false;
	}
	return true;
}

/*************************Function for setting the velocity of the servo motor in U/min*************************************************/
bool RotaryTable::startVelMode(int velocity, int acceleration, int decceleration) {
	// Clear error
	if (!writeSDO(0x4000, 0x02, 0x01)) {
		return false;
	}
	if (! writeSDO(0x4000, 0x02, 0)) {
		return false;
	}
	if (! writeSDO(0x4000, 0x11, velocity)) {
		return false;
	}
	if (! writeSDO(0x4341, 1, acceleration)) {
		return false;
	}
	if (!writeSDO(0x4343, 1, decceleration)) {
		return false;
	}
	if (! writeSDO(0x4000, 0x02, 0x74)) {
		return false;
	}
	
	return true;
}

bool RotaryTable::moveRel(float angular, int velocity)
{
	// Clear error
	if (!writeSDO(0x4000, 0x02, 0x01)) {
		return false;
	}
	int increment = int(4096 * angular * 36 / (2 * M_PI));
	if (!writeSDO(0x4000, 0x02, 0)) {
		return false;
	}
	if (! writeSDO(0x4000, 0x12, increment)) {
		return false;
	}
	if (! writeSDO(0x4000, 0x11, velocity)) {
		return false;
	}
	if (!writeSDO(0x4000, 0x02, 0x77)) {
		return false;
	}
	return true;
}


bool RotaryTable::stopMovement(void) {
	if (!writeSDO(0x4000, 0x02, 0)) {
		return false;
	}
	if (!writeSDO(0x4000, 0x11, 0)) {
		return false;
	}
	if (!writeSDO(0x4000, 0x02, 0x74)) {
		return false;
	}
	// Clear error
	if (!writeSDO(0x4000, 0x02, 0x01)) {
		return false;
	}
	return true;
}

double RotaryTable::getTableAngVel()
{
	int rpm;
	readSDO(0x4A04, 0x02, rpm);
	double angVel = (((rpm / 36.0) * 2 * M_PI) / 60.0);
	return angVel;
}

int RotaryTable::getMotorVel()
{
	int rpm;
	readSDO(0x4A04, 0x02, rpm);
	return rpm;
}


bool RotaryTable::isMoving(void)
{
	int status = 0;
	readSDO(0x4002, 0x01, status);
	bool moving = status & 0x04;
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
	int statusword = 0;
	bool trReached = false;
	//qDebug() << "entering while attention";
	while (! trReached) {
		readSDO(0x6041, 0x0, statusword);
		trReached = statusword & 0x400;
	}
	//qDebug() << "leave while attention";
	return true;
}

bool RotaryTable::writeSDO(WORD index, BYTE subindex, BOOL value)
{
	COP_SetSDOTimeOut(this->m_Board_Hdl, 2000);
	short err = 0;
	DWORD txlen = sizeof(value);
	DWORD abortcode = 0;
	err = COP_WriteSDO(this->m_Board_Hdl, this->m_node_no_slave, this->m_sdo_no, this->m_mode, index, subindex, txlen, (PBYTE)&value, &abortcode);
	
	if (err || abortcode) {
		return true;
	}
	return true;
}

bool RotaryTable::readSDO(WORD index, BYTE subindex, int& value)
{
	short err;
	DWORD txlen;
	DWORD abortcode;
	err = COP_ReadSDO(this->m_Board_Hdl, this->m_node_no_slave, this->m_sdo_no, this->m_mode, index, subindex, &txlen, (PBYTE)&value, &abortcode);
	if (err || abortcode) {
		return false;
	}
	return true;
}

RotaryTable::~RotaryTable(){
}