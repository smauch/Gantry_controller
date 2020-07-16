#include "WaitPattern.h"
#include <iostream>
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


WaitPattern::WaitPattern(Gantry* gantry, Tracker* tracker, RotaryTable* rotary)
{
	this->gantry = gantry;
	this->tracker = tracker;
	this->rotary = rotary;
}

WaitPattern::~WaitPattern()
{
	candiesToRotary();
}

bool WaitPattern::candiesToRotary()
{
	rotary->startVelMode(50);
	int len = placedCandies.size();
	for (int i=0; i<len; i++ )
	{
		if (gantry->placeOnTable(placedCandies[0])) {
			placedCandies.erase(placedCandies.begin());
		}
	}
	if (placedCandies.empty())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool WaitPattern::getFromRotary(std::map<std::array<uunit, Gantry::NUM_AMP>, Colors> candiesTo)
{
	for (auto const& [targetPos, reqCandy] : candiesTo) {
		int failedTries = 0;
		Colors targetCandy = reqCandy;
		while (failedTries < 3) {
			gantry->prepareCatch();
			double angVelStart = rotary->getTableAngVel();
			try
			{
				auto start = std::chrono::high_resolution_clock::now();
				Candy candy = tracker->getCandyOfColor(targetCandy, false);
				auto end = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVelStart / 1000 * 180 / M_PI * elapsed));
				double ang = candy.getCurrentPosition().getAngle();
				double radiusFact = candy.getCurrentPosition().getR() / tracker->getOuterR();
				bool catchSucess = gantry->catchRotary(ang, angVelStart, radiusFact, targetPos);
				if (!catchSucess) {
					failedTries++;
					gantry->prepareCatch();
				}
				else {
					this->placedCandies.push_back(targetPos);
					break;
				}
			}
			catch (const NoCandyException&)
			{
				failedTries++;
				//Prevent running out of candies
				if (!gantry->fillTable(targetCandy)) {
					targetCandy = ANY;
				}
			}
		}
	}
	return true;
}



bool RunningCandy::runCandy()
{
	int len = placedCandies.size();
	len = len - 1;
	gantry->catchStatic(placedCandies[len], INIT_POS);
	for (int i = 1; i <= len; i++)
	{
		gantry->catchStatic(placedCandies[len - i], placedCandies[len - i + 1]);
	}
	gantry->catchStatic(INIT_POS,placedCandies[0]);
	return true;
}

void RunningCandy::calcTargetPos(std::vector<Colors> candies)
{
	std::array<uunit, Gantry::NUM_AMP> currPos = INIT_POS;
	for (auto const& it : candies) {
		currPos[1] = currPos[1] + 1.5 * CANDY_SIZE[0];
		candiesTo.insert({ currPos, it });
	}
}

//void CandyTower::buildTower(Gantry* gantry, Tracker* tracker, RotaryTable* rotary)
//{
//	pos_store.push_back(POS1);
//	pos_store.push_back(POS2);
//	pos_store.push_back(POS3);
//	pos_store.push_back(POS4);
//	pos_store.push_back(POS5);
//
//	bool catched;
//	int i = 0;
//	for (auto it = pos_store.begin(); it < pos_store.end(); it++)
//	{
//		try
//		{
//			Colors nextCandy = static_cast<Colors>(i);
//			auto start = std::chrono::high_resolution_clock::now();
//			Candy candy = tracker->getCandyOfColor(nextCandy, false);
//			auto end = std::chrono::high_resolution_clock::now();
//			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//			double angVel = rotary->getTableAngVel();
//			candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVel / 1000 * 180 / M_PI * elapsed));
//			catched = gantry->catchRotary(angVel, candy.getCurrentPosition().getAngle(), candy.getCurrentPosition().getR(), *it);
//
//		}
//		catch (const NoCandyException&)
//		{
//			std::this_thread::sleep_for(std::chrono::milliseconds(500));
//			//make something
//		}
//		if (catched)
//		{
//			placed_candies.push_back(*it);
//			i += 1;
//
//		}
//	}
//}
//
//void CandyCircle::buildCircle(Gantry* gantry, Tracker* tracker, RotaryTable* rotary)
//{
//	pos_store.push_back(POS1);
//	pos_store.push_back(POS2);
//	pos_store.push_back(POS3);
//	pos_store.push_back(POS4);
//	pos_store.push_back(POS5);
//	pos_store.push_back(POS6);
//	pos_store.push_back(POS7);
//	pos_store.push_back(POS8);
//	bool catched;
//	int i = 0;
//	for (auto it = pos_store.begin(); it < pos_store.end(); it++)
//	{
//		try
//		{
//			if (i >= 7)
//			{
//				i = i - 7;
//			}
//			Colors nextCandy = static_cast<Colors>(i);
//			auto start = std::chrono::high_resolution_clock::now();
//			Candy candy = tracker->getCandyOfColor(nextCandy, false);
//			auto end = std::chrono::high_resolution_clock::now();
//			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//			double angVel = rotary->getTableAngVel();
//			candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVel / 1000 * 180 / M_PI * elapsed));
//			catched = gantry->catchRotary(angVel, candy.getCurrentPosition().getAngle(), candy.getCurrentPosition().getR(), *it);
//
//		}
//		catch (const NoCandyException&)
//		{
//			std::this_thread::sleep_for(std::chrono::milliseconds(500));
//			//make something
//		}
//		if (catched)
//		{
//			placed_candies.push_back(*it);
//			i += 1;
//
//		}
//	}
//
//
//}

