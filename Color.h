#ifndef COLOR_H
#define COLOR_H

#include <vector>
#include "opencv2/opencv.hpp"

using namespace std;
enum Colors {GREEN,
    RED,
    DARK_BLUE,
    YELLOW,
    BROWN,
    LIGHT_BLUE};

class Color {
    private:
        static vector<Color> colorValues;
        static bool initialized;
        int blue;
        int green;
        int red;
        Colors color;

    public:
        Color(int blue, int green, int red);
        Color(double blue, double green, double red);


        double getSquareError(cv::Scalar);
};

bool Color::initialized = false;
#endif
