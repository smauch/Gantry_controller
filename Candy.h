#ifndef CANDY_H 
#define CANDY_H
#include "Color.h"
#include "Polar.h"
class Candy {
    private:
        Colors color;
        Polar currentPosition;
        Polar prevPosition;
        double angularVelocity;

    public:
        Candy(Colors color, Polar currentPosition);
        Candy();

        void updateValues(Polar newPosition);
        bool isSameObject(Polar otherObject, Colors otherColor);

        Colors getColor() { return color; }
        Polar getCurrentPosition() { return currentPosition; }
        Polar getPrevPosition() { return prevPosition; }
        double getAngularVelocity() { return angularVelocity; }
};

#endif
