#ifndef WAITPATTERN_H 
#define WAITPATTERN_H

#include <stdio.h>
#include <string>
#include <Gantry.h>
#include <opencv2/opencv.hpp>
#include <Candy.h>
#include <ColorTracker.h>
#include <Tracker.h>
#include <Color.h>
#include <Camera.h>
#include <Rotarytable.h>
#include <pylon/Device.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <vector>
#include <fstream>
#include <iostream>


class WaitPattern
{

public:
	WaitPattern(Gantry* gantry, Tracker* tracker, RotaryTable* rotary);
	~WaitPattern(); 
	
protected:
	Gantry* gantry;
	Tracker* tracker;
	RotaryTable* rotary;

	bool candiesToRotary();
	bool getFromRotary(std::map<std::array<uunit, Gantry::NUM_AMP>, Colors> candiesTo);
	std::vector<std::array<uunit, Gantry::NUM_AMP>> placedCandies;
	std::array<uunit, 3> INIT_POS = Gantry::WAIT_PAT_BASE;
	std::array<uunit, 3> CANDY_SIZE = Gantry::CANDY_SIZE;
};

class RunningCandy: public WaitPattern
{
public:
	RunningCandy(std::vector<Colors> candies, Gantry* gantry, Tracker* tracker, RotaryTable* rotary) : WaitPattern(gantry, tracker, rotary) {
		calcTargetPos(candies);
		getFromRotary(candiesTo);
	};
	bool runCandy();

private:
	void calcTargetPos(std::vector<Colors> candies);
	std::map<std::array<uunit, Gantry::NUM_AMP>, Colors> candiesTo;
};



//class CandyTower : public WaitPattern
//{
//public:
//	void buildTower(Gantry* gantry, Tracker* tracker, RotaryTable* rotary);
//
//
//private:
//	const std::array<uunit, 3> POS1 = { INIT_POS[0], INIT_POS[1], INIT_POS[2] + CANDY_SIZE[2] };
//	const std::array<uunit, 3> POS2 = { INIT_POS[0], INIT_POS[1], POS1[2] + CANDY_SIZE[2] };
//	const std::array<uunit, 3> POS3 = { INIT_POS[0], INIT_POS[1], POS2[2] + CANDY_SIZE[2] };
//	const std::array<uunit, 3> POS4 = { INIT_POS[0], INIT_POS[1], POS3[2] + CANDY_SIZE[2] };
//	const std::array<uunit, 3> POS5 = { INIT_POS[0], INIT_POS[1], POS4[2] + CANDY_SIZE[2] };
//
//};
//
//class CandyCircle : public WaitPattern
//{
//public:
//	void buildCircle(Gantry* gantry, Tracker* tracker, RotaryTable * rotary);
//
//private:
//	const std::array<uunit, 3> POS1 = {INIT_POS[0] + 2 * CANDY_SIZE[0], INIT_POS[1], 0};
//	const std::array<uunit, 3> POS2 = {POS1[0] - CANDY_SIZE[0], POS1[1] + CANDY_SIZE[1],0};
//	const std::array<uunit, 3> POS3 = {INIT_POS[0], INIT_POS[1] + 2 * CANDY_SIZE[1], 0};
//	const std::array<uunit, 3> POS4 = {POS3[0] - CANDY_SIZE[0], POS3[1] - CANDY_SIZE[1], 0};
//	const std::array<uunit, 3> POS5 = {INIT_POS[0] - 2 * CANDY_SIZE[0], INIT_POS[1],0};
//	const std::array<uunit, 3> POS6 = {POS5[0] + CANDY_SIZE[0],POS5[1] - CANDY_SIZE[1],0 };
//	const std::array<uunit, 3> POS7 = {INIT_POS[0], INIT_POS[1] - 2 * CANDY_SIZE[1],0};
//	const std::array<uunit, 3> POS8 = { POS7[0] + CANDY_SIZE[0], POS7[1] + CANDY_SIZE[1],0 };
//
//
//};






#endif
