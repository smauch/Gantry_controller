#pragma once
#include "State.h"
class WaitState : public State
{
public:
	WaitState(BackendModel* mod, Status status) : State(mod, status) {}
	void doJob();
};

