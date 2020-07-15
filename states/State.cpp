#include "State.h"

State::State(BackendModel *statusModel, Status state, Gantry* gantry, RotaryTable* rotary, Tracker* tracker)
{
	this->state = state;
	this->gantry = gantry;
	this->rotary = rotary;
	this->tracker = tracker;
	this->statusModel = statusModel;
}

void State::exectue()
{
	isRunning = true;
	
}

void State::setJobDone()
{
	statusModel->setJobDone();
	isRunning = false;
}
