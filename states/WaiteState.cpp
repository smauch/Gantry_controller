#include "WaiteState.h"
#include <WaitPattern.h>

void WaitState::doJob()
{
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
	gantry->prepareCatch();
	std::vector<Colors> test = { LIGHT_BLUE,LIGHT_BLUE,LIGHT_BLUE,RED };
	RunningCandy myRunner(test, gantry, tracker, rotary);
	myRunner.runCandy();
	myRunner.runCandy();

}
