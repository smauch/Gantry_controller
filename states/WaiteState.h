#pragma once
#include "State.h"
#include "State.h"
#include <Gantry.h>
#include <Rotarytable.h>
#include <Tracker.h>
class WaitState : public State
{
public:
	WaitState(BackendModel* mod, Status status, Gantry* gantry, RotaryTable* rotary, Tracker* tracker) : State(mod, status) {
		this->gantry = gantry;
		this->rotary = rotary;
		this->tracker = tracker;
	}
protected:
	void doJob();
private:
	Gantry* gantry;
	RotaryTable* rotary;
	Tracker* tracker;
	std::vector<std::array<CML::uunit, Gantry::NUM_AMP>> candyBuffer;
};

