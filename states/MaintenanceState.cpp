#include "MaintenanceState.h"

void MaintenanceState::doJob()
{
	//Deactivate
	gantry->maintenance();
	rotary->maintenance();
	while (true) {
		if (!mod->getReqStatus().empty()) {
			if (mod->getReqStatus().back() == RESET) {
				break;
			}
		}
	}
}
