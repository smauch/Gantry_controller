#include "ShutdownState.h"

void ShutdownState::doJob()
{
	std::this_thread::sleep_for(std::chrono::seconds(10));
}
