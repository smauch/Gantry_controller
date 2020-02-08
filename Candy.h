#ifndef CANDY_H 
#define CANDY_H
#include "Color.h"
#include "Coordinates.h"
#include <math.h>
#include <cmath>
#include <iostream>
#define _USE_MATH_DEFINES
/**
 * this class models a single piece of candy
 */
class Candy {
    private:
        /** color of the candy **/
        Colors color;
        /** current position of the candy **/
        Coordinates currentPosition;
        /** angular velocity of the candy **/
        double angularVelocity;


    public:
        // constructors
        Candy(Colors color, Coordinates currentPosition);
        Candy();

        // getter currentPosition
        Coordinates getCurrentPosition() { return this->currentPosition; }
       
        // getter angularVelocity
        double getAngularVelocity() { return this->angularVelocity; }
        // getter color
        Colors getColor() { return this->color; }

        // setter currentPosition
        void setCurrentPosition(Coordinates currentPosition) { this->currentPosition = currentPosition; }
};

#endif
