#include <CML.h>
#include "GantryTrajectory.h"
#include <stdio.h>
#include <math.h>  
#include "Gantry.h"
#include <interpolation.h>
#include <fstream>
#include <filesystem>
#include <direct.h>
#include <stdexcept>


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

bool GantryTrajectory::saveTrj(double rad, double ang, double angVel)
{
	char rootDir[] = ".\\log";
	if (!std::filesystem::exists(rootDir)) {
		if (!_mkdir(rootDir)) {
			return false;
		}
	}
	std::ofstream xFile;
	std::ofstream yFile;
	std::ofstream zFile;
	std::ofstream detectedCandyFile;
	xFile.open(".\\log\\xPos.txt", std::ios::out);
	yFile.open(".\\log\\yPos.txt", std::ios::out);
	zFile.open(".\\log\\zPos.txt", std::ios::out);
	detectedCandyFile.open(".\\log\\detectedCandyFile.txt", std::ios::out);
	if (xFile.is_open() && yFile.is_open() && zFile.is_open()) {
		detectedCandyFile << "Angular" << ang << std::endl;
		detectedCandyFile << "AngVel" << angVel << std::endl;
		detectedCandyFile << "radius" << rad << std::endl;

		for (int count = 0; count < NUMBER_POS_CALC; count++) {
			xFile << xPos[count] << std::endl;
			yFile << yPos[count] << std::endl;
			zFile << zPos[count] << std::endl;
		}
		xFile.close();
		yFile.close();
		zFile.close();
	}
	else {
		throw std::runtime_error("File not written");
	}
}

void GantryTrajectory::attachMoveLimits(std::array<SoftPosLimit, 3> posLimit)
{
	this->posLimit = posLimit;
}




void GantryTrajectory::calcMovement(std::array<uunit, 3> actPos, double radius, double ang, double angVel, std::array<uunit, 3> dropPos, unsigned short maxTime)
{
	double stepTime = 0.015;
	// Predict
	ang = ang + angVel * 0.9;

	uunit splPointX[2], splPointY[2];
	uunit splPointZ[4] = { actPos[2], Gantry::CATCH_Z_HEIGHT, actPos[2], dropPos[2] };
	uunit splVelX[2], splVelY[2];
	uunit splVelZ[4] = { 0,0,0,0 };
	double time[5] = { 0, 49, 99, 149, 199 };

	alglib::spline1dinterpolant hermiteX, hermiteY, hermiteZ;

	alglib::real_1d_array realPointX, realPointY, realPointZ, realPointZ2;
	alglib::real_1d_array realVelX, realVelY, realVelZ, realVelZ2;
	alglib::real_1d_array realT1, realT2, realT3;

	realPointX.attach_to_ptr(2, &(splPointX[0]));
	realPointY.attach_to_ptr(2, &(splPointY[0]));
	realPointZ.attach_to_ptr(3, &(splPointZ[0]));
	realPointZ2.attach_to_ptr(2, &(splPointZ[2]));
	realVelX.attach_to_ptr(2, &(splVelX[0]));
	realVelY.attach_to_ptr(2, &(splVelY[0]));
	realVelZ.attach_to_ptr(3, &(splVelZ[0]));
	realVelZ2.attach_to_ptr(2, &(splVelZ[2]));
	
	
 
	//Poit calcs
	//Sction 1
	splPointX[0] = actPos[0];
	splPointY[0] = actPos[1];
	splVelX[0] = 0;
	splVelY[0] = 0;


	splPointX[1] = radius * sin(ang) + Gantry::DISC_CENTER_POS[0];
	splPointY[1] = radius * -cos(ang) + Gantry::DISC_CENTER_POS[1];

	splVelX[1] = radius * cos(ang) * angVel * stepTime;
	splVelY[1] = radius* sin(ang)* angVel * stepTime;


	//Only the first two elements
	realT1.attach_to_ptr(2, &(time[0]));
	alglib::spline1dbuildhermite(realT1, realPointX, realVelX, hermiteX);
	alglib::spline1dbuildhermite(realT1, realPointY, realVelY, hermiteY);

	for (int t = 0; t < 50; t++)
	{
		xPos[t] = alglib::spline1dcalc(hermiteX, t);
		yPos[t] = alglib::spline1dcalc(hermiteY, t);
		zPos[t] = actPos[2];
		trjTime[t] = stepTime * 1000;
	}


	//Only the first two elements
	realT2.attach_to_ptr(3, &(time[1]));
	alglib::spline1dbuildhermite(realT2, realPointZ, realVelZ, hermiteZ);

	for (int t = 50; t < 150; t++)
	{
		ang = ang + angVel * stepTime;
		xPos[t] = radius * sin(ang) + Gantry::DISC_CENTER_POS[0];
		yPos[t] = radius * -cos(ang) + Gantry::DISC_CENTER_POS[1];
		zPos[t] = alglib::spline1dcalc(hermiteZ, t);
		trjTime[t] = stepTime * 1000;
	}

	splPointX[0] = radius * sin(ang) + Gantry::DISC_CENTER_POS[0];
	splPointY[0] = radius * -cos(ang) + Gantry::DISC_CENTER_POS[1];


	splVelX[0] = radius * cos(ang) * angVel * stepTime;
	splVelY[0] = radius * sin(ang) * angVel * stepTime;

	splPointX[1] = dropPos[0];
	splPointY[1] = dropPos[1];
	splVelX[1] = 0;
	splVelY[1] = 0;


	realT3.attach_to_ptr(2, &(time[3]));

	alglib::spline1dbuildhermite(realT3, realPointX, realVelX, hermiteX);
	alglib::spline1dbuildhermite(realT3, realPointY, realVelY, hermiteY);
	alglib::spline1dbuildhermite(realT3, realPointZ2, realVelZ2, hermiteZ);

	for (int t = 150; t < 200; t++)
	{
		xPos[t] = alglib::spline1dcalc(hermiteX, t);
		yPos[t] = alglib::spline1dcalc(hermiteY, t);
		zPos[t] = alglib::spline1dcalc(hermiteZ, t);
		trjTime[t] = stepTime * 1000;
	}
	trjTime[NUMBER_POS_CALC - 1] = 0;
	
	//Check safe calc
	for (int t = 0; t < NUMBER_POS_CALC; t++)
	{
		if ((posLimit[0].neg > xPos[t]) || (xPos[t] > (posLimit[0].pos + 1000))) {
			std::cerr << "Trajectory out of software position limit, X-Value: " << xPos[t] << "radius:" << radius << "angVel:" << angVel << std::endl;
			throw std::out_of_range("Trajectroy out of software X position limit");
		}
		if ((posLimit[1].neg > yPos[t]) || (yPos[t] > (posLimit[1].pos + 1000))) {
			std::cerr << "Trajectory out of software position limit, Y-Value:" << yPos[t] << "radius:" << radius << "angVel:" << angVel << std::endl;
			throw std::out_of_range("Trajectroy out of software position limit");
		}
		if ((posLimit[2].neg > zPos[t]) || (zPos[t] > (posLimit[2].pos + 1000))) {
			std::cerr << "Trajectory out of software position Z limit, Z-Value:" << zPos[t] << "radius:" << radius << "angVel:" << angVel << std::endl;
			throw std::out_of_range("Trajectroy out of software position limit");
		}
	}
}

