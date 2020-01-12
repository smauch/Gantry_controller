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
        /** angular velocity of the candy **/
        double angularVelocity;

    public:
        // constructors
        Candy(Colors iColor, Coordinates iCurrentPosition);
        Candy();

        // updates the attributes
        void updateValues(Coordinates newPosition, int neededTime, double stepSize);
        
        // checks if two candies are the same
        bool isSameObject(Candy otherObject);

        // getter currentPosition
        Coordinates getCurrentPosition() { return currentPosition; }
        // getter angularVelocity
        double getAngularVelocity() { return angularVelocity; }

        // predicts the position in X frames
        Coordinates predictPosition(int frames);
};

#endif
