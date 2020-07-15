#include "State.h"

State::State(Status state, Gantry* gantry, RotaryTable* rotary, Candy* candy, Tracker* tracker)
{
	this->state = state;
	this->gantry = gantry;
	this->rotary = rotary;
	this->candy = candy;
	this->tracker = tracker;

}
