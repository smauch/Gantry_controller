#include <math.h>
#include <cmath>
#include "Candy.h"

Candy::Candy(Colors color, Polar currentPosition) {
    color = color;
    currentPosition = currentPosition;
    prevPosition = currentPosition;
    angularVelocity = 0;
}

Candy::Candy() {
    color = RED;
    currentPosition = Polar();
    prevPosition = currentPosition;
    angularVelocity = 0;
}

bool Candy::isSameObject(Polar otherObject, Colors otherColor) {
    if (color != otherColor) {
        return false;
    }

    double dx = abs(currentPosition.getXCartesianCoordinates() - otherObject.getXCartesianCoordinates());
    double dy = abs(currentPosition.getYCartesianCoordinates() - otherObject.getYCartesianCoordinates());
    
    if (dx > 90.0 || dy > 90.0) {
        return false;
    }

    return true;
}

void Candy::updateValues(Polar newPosition) {
    prevPosition = currentPosition;
    currentPosition = newPosition;
    angularVelocity = prevPosition.calculateStep(currentPosition);
}
