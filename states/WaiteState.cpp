#include "WaiteState.h"
#include <WaitPattern.h>

void WaitState::doJob()
{
	// TODO check if this is needed
	for (auto it = Gantry::Y_STORAGE.begin(); it != Gantry::Y_STORAGE.end(); it++)
	{
		int currFillHeight = gantry->getFillState(it->first);
		if (!currFillHeight) {
			std::set <Colors> currAvailable = getModel()->getAvailableCandies();
			currAvailable.erase(it->first);
			getModel()->setAvailableCandies(currAvailable);
		}
	}
	rotary->startVelMode(500);
	rotary->waitTargetReached(2000);
	gantry->prepareCatch();
	std::vector<Colors> sortColors = { LIGHT_BLUE,LIGHT_BLUE,LIGHT_BLUE,RED };
	RunningCandy myRunner(sortColors, gantry, tracker, rotary);
	// Run sorting twice
	myRunner.runCandy();
	myRunner.runCandy();
}
