#include "ServeState.h"

void ServeState::doJob()
{
	while (!this->getModel()->getCandiesToServe().empty())
	{
		std::cout << "I will serve Candy" << this->getModel()->getCandiesToServe().front() << std::endl;
		this->getModel()->setCandyServeDone();
		std::this_thread::sleep_for(std::chrono::seconds(10));
		std::cout << "Finished with serving" << std::endl;
	}
}
