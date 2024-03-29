#pragma once
#include "State.h"
#include "Status.h"
#include <Gantry.h>
#include <Rotarytable.h>
#include <BackendModel.h>
class MaintenanceState : public State
{
public:
	MaintenanceState(BackendModel* mod, Status status, Gantry* gantry, RotaryTable* rotary) : State(mod, status) {
		this->gantry = gantry;
		this->rotary = rotary;
		this->mod = mod;
	}
protected:
	void doJob();
private:
	Gantry* gantry;
	RotaryTable* rotary;
	BackendModel* mod;
};

