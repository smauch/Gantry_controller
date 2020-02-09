#include <CML.h>
#include "GantryTrajectory.h"
#include <stdio.h>
#include <math.h>  
#include "Gantry.h"
#include <interpolation.h>
#include <fstream>

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

void GantryTrajectory::cubicSpline()
{
	int t_interp[5] = { 0,1,2,3,4 };
	alglib::spline1dinterpolant cubic_spline_x;
	alglib::spline1dinterpolant cubic_spline_y;
	alglib::spline1dinterpolant cubic_spline_z;
}





bool GantryTrajectory::calcMovement(uunit actPos[], double radius, double ang, double angVel, const uunit targetPos[])
{
	uunit splPointX[2], splPointY[2];
	uunit splPointZ[3] = { actPos[2], Gantry::CATCH_Z_HEIGHT, targetPos[2] };
	uunit splVelX[2], splVelY[2];
	uunit splVelZ[3] = { 0,0,0 };
	double time[5] = { 0, 49, 99, 149, 199 };

	alglib::spline1dinterpolant hermiteX, hermiteY, hermiteZ;

	alglib::real_1d_array realPointX, realPointY, realPointZ;
	alglib::real_1d_array realVelX, realVelY, realVelZ;
	alglib::real_1d_array realT1, realT2, realT3;

	realPointX.attach_to_ptr(2, &(splPointX[0]));
	realPointY.attach_to_ptr(2, &(splPointY[0]));
	realPointZ.attach_to_ptr(3, &(splPointZ[0]));
	realVelX.attach_to_ptr(2, &(splVelX[0]));
	realVelY.attach_to_ptr(2, &(splVelY[0]));
	realVelZ.attach_to_ptr(3, &(splVelZ[0]));
	// Predict
	//ang = ang + angVel * 0.5;
 
	//Poit calcs
	//Sction 1
	splPointX[0] = actPos[0];
	splPointY[0] = actPos[1];
	splVelX[0] = 0;
	splVelY[0] = 0;


	splPointX[1] = radius * sin(ang) + Gantry::DISC_CENTER_POS[0];
	splPointY[1] = radius * -cos(ang) + Gantry::DISC_CENTER_POS[1];
	splVelX[1] = radius * cos(ang) * angVel / 100.0;
	splVelY[1] = radius* sin(ang)* angVel / 100.0;


	//Only the first two elements
	realT1.attach_to_ptr(2, &(time[0]));
	alglib::spline1dbuildhermite(realT1, realPointX, realVelX, hermiteX);
	alglib::spline1dbuildhermite(realT1, realPointY, realVelY, hermiteY);

	for (int t = 0; t < 50; t++)
	{
		xPos[t] = alglib::spline1dcalc(hermiteX, t);
		yPos[t] = alglib::spline1dcalc(hermiteY, t);
		zPos[t] = actPos[2];
		trjTime[t] = 30;
	}


	//Only the first two elements
	realT2.attach_to_ptr(3, &(time[1]));
	alglib::spline1dbuildhermite(realT2, realPointZ, realVelZ, hermiteZ);

	for (int t = 50; t < 150; t++)
	{
		ang = ang + angVel * 0.03;
		xPos[t] = radius * sin(ang) + Gantry::DISC_CENTER_POS[0];
		yPos[t] = radius * -cos(ang) + Gantry::DISC_CENTER_POS[1];
		zPos[t] = alglib::spline1dcalc(hermiteZ, t);
		trjTime[t] = 30;
	}

	//ang = ang + angVel * 0.01;
	splPointX[0] = radius * sin(ang) + Gantry::DISC_CENTER_POS[0];
	splPointY[0] = radius * -cos(ang) + Gantry::DISC_CENTER_POS[1];
	splVelX[0] = radius * cos(ang) * angVel / 100.0;
	splVelY[0] = radius * sin(ang) * angVel / 100.0;

	splPointX[1] = targetPos[0];
	splPointY[1] = targetPos[1];
	splVelX[1] = 0;
	splVelY[1] = 0;

	realT3.attach_to_ptr(2, &(time[3]));
	alglib::spline1dbuildhermite(realT3, realPointX, realVelX, hermiteX);
	alglib::spline1dbuildhermite(realT3, realPointY, realVelY, hermiteY);

	for (int t = 150; t < 200; t++)
	{
		xPos[t] = alglib::spline1dcalc(hermiteX, t);
		yPos[t] = alglib::spline1dcalc(hermiteY, t);
		zPos[t] = targetPos[2];
		trjTime[t] = 30;
	}
	trjTime[NUMBER_POS_CALC - 1] = 0;
	std::ofstream xFile("xPos.txt");
	std::ofstream yFile("yPos.txt");
	std::ofstream zFile("zPos.txt");

	for (int count = 0; count < 200; count++) {
		xFile << xPos[count] << std::endl;
		yFile << yPos[count] << std::endl;
		zFile << zPos[count] << std::endl;
	}

	std::cout << "finished" << std::endl;
	return false;
}

