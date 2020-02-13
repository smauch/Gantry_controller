#include "WaitPattern.h"
#include <iostream>


WaitPattern::WaitPattern()
{
	
}

WaitPattern::~WaitPattern()
{

}

bool WaitPattern::candiesToRotary(Gantry* gantry)
{
	for (int i = 0; i < placed_candies.size(); i++)
	{
		gantry->catch(*placed_candies.end(), ROT_POS);//Here has to come the start position and the ROT_POS
		placed_candies.erase(placed_candies.end());
	}
	if (placed_candies.empty())
	{
		return true;
	}
}


void RunningCandy::placeCandy(Gantry* gantry, Tracker* tracker, Colors maj, Colors min)
{
	pos_store.push_back(POS1);
	pos_store.push_back(POS2);
	pos_store.push_back(POS3);
	pos_store.push_back(POS4);
	pos_store.push_back(POS5);
	bool catched;
	Candy detectedCandies;
	//pick up four candies of the same color
	for (auto it = pos_store.begin(); it < pos_store.end(); it++)
	{
		int i = 0;
		if (i <= 3)
		{
			detectedCandies = tracker->getCandyOfColor(maj, 10);
			catched = gantry->catchCandy(detectedCandies.getAngularVelocity(), detectedCandies.getCurrentPosition().getAngle(), detectedCandies.getCurrentPosition().getR());
			if (catched)
			{
				placed_candies.push_back(*it);

			}

		}
		else
		{
			detectedCandies = tracker->getCandyOfColor(min, 10);
			catched = gantry->catchCandy(detectedCandies.getAngularVelocity(), detectedCandies.getCurrentPosition().getAngle(), detectedCandies.getCurrentPosition().getR());
			if (catched)
			{
				placed_candies.push_back(*it);

			}


		}
		i += 1;
	}
}

bool RunningCandy::runCandy(Gantry* gantry, Tracker* tracker)
{
	for (int i = 0; i < 3; i++)
	{
		//assuming the unique candy is placed in pos 4
		gantry->catch (placed_candies[3-i], INIT_POS);
		gantry->catch (placed_candies[4-i], placed_candies[4-i]);
		gantry->catch (INIT_POS, placed_candies[4-i]);
	}
	return true;
}

bool CandyTower::buildTower(Gantry* gantry, Tracker* tracker, Colors maj, Colors min)
{
	pos_store.push_back(POS1);
	pos_store.push_back(POS2);
	pos_store.push_back(POS3);
	pos_store.push_back(POS4);
	pos_store.push_back(POS5);

	Candy detectedCandies;
	bool catched;
	for (auto it = pos_store.begin(); it < pos_store.end(); it++)
	{
		detectedCandies = tracker->getCandyOfColor(min, 10);
		catched = gantry->catchCandy(detectedCandies.getAngularVelocity(), detectedCandies.getCurrentPosition().getAngle(), detectedCandies.getCurrentPosition().getR());
		if (catched)
		{
			placed_candies.push_back(*it);

		}
	}
}

void CandyCircle::buildCircle(Gantry* gantry, Tracker* tracker)
{
	pos_store.push_back(POS1);
	pos_store.push_back(POS2);
	pos_store.push_back(POS3);
	pos_store.push_back(POS4);
	pos_store.push_back(POS5);
	pos_store.push_back(POS6);
	pos_store.push_back(POS7);
	pos_store.push_back(POS8);
	Candy detectedCandies;
	bool catched;
	for (auto it = pos_store.begin(); it < pos_store.end(); it++)
	{
		detectedCandies = tracker->getCandyOfColor(min, 10);
		catched = gantry->catchCandy(detectedCandies.getAngularVelocity(), detectedCandies.getCurrentPosition().getAngle(), detectedCandies.getCurrentPosition().getR());
		if (catched)
		{
			placed_candies.push_back(*it);

		}
	}


}
