#include "Coordinates.h"

/**
 * constructor of Coordinates
 *
 * @param x x value
 * @param y y value
 */
Coordinates::Coordinates(double x, double y) {
    fromCartesian(x, y);
}

/**
 * sets the values of the object from cartesian coordinates
 *
 * @param x x value
 * @param y y value 
 */
void Coordinates::fromCartesian(double x, double y) {
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
void Coordinates::fromPolar(double r, double phi) {
    this->r = r;
    if (phi > (M_PI * 2)) {
        this -> phi = phi - (M_PI * 2);
    } else if (phi < 0) {
        this -> phi = phi + (M_PI * 2);
    } else {
        this->phi = phi;
    }
    this->x = r * cos(this -> phi);
    this->y = r * sin(this -> phi);
}

/**
 * returns this coordinate rotated by the given angle
 * 
 * @param degree given angle in degree
 * @return copy of the coordinate rotated by the given angle
 */
Coordinates Coordinates::rotate(int degree) {
    double angleInRadians = degree * M_PI / 180;
    int rotatedX = cos(angleInRadians) * x - sin(angleInRadians) * y;
    int rotatedY = sin(angleInRadians) * x + cos(angleInRadians) * y;

    return Coordinates(rotatedX, rotatedY);
}
