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



RotaryTable::RotaryTable(){

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
	m_res2 = 0;

	//Adding node to can network
	m_NgOrHb = COP_k_HEARTBEAT;
	m_GuardHeartbeatTime = 500;
	m_lifetimefactor = 0;
	m_node_no_slave = 127;
	m_res3 = 0;


	//Initializsation of board
	m_res = COP_InitBoard(&m_Board_Hdl, &m_boardtype, &m_boardID, m_canline);
	try
	{
		if (m_res != BER_k_OK)
		{ 
			throw "Initialization of board failed";

		}

	}
	catch (const char *e)
	{
		std::cout << e << std::endl;

	}

	//Initialization of interface
	m_res2 = COP_InitInterface(m_Board_Hdl, m_baudtable, m_baudrate, m_node_no_master, m_hbtime, m_AddFeatures);
	try
	{
		if (m_res2 != COP_k_OK)
		{
			throw "Initialization of interface failed";
		}

	}
	catch (const char *e)
	{
		std::cout << e << std::endl;

	}
	//Add of node to the can network
	m_res3 = COP_AddNode(m_Board_Hdl, m_node_no_slave, m_NgOrHb, m_GuardHeartbeatTime + 100, m_lifetimefactor);
	try
	{
		if (m_res3 != COP_k_OK)
		{
			throw "Add of the node failed";

		}

	}
	catch (const char *e)
	{
		std::cout << e << std::endl;
	}

	WORD idx_vector[2] = { 0x4000, 0x4150 };
	BYTE subidx_vector[2] = { 2, 1 };
	BOOL value_vector[2] = { 1, 1, };
	SHORT results[2];
	m_sdo_no = COP_k_DEFAULT_SDO;
	m_mode = COP_k_NO_BLOCKTRANSFER;
	m_abortcode = 0;

	for (auto i = 0; i < 2; i++)
	{
		results[i] = COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, idx_vector[i], subidx_vector[i], sizeof(value_vector[i]), (PBYTE)&value_vector[i], &m_abortcode);	
		
		try
		{
			if (results[i] != 0)
			{
				throw "One of the Initialization steps failed";
			}
		}
		catch (const char *e)
		{
			std::cout << e << std::endl;
		}

	}

}

/*************************Function for setting the velocity of the servo motor in U/min*************************************************/
void RotaryTable::setVelocity(BOOL velocity, BOOL v_acc, BOOL v_dec) {
	m_vel = velocity;

	m_sdo_no = COP_k_DEFAULT_SDO;
	m_mode = COP_k_NO_BLOCKTRANSFER;
	
	//good values for vel_acc and vel_dec is 1000
	m_abortcode = 0;
	m_vel_acc_dv = v_acc;
	m_vel_acc_dt = 5000;
	m_vel_dec_dv = v_dec;
	m_vel_dec_dt = 5000;

	/**************Setting the values for acceleration/deceleration*****************/
	COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4340, 1, sizeof(m_vel_acc_dv), (PBYTE)&m_vel_acc_dv, &m_abortcode);
	COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4341, 1, sizeof(m_vel_acc_dt), (PBYTE)&m_vel_acc_dt, &m_abortcode);
	COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4342, 1, sizeof(m_vel_dec_dv), (PBYTE)&m_vel_dec_dv, &m_abortcode);
	COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4343, 1, sizeof(m_vel_dec_dt), (PBYTE)&m_vel_dec_dt, &m_abortcode);

	//Writing the corresponding velocity
	COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4000, 0x11, sizeof(m_vel), (PBYTE)&m_vel, &m_abortcode);

	
}

void RotaryTable::startMovement() {

	m_abortcode = 0;
	m_mode_motor = 0x74;
	m_continue = 0x04;
	//writing the continue param if motor has already moved
	COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4000, 1, sizeof(m_continue), (PBYTE)&m_continue, &m_abortcode);
	//Starting motor
	COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4000, 2, sizeof(m_mode_motor), (PBYTE)&m_mode_motor, &m_abortcode);
	bool moving = this->isMoving();
	if (moving)
	{
		std::cout << "Motor is in good state" << std::endl;
	}
	else
	{
		std::cout << "Motor has an internal error" << std::endl;
	}
}

void RotaryTable::stopMovement(void) {

	m_abortcode = 0;
	m_mode_motor = 0x03;
	//Stopping motor
	COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4000, 1, sizeof(m_mode_motor), (PBYTE)&m_mode_motor, &m_abortcode);

}

void RotaryTable::updateVelocity(BOOL upvelocity) {

	m_vel = upvelocity;
	m_abortcode = 0;
	m_mode_motor = 0x74;
	//Writing the corresponding velocity
	COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4000, 0x11, sizeof(m_vel), (PBYTE)&m_vel, &m_abortcode);
	COP_WriteSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4000, 2, sizeof(m_mode_motor), (PBYTE)&m_mode_motor, &m_abortcode);
	bool moving = this->isMoving();
	if (moving)
	{
		std::cout << "Motor is in good state" << std::endl;
	}
	else
	{
		std::cout << "Motor has an internal error" << std::endl;
	}
}

double RotaryTable::getAngVelocity()
{
	//DWORD *len;
	//BYTE m_vel;
	DWORD len;
	m_abortcode = 0;
	COP_ReadSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4000, 0x11, &len, (PBYTE)&m_vel, &m_abortcode);
	double tableAngVel = (((m_vel / 36.0) * 2 * M_PI) / 60.0);
	return tableAngVel;
}

bool RotaryTable::isMoving(void)
{
	DWORD len;
	m_abortcode = 0;	
	COP_ReadSDO(m_Board_Hdl, m_node_no_slave, m_sdo_no, m_mode, 0x4001, 1, &len, (PBYTE)&m_status, &m_abortcode);//& 0x04;
	if (m_status == 0) {
		return true;
	}
	else { return false; }
}

RotaryTable::~RotaryTable(){
}