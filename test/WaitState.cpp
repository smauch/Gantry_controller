#include "WaitState.h"

void WaitState::doJob()
{
		std::cout << "I will start waitPattern for 60 seconds" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(60));
		std::cout << "Finished with waitPattern" << std::endl;
}
