#ifndef WAITPATTERN_H 
#define WAITPATTERN_H

#include <iostream>
#include <vector>
#include "gantry/Gantry.h"
#include "cv/Camera.h"
#include "cv/ColorTracker.h"
#include "cv/Tracker.h"
#include "cv/Candy.h"
#include "cv/Color.h"
#include "C:/Users/hartm/Documents/develop/cv/Camera.h"
class WaitPattern
{

public:
	WaitPattern();
	~WaitPattern(); 
	bool candiesToRotary(Gantry* gantry);

protected:
	std::vector<const int[Gantry::NUM_AMP]> placed_candies;
	std::vector<const int[Gantry::NUM_AMP]>pos_store;
	const int INIT_POS[Gantry::NUM_AMP] = { 500,46350,0 }; //has to be adjusted
	const int CANDY_SIZE[3] = { 55,42, 10 };
	const int ROT_POS[Gantry::NUM_AMP] = { 0,0,0 }; //has to be adjusted

};

class RunningCandy: public WaitPattern
{
public:
	void placeCandy(Gantry* gantry, Tracker* tracker, Colors maj, Colors min);
	bool runCandy(Gantry* gantry, Tracker* tracker);



private:
	const int POS1[Gantry::NUM_AMP] = { INIT_POS[0] - 2 * CANDY_SIZE[0] , INIT_POS[1], 0 };
	const int POS2[Gantry::NUM_AMP] = { POS1[0] ,POS1[1] + CANDY_SIZE[1] , 0 };
	const int POS3[Gantry::NUM_AMP] = { POS2[0], POS2[1] + CANDY_SIZE[1] ,0 };
	const int POS4[Gantry::NUM_AMP] = { POS3[0], POS3[1] + CANDY_SIZE[1], 0 };
	const int POS5[Gantry::NUM_AMP] = { POS4[0], POS4[1] + CANDY_SIZE[1], 0 };


};

class CandyTower : public WaitPattern
{
public:
	bool buildTower(Gantry* gantry, Tracker* tracker, Colors maj, Colors min);


private:
	const int POS1[Gantry::NUM_AMP] = { INIT_POS[0], INIT_POS[1], INIT_POS[2] + CANDY_SIZE[2] };
	const int POS2[Gantry::NUM_AMP] = { INIT_POS[0], INIT_POS[1], POS1[2] + CANDY_SIZE[2] };
	const int POS3[Gantry::NUM_AMP] = { INIT_POS[0], INIT_POS[1], POS2[2] + CANDY_SIZE[2] };
	const int POS4[Gantry::NUM_AMP] = { INIT_POS[0], INIT_POS[1], POS3[2] + CANDY_SIZE[2] };
	const int POS5[Gantry::NUM_AMP] = { INIT_POS[0], INIT_POS[1], POS4[2] + CANDY_SIZE[2] };

};

class CandyCircle : public WaitPattern
{
public:
	void buildCircle(Gantry* gantry, Tracker* tracker);

private:
	const int POS1[Gantry::NUM_AMP] = {INIT_POS[0] + 2 * CANDY_SIZE[0], INIT_POS[1], 0};
	const int POS2[Gantry::NUM_AMP] = {POS1[0] - CANDY_SIZE[0], POS1[1] + CANDY_SIZE[1],0};
	const int POS3[Gantry::NUM_AMP] = {INIT_POS[0], INIT_POS[1] + 2 * CANDY_SIZE[1], 0};
	const int POS4[Gantry::NUM_AMP] = {POS3[0] - CANDY_SIZE[0], POS3[1] - CANDY_SIZE[1], 0};
	const int POS5[Gantry::NUM_AMP] = {INIT_POS[0] - 2 * CANDY_SIZE[0], INIT_POS[1],0};
	const int POS6[Gantry::NUM_AMP] = {POS5[0] + CANDY_SIZE[0],POS5[1] - CANDY_SIZE[1],0 };
	const int POS7[Gantry::NUM_AMP] = {INIT_POS[0], INIT_POS[1] - 2 * CANDY_SIZE[1],0};
	const int POS8[Gantry::NUM_AMP] = { POS7[0] + CANDY_SIZE[0], POS7[1] + CANDY_SIZE[1],0 };


};






#endif
