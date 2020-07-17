#pragma once
#include "State.h"
class MaintState : public State
{
public:
	MaintState(BackendModel* mod, Status status) : State(mod, status) {}
	void doJob();
};

