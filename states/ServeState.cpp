#include "ServeState.h"

void ServeState::doJob()
{ 
	failedTrys = 0;
	if (statusModel->candiesToServe.size()) {
		reqCandy = statusModel->candiesToServe.front();

		while (failedTrys <= 3) {
			failedTrys++;
			rotary->startRandMove(800);
			gantry->prepareCatch();
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			double angVelStart = rotary->getTableAngVel();
			try
			{
				auto start = std::chrono::high_resolution_clock::now();
				Candy candy = tracker->getCandyOfColor(reqCandy, false);
				auto end = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				double angVelEnd = rotary->getTableAngVel();
				candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVelStart / 1000 * 180 / M_PI * elapsed));
				double ang = candy.getCurrentPosition().getAngle();
				float radiusFact = candy.getCurrentPosition().getR() / tracker->getOuterR();
				bool success = gantry->catchRotary(ang, angVelStart, radiusFact, Gantry::DROP_POS);
				if (success) {
					// Model sucessfull
					statusModel->candiesToServe.pop();
					gantry->fillTable(reqCandy);
					break;
				}
				else
				{
					failedTrys++;
				}
			}
			catch (const NoCandyException&)
			{
				gantry->fillTable(reqCandy);
				failedTrys++;
			}
		}
	}
	else {
		// No Requested candy
	}
}
