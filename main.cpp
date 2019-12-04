#include <opencv2/opencv.hpp>
#include <sys/resource.h>
#include "helper.h"
using namespace cv;
using namespace std;
using namespace std::chrono;

void pipeline(char* imagePath) {
    Mat image = imread(imagePath, IMREAD_GRAYSCALE);
    Mat croppedImage = circleROI(image);
    Mat output;
    croppedImage.convertTo(output, DataType<int>::type);
    int **array = convertMatToArray(output);
    int **detectedColor = detectColor(array, 155);
    delete array;
    dfsForCVMat(detectedColor);
    Mat mat = convertArrayToMat(detectedColor);
}

int main( int argc, char** argv ) {

    const rlim_t kStackSize = 128 * 1024 * 1024;   // min stack size = 16 MB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    auto start = high_resolution_clock::now();
    
    //for (int i = 1; i < argc; i++) {
        pipeline(argv[i]);
    //}

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() << endl;
    return 0;
} 
