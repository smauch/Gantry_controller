#ifndef COORDINATES_H 
#define COORDINATES_H
#include <math.h>
#include <iostream>

class Coordinates {
    private:
        /** radius **/
        float r;
        /** angle in radians **/
        float phi;
        /** x value **/
        float x;
        /** y value **/
        float y;
    public:
        // constructor
        Coordinates(float x = 0, float y = 0);
        // set from cartesian coordinates
        void fromCartesian(float x, float y);
        // set from polar coordinates
        void fromPolar(float r, float phi);
        // difference in angle between two coordinates
        float calculateStep(Coordinates nextValue); 
        // getter for radius
        float getR() {
            return r;
        };
        // getter for phi
        float getAngle() {
            return phi;
        };
        // getter for x
        float getX() {
            return x;
        };
        // getter for y
        float getY() {
            return y;
        };
};
#endif
