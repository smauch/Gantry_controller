#include <cmath>
#include "Color.h"

Color::Color(int blue, int green, int red) {
    if (!Color::initialized) {
        vector<Color> Color::colorValues = generateColorValueList();
        Color::initialized = true;
    blue = blue;
    green = green;
    red = red;
}

Color::Color(double blue, double green, double red) {
    if (!Color::initialized) {
        vector<Color> Color::colorValues = generateColorValueList();
        Color::initialized = true;
    blue = (int) blue;
    green = (int) green;
    red = (int) red;
}

Color::getTotalDifference(cv::Scalar color) {
    double blueDifference = abs(blue - color[0]);
    double greenDifference = abs(green - color[1]);
    double redDifference = abs(red - color[2]);

    return blueDifference + redDifference + greenDifference;
}

vector<Color> generateColorValueList() {
    vector<Color> colorValues;
    colorValues.reserve(6);
    colorValues.addElement(Color(60, 140, 0));
    colorValues.addElement(Color(15, 0, 230));
    colorValues.addElement(Color(90, 30, 20));
    colorValues.addElement(Color(250, 190, 0));
    colorValues.addElement(Color(40, 55, 110));
    colorValues.addElement(Color(180, 100, 0));

    return colorValues;
}
