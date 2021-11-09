#include "MaintenanceState.h"

void MaintenanceState::doJob()
{
	//Deactivate
	gantry->maintenance();
	rotary->stopMovement(3000);
	while (true) {
		if (!mod->getReqStatus().empty()) {
			if (mod->getReqStatus().back() == RESET) {
				break;
			}
		}
	}
}
