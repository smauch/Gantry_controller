//
// Created by max on 11/9/19.
//

#include <opencv2/opencv.hpp>
#include "helper.h"
#include <iostream>

const int IMAGE_SIZE = 1080;

using namespace std;

int isSafe(int** image, int row, int col, int** visited) {
    return ((row >= 0) && (row < IMAGE_SIZE) && (col >= 0) && (col < IMAGE_SIZE) &&
            (image[row][col] && !visited[row][col]));
}

void DFS(int** image, int row, int col, int** visited) {
    static int rowNbr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    static int colNbr[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    visited[row][col] = 1;

    for (int k = 0; k < 8; ++k) {
        if (isSafe(image, row + rowNbr[k], col + colNbr[k], visited)) {
            DFS(image, row + rowNbr[k], col + colNbr[k], visited);
        }
    }

}

int dfsForCVMat(int** image) {
    int **visited = getZero2DArray();

    int count = 0;
    for (int i = 0; i < IMAGE_SIZE; ++i) {
        for (int j = 0; j < IMAGE_SIZE; ++j) {
            if (image[i][j] && !visited[i][j]) {
                DFS(image, i, j, visited);
                ++count;
            }
        }
    }
    delete visited;
    return count;
}

