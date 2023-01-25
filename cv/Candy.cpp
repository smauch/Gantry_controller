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
    this -> rotationAngle = 100;
}

/**
 * default constructor of candy
 */
Candy::Candy() {
    this -> color = RED;
    this -> currentPosition = Coordinates();
    this -> rotationAngle = 100;
}
