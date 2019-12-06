#include <math.h>
#include <cmath>
#include "Polar.h"

Polar::Polar(double radiusValue, double angleValue) {
    radius = radiusValue;
    angle = angleValue;
}

Polar::Polar(int x, int y) {
    radius = sqrt(pow(x, 2) + pow(y, 2));
    angle = atan(y/x);
}

Polar::Polar() {
    radius = 0;
    angle = 0;
}

void Polar::add(double addedAngle) {
    angle = (angle + addedAngle);
    if (angle >= 2 * M_PI) {
        angle -= 2 * M_PI;
    } else if (angle < 0) {
        angle += 2 * M_PI;
    }
}



double Polar::calculateStep(Polar nextValue) {
    double nextAngle = nextValue.getAngle();
    double value = abs(nextValue.getAngle() - angle);
    if (value > 180) {
        value -= 180 * 2;
    }
    return value;
}

double Polar::getXCartesianCoordinates() {
    double x = radius * cos(angle);
    return x;
}

double Polar::getYCartesianCoordinates() {
    double y = radius * sin(angle);
    return y;

}
