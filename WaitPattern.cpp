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

WaitPattern::WaitPattern()
{
	
}

WaitPattern::~WaitPattern()
{

}




bool WaitPattern::candiesToRotary(Gantry* gantry)
{
	int len = placed_candies.size();
	for (int i=0; i<len; i++ )
	{
		gantry->catchStatic(placed_candies[0], DISC_DROP);
		placed_candies.erase(placed_candies.begin());
	}
	if (placed_candies.empty())
	{
		return true;
	}
	else
	{
		return false;
	}
}


void RunningCandy::placeCandy(Gantry* gantry, Tracker* tracker, RotaryTable* rotary, Colors maj, Colors min)
{
	pos_store.push_back(POS1);
	pos_store.push_back(POS2);
	pos_store.push_back(POS3);
	pos_store.push_back(POS4);
	pos_store.push_back(POS5);
	bool catched;
	int i = 0;
	//pick up four candies of the same color
	for (auto it = pos_store.begin(); it < pos_store.end(); it++)
	{
		if (i <= 3)
		{
			gantry->prepareCatch();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			try
			{
				auto start = std::chrono::high_resolution_clock::now();
				Candy candy = tracker->getCandyOfColor(maj, false);
				auto end = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				double angVel = rotary->getAngVelocity();
				candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVel / 1000 * 180 / M_PI * elapsed));
				catched = gantry->catchRotary(angVel, candy.getCurrentPosition().getAngle(), candy.getCurrentPosition().getR(), *it);

			}
			catch (const NoCandyException&)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				//make something
			}
			if (catched)
			{
				placed_candies.push_back(*it);
				i += 1;

			}

		}
		else
		{
			try
			{
				auto start = std::chrono::high_resolution_clock::now();
				Candy candy = tracker->getCandyOfColor(min, false);
				auto end = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				double angVel = rotary->getAngVelocity();
				candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVel / 1000 * 180 / M_PI * elapsed));
				catched = gantry->catchRotary(angVel, candy.getCurrentPosition().getAngle(), candy.getCurrentPosition().getR(), *it);

			}
			catch (const NoCandyException&)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				//make something
			}
			if (catched)
			{
				placed_candies.push_back(*it);

			}

		}
	}
}

bool RunningCandy::runCandy(Gantry* gantry)
{
	for (int i = 0; i < 3; i++)
	{
		//assuming the unique candy is placed in pos 4
		gantry->catchStatic(placed_candies[3-i], INIT_POS);
		gantry->catchStatic(placed_candies[4-i], placed_candies[4-i]);
		gantry->catchStatic(INIT_POS, placed_candies[4-i]);
	}
	return true;
}

void CandyTower::buildTower(Gantry* gantry, Tracker* tracker, RotaryTable* rotary)
{
	pos_store.push_back(POS1);
	pos_store.push_back(POS2);
	pos_store.push_back(POS3);
	pos_store.push_back(POS4);
	pos_store.push_back(POS5);

	bool catched;
	int i = 0;
	for (auto it = pos_store.begin(); it < pos_store.end(); it++)
	{
		try
		{
			Colors nextCandy = static_cast<Colors>(i);
			auto start = std::chrono::high_resolution_clock::now();
			Candy candy = tracker->getCandyOfColor(nextCandy, false);
			auto end = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			double angVel = rotary->getAngVelocity();
			candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVel / 1000 * 180 / M_PI * elapsed));
			catched = gantry->catchRotary(angVel, candy.getCurrentPosition().getAngle(), candy.getCurrentPosition().getR(), *it);

		}
		catch (const NoCandyException&)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			//make something
		}
		if (catched)
		{
			placed_candies.push_back(*it);
			i += 1;

		}
	}
}

void CandyCircle::buildCircle(Gantry* gantry, Tracker* tracker, RotaryTable* rotary)
{
	pos_store.push_back(POS1);
	pos_store.push_back(POS2);
	pos_store.push_back(POS3);
	pos_store.push_back(POS4);
	pos_store.push_back(POS5);
	pos_store.push_back(POS6);
	pos_store.push_back(POS7);
	pos_store.push_back(POS8);
	bool catched;
	int i = 0;
	for (auto it = pos_store.begin(); it < pos_store.end(); it++)
	{
		try
		{
			if (i >= 7)
			{
				i = i - 7;
			}
			Colors nextCandy = static_cast<Colors>(i);
			auto start = std::chrono::high_resolution_clock::now();
			Candy candy = tracker->getCandyOfColor(nextCandy, false);
			auto end = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			double angVel = rotary->getAngVelocity();
			candy.setCurrentPosition(candy.getCurrentPosition().rotate(angVel / 1000 * 180 / M_PI * elapsed));
			catched = gantry->catchRotary(angVel, candy.getCurrentPosition().getAngle(), candy.getCurrentPosition().getR(), *it);

		}
		catch (const NoCandyException&)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			//make something
		}
		if (catched)
		{
			placed_candies.push_back(*it);
			i += 1;

		}
	}


}

