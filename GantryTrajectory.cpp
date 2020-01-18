#include <CML.h>
#include "GantryTrajectory.h"
#include <stdio.h>
#include <math.h>  
#include "Gantry.h"

CML_NAMESPACE_USE();

GantryTrajectory::GantryTrajectory()
{
}

GantryTrajectory::~GantryTrajectory()
{
}

const Error* GantryTrajectory::StartNew(void)
{ 
	return 0;
}

void GantryTrajectory::Finish(void)
{
	std::cout << "fertisch" << std::endl;
}

int GantryTrajectory::GetDim(void)
{
	return Gantry::NUM_AMP;
}

bool GantryTrajectory::UseVelocityInfo(void)
{
	return false;
}

const Error* GantryTrajectory::NextSegment(uunit pos[], uunit vel[], uint8& time) {
	//Samuels pure Virtual override

	pos[0] = xPos[posCounter];
	pos[1] = 0;
	pos[2] = 0;
	time = trjTime[posCounter];
	posCounter++;
	return 0;

}

bool GantryTrajectory::calcMovement(Linkage link, double angularVel, double angular, double radius, int time)
{
	//Check if radius is out of range
	if (radius < Gantry::DISC_RADIUS[0] || radius > Gantry::DISC_RADIUS[1])
	{
		printf("radius out of operation area");
		exit(1);
	}

	float predictedAngle = angular + (angularVel / 1000 * time);
	
	uunit actPos[Gantry::NUM_AMP];

	//get current motorPos should be lurk pos
	for (short i = 0; i < Gantry::NUM_AMP; i++)
	{
		link[i].GetPositionActual(actPos[i]);
	}

	xPos[0] = Gantry::LURK_POS[0];
	yPos[0] = Gantry::LURK_POS[1];
	zPos[0] = Gantry::LURK_POS[2];


	radius* sin(angular) + Gantry::DISC_CENTER_POS[0];
	radius * -cos(angular) + Gantry::DISC_CENTER_POS[1];
	Gantry::CATCH_Z_HEIGHT;
	

	double t = 0.01;
	double angularVelMax = PI / 4;
	double angularAccMax = PI / 4;
	for (int i = 0; i < NUMBER_POS_CALC; i++)
	{
		xPos[i] = 0;
		yPos[i] = 0;
		zPos[i] = 0;

		xVel[i] = 0;
		yVel[i] = 0;
		zVel[i] = 0;

		trjTime[i] = 0;
	}
//{
//	if (angularVel < angularVelMax) {
//		angularAcc = angularAccMax;
//	}
//	else
//	{
//		angularAcc = 0;
//	}

//	angular = angular + angular * t + angularAcc * t * t / 2;
//	angularVel = angularVel + angularAcc * t;

//	trj.xPos[i] = cos(angular) * radius + discCenterPos[0];
//	trj.yPos[i] = sin(angular) * radius + discCenterPos[1];
//	trj.zPos[i] = 20000;
//	trj.xVel[i] = -cos(angular) * angularVel * radius;
//	trj.yVel[i] = sin(angular) * angularVel * radius;
//	trj.zVel[i] = 0;
//	if (i < NUMBER_POS_CALC - 1) {
//		trj.trjTime[i] = t;
//	}
//	else
//	{
//		trj.trjTime[i] = 0;
//	}

//}

	return false;
}
