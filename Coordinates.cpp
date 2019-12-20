#include "Coordinates.h"

/**
 * constructor of Coordinates
 *
 * @param x x value
 * @param y y value
 */
Coordinates::Coordinates(float x, float y) {
    fromCartesian(x, y);
}

/**
 * sets the values of the object from cartesian coordinates
 *
 * @param x x value
 * @param y y value 
 */
void Coordinates::fromCartesian(float x, float y) {
    this->x = x;
    this->y = y;
    if (x == 0 && y == 0 ) {
        this->r = 0;
        this->phi = 0;
        return;
    }
    this->r = sqrt(x * x + y * y);
    if (x == 0 && 0 < y) {
        this->phi = M_PI / 2;
    } else if (x == 0 && y < 0) {
        this->phi = M_PI * 3 / 2;
    } else if (x < 0) { //x != 0
        this->phi = atan(y / x) + M_PI;
    } else if (y < 0) {
        this->phi = atan(y / x) + 2 * M_PI;
    } else {
        this->phi = atan(y / x);
    }

}

/**
 * sets the values of the object from polar coordinates
 *
 * @param r radius
 * @param phi angle in radians
 */
void Coordinates::fromPolar(float r, float phi) {
    this->r = r;
    this->phi = phi;
    this->x = r * cos(this -> phi);
    this->y = r * sin(this -> phi);
}

/**
 * calculates the difference in angle between two coordinates
 *
 * @param nextValue the other coordinate
 * @return the difference in angle
 */
float Coordinates::calculateStep(Coordinates nextValue) {
    double nextAngle = nextValue.getAngle();
    double value = abs(nextValue.getAngle() - phi);
    if (value > M_PI) {
        value -= M_PI * 2;
    }

    if (nextAngle < phi) {
        value *= -1;
    }
    return value;
}
