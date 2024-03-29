#ifndef COORDINATES_H 
#define COORDINATES_H
#define _USE_MATH_DEFINES
#include <math.h>

class Coordinates {
    private:
        /** radius **/
        double r;
        /** angle in radians **/
        double phi;
        /** x value **/
        double x;
        /** y value **/
        double y;
    public:
        // constructor
        Coordinates(double x = 0, double y = 0);
        // set from cartesian coordinates
        void fromCartesian(double x, double y);
        // set from polar coordinates
        void fromPolar(double r, double phi);
        // rotates the coordinate by x-degrees
        Coordinates rotate(int degree);
        // getter for radius
        double getR() { return r; };
        // getter for phi
        double getAngle() { return phi; };
        // getter for x
        double getX() { return x; };
        // getter for y
        double getY() { return y; };
};
#endif
