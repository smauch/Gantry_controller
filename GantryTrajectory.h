#ifndef GANTRYTRAJECTORY_H 
#define GANTRYTRAJECTORY_H
#include "CML.h"

using namespace CML;

class GantryTrajectory : public LinkTrajectory
{
private:
	const int NUMBER_POS_CALC = 200;
	int posCounter = 0;
	uunit xPos[200];
	uunit yPos[200];
	uunit zPos[200];
	uunit xVel[200];
	uunit yVel[200];
	uunit zVel[200];
	uunit trjTime[200];


public:
	//Constructor
	GantryTrajectory();
	~GantryTrajectory();
	virtual const Error* StartNew(void);
	virtual void Finish(void);
	virtual int GetDim(void);
	virtual bool UseVelocityInfo(void);
	virtual const Error* NextSegment(uunit pos[], uunit vel[], uint8& time);
	bool calcMovement(Linkage link, double angularVel, double angular, double radius, int time);
};
#endif