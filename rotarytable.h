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

class RotaryTable
{
public:
	/*Constructor and destructor for any instances of the rotarytable class */
	RotaryTable();
	~RotaryTable();
	
	/*Use this function to set the params for the ramps and the needed velocity in U/min  */
	void SetVelocity(BOOL velocity, BOOL v_acc, BOOL v_dec);

	/*Functions to start and stop the movement the movement*/
	void StartMovement(void);
	void StopMovement(void);

private:

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
SHORT m_res2;

BYTE m_NgOrHb;
WORD m_GuardHeartbeatTime;
BYTE m_lifetimefactor;
SHORT m_res3;

BYTE m_sdo_no;
BYTE m_mode;
WORD m_idx;
BYTE m_subidx;
DWORD m_buffer_size;
DWORD m_code;
SHORT m_res_4;

BOOL m_vel;
DWORD m_txlen;
BYTE* m_txdata;
BOOL m_buffer;
DWORD m_abortcode;
SHORT m_res_5;
BOOL m_mode_motor;
BOOL m_datareceiver;
BOOL m_size_datarec;
BOOL m_status;
BOOL m_continue;

BOOL m_vel_acc_dv;
BOOL m_vel_acc_dt;
BOOL m_vel_dec_dv;
BOOL m_vel_dec_dt;


};



	












	



//BOOL RotaryTable::GetPos() { return m_currentpos; }







#endif
