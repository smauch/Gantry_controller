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

	std::cout << "fertisch" << posCounter << std::endl;
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
	pos[1] = yPos[posCounter];
	pos[2] = zPos[posCounter];
	vel[0] = xVel[posCounter];
	vel[1] = yVel[posCounter];
	vel[2] = zVel[posCounter];
	time = trjTime[posCounter];
	posCounter++;
	if (posCounter >= NUMBER_POS_CALC) {
		posCounter = 0;
	}
	return 0;

}

void GantryTrajectory::circle(double radius, double angular, double angularVelTarget)
{
	this->radius = radius;
	this->angular = angular;
	double t = 0.01;
	double angularVelMax = angularVelTarget;
	double angularAccMax = M_PI / 4;

	//double angularMax = circluarArc + angular;

	for (int i = 0; i < NUMBER_POS_CALC; i++)
	{
		if (this->angularVel < angularVelMax) {
			this->angularAcc = angularAccMax;
		}
		else
		{
			this->angularAcc = 0;
		}

		this->angular = this->angular + this->angularVel * t + this->angularAcc * t * t / 2;
		this->angularVel = this->angularVel + this->angularAcc * t;

		xPos[i] = cos(this->angular) * this->radius + Gantry::DISC_CENTER_POS[0];
		yPos[i] = sin(this->angular) * this->radius + Gantry::DISC_CENTER_POS[1];
		zPos[i] = 0;

		xVel[i] = -sin(this->angular) * this->angularVel * this->radius;
		yVel[i] = cos(this->angular) * this->angularVel * this->radius;
		zVel[i] = 0;
		trjTime[i] = int(t * 1000);
	}

}



bool GantryTrajectory::calcMovement(uunit actPos[], double radius, double currAngular, double angularVelTarget)
{
	//Make the preiction
	std::cout << angularVelTarget << " rad /s" << std::endl;
	double angular = currAngular + angularVelTarget * 1200 / 1000; ;
	double t = 0.01;
	double angularVelMax = angularVelTarget;
	double angularAccMax = M_PI;
	double angularAcc = 0;
	double angularVel = 0;
	int tarTime = 100;
	this->posCounter = 0;

	double tarPos[3];
	tarPos[0] = radius * sin(angular) + Gantry::DISC_CENTER_POS[0] - actPos[0];
	tarPos[1] = radius * -cos(angular) + Gantry::DISC_CENTER_POS[1] - actPos[1];
	tarPos[2] = Gantry::CATCH_Z_HEIGHT - 5000 - actPos[2];

	for (int i = 0; i < NUMBER_POS_CALC/2; i++)
	{
		xPos[i] = tarPos[0] / 2 * sin(M_PI / tarTime * (i - tarTime / 2)) + tarPos[0] / 2 + actPos[0];
		yPos[i] = tarPos[1] / 2 * sin(M_PI / tarTime * (i - tarTime / 2)) + tarPos[1] / 2 + actPos[1];
		zPos[i] = tarPos[2] / 2 * sin(M_PI / tarTime * (i - tarTime / 2)) + tarPos[2] / 2 + actPos[2];
		trjTime[i] = int(t * 1000);
		//std::cout << xPos[i] << ", " << yPos[i] << ", " << zPos[i] << std::endl;
	}

	uunit actZPos = Gantry::CATCH_Z_HEIGHT - 5000;
	uunit zRel = 5000;
	int zTime = 50;


	for (int i = NUMBER_POS_CALC / 2; i < NUMBER_POS_CALC; i++)
	{
		if (i < NUMBER_POS_CALC / 2 * 1.04) {
			angularAcc = (angularVelMax - angularVel) / ((NUMBER_POS_CALC / 2 * 1.04 - i) / 100.0);
		}
		else if (i > NUMBER_POS_CALC * 0.95) {
			if (abs(angularVel) > 0) {
				angularAcc = -angularVel / ((NUMBER_POS_CALC - i) / 100.0);
			}
			else
			{
				angularAcc = 0;
			}	
		}
		else {
			angularAcc = 0;
		}
		
		angular = angular + angularVel * t + angularAcc * t * t / 2;
		angularVel = angularVel + angularAcc * t;
		xPos[i] = sin(angular) * radius + Gantry::DISC_CENTER_POS[0];
		yPos[i] = -cos(angular) * radius + Gantry::DISC_CENTER_POS[1];

		zPos[i] = zRel / 2 * sin(M_PI / zTime * (i - zTime / 2)) + zRel / 2 + actZPos;
		trjTime[i] = int(t * 1000);
		//std::cout << xPos[i] << ", " << yPos[i] << ", " << zPos[i] << ", " << angularVel << std::endl;
		trjTime[NUMBER_POS_CALC - 1] = 0;
	}
	return false;
}

