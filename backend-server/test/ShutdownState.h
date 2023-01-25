#pragma once
#include "State.h"
class ShutdownState : public State
{
public:
	ShutdownState(BackendModel* mod, Status status) : State(mod, status) {}
	void doJob();
};

