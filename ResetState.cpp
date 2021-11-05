#include "ResetState.h"

void ResetState::doJob()
{
	while (true) {
		try
		{
			gantry->resetMaintenance();
			rotary->initMotor();
		}
		catch (Gantry::UndervoltageException& e)
		{
			std::cerr << "Reset failed make sure to have the door closed." << std::endl;
			mod->setErr("Reset failed make sure to have the door closed");
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
	}
	mod->setReqStatus(IDLE);
}
