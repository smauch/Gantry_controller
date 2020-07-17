#include "ServeState.h"


void ServeState::doJob()
{ 
	failedTrys = 0;
	if (getModel()->getCandiesToServe().size()) {
		reqCandy = getModel()->getCandiesToServe().front();
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
				double angVelDiff = angVelEnd - angVelStart;
				std::cout << "AngVel difference: " << angVelDiff;
				candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVelStart / 1000 * 180 / M_PI * elapsed));
				double ang = candy.getCurrentPosition().getAngle();
				float radiusFact = candy.getCurrentPosition().getR() / tracker->getOuterR();
				bool success = gantry->catchRotary(ang, angVelStart, radiusFact, Gantry::DROP_POS);
				if (success) {
					// Model sucessfull
					getModel()->setCandyServeDone();
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
				failedTrys++;
				if (gantry->getFillState(reqCandy)) {
					gantry->fillTable(reqCandy);
				}
				else {
					std::set <Colors> currAvailable = getModel()->getAvailableCandies();
					currAvailable.erase(reqCandy);
					getModel()->setAvailableCandies(currAvailable);
					gantry->prepareCatch();
					return;
				}
			}
		}
	}
	else {
		// No Requested candy
	}
}
