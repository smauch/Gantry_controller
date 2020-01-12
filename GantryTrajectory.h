#ifndef GANTRYTRAJECTORY_H 
#define GANTRYTRAJECTORY_H

#include "CML.h"

class GantryTrajectory : public LinkTrajectory
{

public:
	uunit xPos[200];
	uunit yPos[200];
	uunit zPos[200];
	uunit xVel[200];
	uunit yVel[200];
	uunit zVel[200];
	uunit trjTime[200];
	int posCounter = 0;

	virtual const Error* StartNew(void) { return 0; }

	virtual void Finish(void) {
		std::cout << "fertisch" << std::endl;
	}

	virtual int GetDim(void) {
		return 3;
	}

	virtual bool UseVelocityInfo(void) { return false; }

	virtual const Error* NextSegment(uunit pos[], uunit vel[], uint8& time) {
		//Samuels pure Virtual override
	
		pos[0] = xPos[posCounter];
		pos[1] = 0;
		pos[2] = 0;
		time = trjTime[posCounter];
		posCounter++;
		return 0;
	}
};
#endif