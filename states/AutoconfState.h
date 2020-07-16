#pragma once
#include "State.h"
#include <Gantry.h>
#include <Rotarytable.h>
#include <Tracker.h>
#include <Color.h>
#include <Status.h>

class AutoconfState : public State
{
public:
	AutoconfState(BackendModel* mod, Status status, Gantry* gantry, RotaryTable* rotary, Tracker* tracker) : State(mod, status) {
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
	const char CANDIES_CONFIG[32] = "./parameters/CADIES_CONFIG.json";
	std::vector<std::array<CML::uunit, Gantry::NUM_AMP>> candyBuffer;
};

