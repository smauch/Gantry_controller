#include "AutoconfState.h"

void AutoconfState::doJob()
{
	const char CANDIES_CONFIG[] = "C:/Users/dunkerLinear/Documents/dev/Controller/x64/Release/parameters/CADIES_CONFIG.json";
	//Clean table
	rotary->stopMovement(3000);
	int failed_tries = 0;
	while (failed_tries < 5) {
		try {
			Candy candy = tracker->getCandyOfColor(ANY);
			double ang = candy.getCurrentPosition().getAngle();
			double radiusFact = candy.getCurrentPosition().getR() / tracker->getOuterR();
			bool catchSucess = gantry->catchRotary(ang, 0, radiusFact, Gantry::DROP_POS);
			if (catchSucess) {
				failed_tries = 0; 
				gantry->prepareCatch();
			}
			else {
				rotary->startRelMove(PI, 800);
				rotary->waitTargetReached(2000);
				gantry->prepareCatch();
				failed_tries += 1;
			}
		}
		catch (const NoCandyException&)
		{
			failed_tries += 1;
		}
	}
	// Auto conf
	int i = 0;
	for (auto it = Gantry::Y_STORAGE.begin(); it != Gantry::Y_STORAGE.end(); it++)
	{
		rotary->startVelMode(500);
		gantry->prepareCatch();
		gantry->fillTable(it->first);
		tracker->autoConfigure(it->first);
		rotary->stopMovement(3000);
		int failed_tries = 0;
		// TODO if these trys to not work throw move plate
		while (failed_tries < 5) {
			try {
				auto start = std::chrono::high_resolution_clock::now();
				Candy candy = tracker->getCandyOfColor(it->first, false);
				auto end = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				double angVel = rotary->getTableAngVel();
				candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVel / 1000 * 180 / M_PI * elapsed));
				double ang = candy.getCurrentPosition().getAngle();
				double radiusFact = candy.getCurrentPosition().getR() / tracker->getOuterR();
				candyBuffer.push_back(Gantry::WAIT_PAT_BASE);
				candyBuffer[i][1] = candyBuffer[i][1] + i * 12000;

				bool catchSucess = gantry->catchRotary(ang, angVel, radiusFact, candyBuffer[i], 8000);
				if (catchSucess) {
					i++;
					break;
				}
				else {
					rotary->startRelMove(PI, 800);
					rotary->waitTargetReached(2000);
					gantry->prepareCatch();
					failed_tries += 1;
				}
			}
			catch (const NoCandyException&)
			{
				failed_tries += 1;
			}
		}
	}

	//clear afterwards the plate and fill the table
	int len = candyBuffer.size();
	rotary->startVelMode(100);
	for (int i = 0; i < len; i++)
	{
		//TODO make sure is not empty
		if (gantry->placeOnTable(candyBuffer[0])) {
			candyBuffer.erase(candyBuffer.begin());
		}
		else {
			// Failed to catch Candy
			// TODO implement spike search
		}
		
	}
	gantry->prepareCatch();
	rotary->startRandMove(1000);
	rotary->waitTargetReached(2000);
	std::ofstream outfile;
	outfile.open(CANDIES_CONFIG);
	// write data into the json file.
	std::string candyJsonOut = json11::Json(tracker->getColorTrackers()).dump();
	outfile << candyJsonOut << std::endl;
	outfile.close();
	mod->setReqStatus(IDLE);
}
