#include <cmath>
#include "Color.h"
vector<Scalar> Color::colorValues = generateColorValues();
string Color::colorNames[6] = {"green", "red", "dark blue", "yellow", "brow", "light blue"};

Color::Color(int blueValue, int greenValue, int redValue) {
    blue = static_cast<double>(blueValue);
    green = static_cast<double>(greenValue);
    red = static_cast<double>(redValue);
}

Color::Color(double blueValue, double greenValue, double redValue) {
    blue = blueValue;
    green = greenValue;
    red = redValue;
}

Color::Color() {
    blue = 0.0;
    green = 0.0;
    red = 0.0;
}

double Color::getTotalDifference(cv::Scalar color) {
    double blueDifference = abs(blue - color[0]);
    double greenDifference = abs(green - color[1]);
    double redDifference = abs(red - color[2]);

    return blueDifference + redDifference + greenDifference;
}

Colors Color::getColor() {
    Colors fittingColor;
    double fittness = 3 * 400;
    for (int i = 0; i < Color::colorValues.size(); i++) {
        Scalar currentColor = Color::colorValues.at(i);
        double value = Color::getTotalDifference(currentColor);
        if (value < fittness) {
            fittness = value;
            fittingColor = static_cast<Colors>(i);
        }
    }

    return fittingColor;
}

Scalar Color::getAsScalar() {
    return Scalar(blue, green, red);
}

string Color::getAsString() {
    return format("[%f, %f, %f]", blue, green, red);
}

vector<Scalar> generateColorValues() {
        vector<Scalar> colorValueList = {
            Scalar(70, 190, 100),
            Scalar(25, 200, 160),
            Scalar(110, 120, 85),
            Scalar(30, 200, 185),
            Scalar(50, 110, 100),
            Scalar(90, 170, 120)
        };

        return colorValueList;
}

Scalar Color::getColorValue(Colors index) {
    return Color::colorValues.at(index);
}

string Color::getColorName(Colors index) {
    return Color::colorNames[index];
}
