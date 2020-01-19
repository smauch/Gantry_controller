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
	return true;
}

const Error* GantryTrajectory::NextSegment(uunit pos[], uunit vel[], uint8& time) {
	//Samuels pure Virtual override
	pos[0] = xPos[posCounter];
	pos[1] = yPos[posCounter];
	pos[2] = 15000;
	vel[0] = xVel[posCounter];
	vel[1] = yVel[posCounter];
	vel[2] = 0;
	time = trjTime[posCounter];
	posCounter++;
	if (posCounter >= NUMBER_POS_CALC) {
		posCounter = 0;
		circle(this->radius,this->angular,this->targetVel);
	}
	return 0;

}

void GantryTrajectory::circle(double radius, double angular, double angularVelTarget)
{
	this->radius = radius;
	this->angular = angular;
	this->targetVel = angularVelTarget;
	double t = 0.01;
	double angularVelMax = this->targetVel;
	double angularAccMax = M_PI / 4;


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
		zPos[i] = sin(this->angular) * 5000 + 15000;
		xVel[i] = -sin(this->angular) * this->angularVel * this->radius;
		yVel[i] = cos(this->angular) * this->angularVel * this->radius;
		zVel[i] = cos(this->angular) * this->angularVel * 5000;;
		trjTime[i] = int(t * 1000);
	}

}

bool GantryTrajectory::calcMovement(uunit actPos[], double angularVel, double angular, double radius, int time)
{
	std::cout << actPos[0] << ", " << actPos[1] << ", " << actPos[2] << std::endl;
	//Check if radius is out of range
	if (radius < Gantry::DISC_RADIUS[0] || radius > Gantry::DISC_RADIUS[1])
	{
		printf("radius out of operation area");
		exit(1);
	}
	xPos[0] = actPos[0];
	yPos[0] = actPos[1];
	zPos[0] = actPos[2];

	//float predictedAngle = angular + (angularVel / 1000 * time);
	//radius* sin(angular) + Gantry::DISC_CENTER_POS[0];
	//radius * -cos(angular) + Gantry::DISC_CENTER_POS[1];
	//Gantry::CATCH_Z_HEIGHT;
	

	
	return false;
}
