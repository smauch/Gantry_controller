#pragma once
#include "State.h"
class ServeState : public State
{
public:
	ServeState(BackendModel* mod, Status status) : State(mod, status) {}
	void doJob();
};

