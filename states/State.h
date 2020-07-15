#pragma once
#include <Gantry.h>
#include <Rotarytable.h>
#include <Candy.h>
#include <Tracker.h>
#include <Status.h>
#include <BackendModel.h>

class State
{
public:
	State();
	State(BackendModel *statusModel, Status state, Gantry* gantry, RotaryTable* rotary, Tracker* tracker);

	void exectue();
	bool getIsRunning();
	

protected:
	void setJobDone();

	BackendModel *statusModel;
	Status state;
	Gantry* gantry;
	RotaryTable* rotary;
	Tracker* tracker;

private:
	bool isRunning = false;
};

