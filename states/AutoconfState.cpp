#include "AutoconfState.h"

void AutoconfState::doJob()
{
	int i = 0;
	for (auto it = Gantry::Y_STORAGE.begin(); it != Gantry::Y_STORAGE.end(); it++)
	{
		rotary->startVelMode(500);
		gantry->prepareCatch();
		gantry->fillTable(it->first);
		tracker->autoConfigure(it->first);
		rotary->stopMovement();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		//detect placed candy and bring it back to a defined position
		int failed_tries = 0;
		while (failed_tries < 3) {
			try {
				auto start = std::chrono::high_resolution_clock::now();
				Candy candy = tracker->getCandyOfColor(it->first, false);
				auto end = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				double angVel = rotary->getTableAngVel();
				candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVel / 1000 * 180 / M_PI * elapsed));
				double ang = candy.getCurrentPosition().getAngle();
				double radiusFact = candy.getCurrentPosition().getR() / tracker->getOuterR();
				statusModel->candyBuffer.push_back(Gantry::WAIT_PAT_BASE);
				statusModel->candyBuffer[i][1] = statusModel->candyBuffer[i][1] + i * 12000;

				bool catchSucess = gantry->catchRotary(ang, angVel, radiusFact, statusModel->candyBuffer[i], 8000);
				if (catchSucess) {
					i++;
					break;
				}
				else {
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
	int len = statusModel->candyBuffer.size();
	rotary->startVelMode(400);
	for (int i = 0; i < len; i++)
	{
		gantry->placeOnTable(statusModel->candyBuffer[0]);
		statusModel->candyBuffer.erase(statusModel->candyBuffer.begin());
	}
	// place one piece of each color on the table
	gantry->prepareCatch();
	for (auto it = Gantry::Y_STORAGE.begin(); it != Gantry::Y_STORAGE.end(); it++)
	{
		gantry->fillTable(it->first);
	}
	rotary->stopMovement();


	std::ofstream outfile;
	outfile.open(CANDIES_CONFIG);
	// write data into the json file.
	std::string candyJsonOut = json11::Json(tracker->getColorTrackers()).dump();
	outfile << candyJsonOut << std::endl;
	outfile.close();
}