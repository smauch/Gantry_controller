#include "opencv2/opencv.hpp"
#include "Candy.h"
#include "ColorTracker.h"
#include "Tracker.h"
#include "Color.h"
#include "Camera.h"
#include <pylon/Device.h>
// Include files to use the PYLON API.

const char filename[] = "C:/Users/hartm/source/repos/cv/Config.pfs";

int main( int argc, char** argv ) {

	// Before using any pylon methods, the pylon runtime must be initialized. 
	Pylon::PylonInitialize();

	Pylon::CInstantCamera baslerCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
	baslerCamera.Open();
	Pylon::CFeaturePersistence::Load(filename, &(baslerCamera.GetNodeMap()), true);

	Camera camera(&baslerCamera);


	
    // initializing all the color trackers
    std::vector<ColorTracker> colorTrackers = {
        ColorTracker("Green", 50, 90, 27, 47, 3000, true), 
        ColorTracker("Red", 0, 0, 200, 100, 3000, true), 
        ColorTracker("Dark Blue", 140, 120, 96, 60, 4000, false), 
        ColorTracker("Yellow", 0, 150, 185, 77, 5000, true), 
        ColorTracker("Brown", 47, 99, 88, 65, 4000, true), 
        ColorTracker("Light Blue", 116, 223, 135, 44, 3000, false), 
    };

    int centerX = 546;
    int centerY = 546;
    int outerRadius = 540;
    int innerRadius = 90;

    Colors color = RED;

    cv::Mat currentImage = camera.grab(true);

	
    // adjust the values of the tracker
    colorTrackers[color].configure(currentImage);

    Tracker tracker(centerX, centerY, outerRadius, innerRadius, &baslerCamera, colorTrackers);


    tracker.configure(currentImage); // configure tracker

	//camera.getCamera()->StartGrabbing(20000);

	while (true) {
		cv::Mat maImage = camera.grab(true);
		std::vector<Candy> detectedCandies = tracker.getCandiesInFrame(color, maImage);

		for (int i = 0; i < detectedCandies.size(); i++) {
			maImage = tracker.markCandyInFrame(detectedCandies[i], maImage);
		}

		//std::cout << maImage.at<uchar>(20, 20) << std::endl;

		cv::imshow("Samuel <3", maImage);
		
		if (cv::waitKey(1) == 27) {
			break;
		}


	}



    // get chocolate
    // Candy foundCandy = tracker.getCandyOfColor(color, 20);

    // predict position
  //  Coordinates predictedPosition = foundCandy.predictPosition(90);
 //   cv::Point predictPositionPoint(predictedPosition.getX() + centerX, predictedPosition.getY() + centerY);


    // mark predicted point
   // cv::circle(currentImage, predictPositionPoint, 10, cv::Scalar(255, 0, 0), 12);
   // cv::imshow("test", currentImage);
   // cv::waitKey(0);
	
	baslerCamera.Close();
    return 0;
} 
