#ifndef COLOR_H
#define COLOR_H

#include <vector>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

enum Colors {GREEN,
    RED,
    DARK_BLUE,
    YELLOW,
    BROWN,
    LIGHT_BLUE};

class Color {
    private:
        static vector<Scalar> colorValues;
        static string colorNames[6];
        double blue;
        double green;
        double red;
        Colors color;

        double getTotalDifference(cv::Scalar);
    public:
        Color(int blueValue, int greenValue, int redValue);
        Color(double blueValue, double greenValue, double redValue);

        Colors getColor();
        Scalar getAsScalar();
        string getAsString();

        static Scalar getColorValue(Colors index);
        static string getColorName(Colors index);

};
vector<Scalar> generateColorValues();
#endif
