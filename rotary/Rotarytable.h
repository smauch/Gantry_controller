#ifndef ROTARYTABLE_H
#define ROTARYTABLE_H

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

//Needed for type def
#include "objbase.h"
#include <Cop.h>
#include <exception>


// custom exception when reading or writing SDO fails
struct CanOpenErr : public std::exception {
	const char* what() const throw() {
		return "Failed writing or reading SDO";
	}
};

class RotaryTable
{

public:

	/*Constructor and destructor for any instances of the rotarytable class */
	RotaryTable();
	~RotaryTable();

	bool initNetwork();
	bool initMotor();

	/*Use this function to set the params for the ramps and the needed velocity in U/min  */
	bool startVelMode(int velocity, int acceleration=1000, int decceleration=1000);
	bool startRelMove(double angular, int velocity);

	/*Functions to start and stop the movement the movement*/
	bool stopMovement(int timeout);
	double getTableAngVel();
	int getMotorVel();
	bool isMoving();
	bool startRandMove(int maxVel);
	bool waitTargetReached(int timeout);

private:
	bool writeSDO(WORD index, BYTE subindex, int value);
	bool readSDO(WORD index, BYTE subindex, int *value);
	void cleanErr();
	bool checkErr();
	//every m_res variable stores information about a possible error of the corresponding step
	COP_t_HANDLE m_Board_Hdl;
	GUID m_boardtype;
	GUID m_boardID;
	long m_canline;
	SHORT m_res;

	BYTE m_baudtable;
	BYTE m_baudrate;
	BYTE m_node_no_master;
	BYTE m_node_no_slave;
	WORD m_hbtime;
	DWORD m_AddFeatures;

	BYTE m_NgOrHb;
	WORD m_GuardHeartbeatTime;
	BYTE m_lifetimefactor;

	BYTE m_sdo_no;
	BYTE m_mode;
};
#endif
