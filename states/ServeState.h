#pragma once
#include "State.h"
#include <Gantry.h>
#include <Rotarytable.h>
#include <Tracker.h>
#include <Color.h>
#include <Status.h>

class ServeState : public State
{
public:
	ServeState(BackendModel* mod, Status status, Gantry* gantry, RotaryTable* rotary, Tracker* tracker) : State(mod, status) {
		this->gantry = gantry;
		this->rotary = rotary;
		this->tracker = tracker;
	}
protected:
	void doJob();
private:
	Colors reqCandy;
	int failedTrys;
	Gantry* gantry;
	RotaryTable* rotary;
	Tracker* tracker;
};