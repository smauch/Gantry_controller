#include <math.h>
#include <cmath>
#include <iostream>
#include "Candy.h"

/**
 * constructor of the candy class
 *
 * @param color enum color
 * @param currentPosition current position of the candy
 */
Candy::Candy(Colors color, Coordinates currentPosition) {
    this -> color = color;
    this -> currentPosition = currentPosition;
    this -> angularVelocity = 0;
}

/**
 * default constructor of candy
 */
Candy::Candy() {
    this -> color = RED;
    this -> currentPosition = Coordinates();
    this -> angularVelocity = 0;
}

/**
 * checks if the this object and otherObject are the same 
 * based on distance
 *
 * @param otherObject the other object
 * @return true if the distance between two objects is not too big
 */
bool Candy::isSameObject(Candy otherObject) {
    double dx = abs(currentPosition.getX() - otherObject.getCurrentPosition().getX());
    double dy = abs(currentPosition.getY() - otherObject.getCurrentPosition().getY());

    if (dx > 25.0 || dy > 25.0) {
        return false;
    }

    return true;
}

/**
 * predicts the position of the candy in X frames
 *
 * @param frames prediciton for how many frames in the future
 * @return the predicited position of the candy
 */
Coordinates Candy::predictPosition(int frames) {
    float predictedAngle = currentPosition.getAngle() + (angularVelocity * frames);
    float radius = currentPosition.getR();
    Coordinates output;
    output.fromPolar(radius, predictedAngle);

    return output;
}

/**
 * updates the values of the candy
 *
 * @param newPosition new position of the candy
 * @param stepSize by how many units should the angularVelocity be adjusted
 */
void Candy::updateValues(Coordinates newPosition, double stepSize) {
    this -> angularVelocity += currentPosition.calculateStep(newPosition) * stepSize;
    this -> currentPosition = newPosition;
}
