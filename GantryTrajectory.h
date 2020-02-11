#ifndef GANTRYTRAJECTORY_H 
#define GANTRYTRAJECTORY_H
#include "CML.h"
#include <corecrt_math_defines.h>
#include <array>

using namespace CML;

class GantryTrajectory : public LinkTrajectory
{
private:
	const static int NUMBER_POS_CALC = 200;
	int posCounter = 0;
	uunit xPos[NUMBER_POS_CALC];
	uunit yPos[NUMBER_POS_CALC];
	uunit zPos[NUMBER_POS_CALC];
	uunit xVel[NUMBER_POS_CALC];
	uunit yVel[NUMBER_POS_CALC];
	uunit zVel[NUMBER_POS_CALC];
	uunit trjTime[NUMBER_POS_CALC];

	double angularAcc = 0;
	double angularVel = 0;
	double angular = M_PI;
	double radius = 0;
	double linVel = 0;
	double linAcc = 0;
public:
	//Constructor
	GantryTrajectory();
	~GantryTrajectory();
	virtual const Error* StartNew(void);
	virtual void Finish(void);
	virtual int GetDim(void);
	virtual bool UseVelocityInfo(void);
	virtual const Error* NextSegment(uunit pos[], uunit vel[], uint8& time);
	bool calcMovement(std::array<uunit, 3> actPos, double radius, double angular, double angularVelTarget,std::array<uunit, 3> targetPos, unsigned short maxTime);
	void circle(double radius, double angular, double angularVel);
	bool saveTrj();
};
#endif