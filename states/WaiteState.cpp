#include "WaiteState.h"

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

	gantry->prepareCatch();
	for (size_t i = 0; i < 4; i++)
	{
		try {
			auto start = std::chrono::high_resolution_clock::now();
			Candy candy = tracker->getCandyOfColor(ANY, false);
			auto end = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			double angVel = rotary->getTableAngVel();
			candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVel / 1000 * 180 / M_PI * elapsed));
			double ang = candy.getCurrentPosition().getAngle();
			double radiusFact = candy.getCurrentPosition().getR() / tracker->getOuterR();
			candyBuffer.push_back(Gantry::WAIT_PAT_BASE);
			candyBuffer[i][1] = candyBuffer[i][1] + i * 12000;

			bool catchSucess = gantry->catchRotary(ang, angVel, radiusFact, candyBuffer[i], 8000);

			}
		catch (const NoCandyException&)
		{
			//failed_tries += 1;
		}
	}
}
