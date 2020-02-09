#ifndef CANDY_H 
#define CANDY_H
#include "Color.h"
#include "Coordinates.h"
/**
 * this class models a single piece of candy
 */
class Candy {
    private:
        /** color of the candy **/
        Colors color;
        /** current position of the candy **/
        Coordinates currentPosition;
        /** rotation angle in degree **/
        double rotationAngle;


    public:
        // constructors
        Candy(Colors color, Coordinates currentPosition);
        Candy();

        // getter currentPosition
        Coordinates getCurrentPosition() { return this->currentPosition; }
        // getter color
        Colors getColor() { return this->color; }
        // getter rotationAngle
        double getRotationAngle() { return this->rotationAngle;}

        // setter currentPosition
        void setCurrentPosition(Coordinates currentPosition) { this->currentPosition = currentPosition; }
        // setter rotationAngle
        void setRotationAngle(double rotationAngle) { this->rotationAngle = rotationAngle; }
};
#endif