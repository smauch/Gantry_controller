#pragma once
#include "State.h"
#include "Status.h"
#include <BackendModel.h>
class IdleState : public State
{
public:
	IdleState(BackendModel* mod, Status status) : State(mod, status) {
		this->mod = mod;
	}
protected:
	void doJob();
private:
	BackendModel* mod;
};

