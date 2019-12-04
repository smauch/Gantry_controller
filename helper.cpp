#include "helper.h"
using namespace cv;
using namespace std;

const int IMAGE_SIZE = 1080;

Mat convertArrayToMat(int **array) {
    Mat output(IMAGE_SIZE, IMAGE_SIZE, CV_8U);

    for (int i = 0; i < IMAGE_SIZE; ++i) {
        for (int j = 0; j < IMAGE_SIZE; ++j) {
            output.at<uchar>(i, j) = array[i][j];
        }
    }

    return output;
}

int** getZero2DArray() {
    int **array = 0;
    array = new int*[IMAGE_SIZE];

    for (int i = 0; i < IMAGE_SIZE; ++i) {
        array[i] = new int[IMAGE_SIZE];
        for (int j = 0; j < IMAGE_SIZE; ++j) {
            array[i][j] = 0;
        }
    }
    return array;
}

void printMatrix(Mat m) {
    int height = m.size().height;
    int width = m.size().width;

    cout << height << " " << width << endl;

    for (int i = 0; i < height; ++i) {
        cout << "[ ";
        for (int j = 0; j < width; ++j) {
            cout << +m.at<uchar>(i, j) << " ";
        }
        cout << "]" << endl;
    }
}

void printArray(int array[IMAGE_SIZE][IMAGE_SIZE]) {
    for (int i = 0; i < IMAGE_SIZE; ++i) {
        cout << "[ ";
        for (int j = 0; j < IMAGE_SIZE; ++j) {
            cout << array[i][j] << " ";
        }
        cout << "]" << endl;
    }
}

int **convertMatToArray(Mat mat) {
    int** outputArray = getZero2DArray();

    for (int i = 0; i < IMAGE_SIZE; ++i) {
        for (int j = 0; j < IMAGE_SIZE; ++j) {
            outputArray[i][j] = mat.at<int>(i, j);
        }
    }

    return outputArray;
}
