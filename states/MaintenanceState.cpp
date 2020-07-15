#include "MaintenanceState.h"

void MaintenanceState::doJob()
{
	gantry->homeAxis(30000, Gantry::HOME_ORDER);
	gantry->disable();
}
