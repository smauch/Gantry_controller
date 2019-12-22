#include "opencv2/opencv.hpp"
#include "Candy.h"
#include "ColorTracker.h"
#include "Tracker.h"
#include "Color.h"
// Include files to use the PYLON API.
#include <pylon/PylonIncludes.h>
#include <pylon/Device.h>
#include <pylon/PylonUtilityIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

const char Filename[] = "Config.pfs";

using namespace Pylon;


int main( int argc, char** argv ) {

	// Before using any pylon methods, the pylon runtime must be initialized. 
	PylonInitialize();
	CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());

	
	try
	{
		// Create an instant camera object with the camera device found first.
		
		camera.Open();

		// Print the model name of the camera.
		std::cout << "Using device " << camera.GetDeviceInfo().GetModelName() << std::endl;

		// Just for demonstration, read the content of the file back to the camera's node map with enabled validation.
		CFeaturePersistence::Load(Filename, &camera.GetNodeMap(), true);
		camera.Close();
	}

	catch (const GenericException &e)
	{
		// Error handling.
		std::cerr << "An exception occurred." << std::endl
			<< e.GetDescription() << std::endl;
	}


    cv::VideoCapture cap(1);
	/*
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

    cv::Mat currentImage;
    cap >> currentImage;

    // adjust the values of the tracker
    colorTrackers[color].configure(currentImage);

    Tracker tracker(centerX, centerY, outerRadius, innerRadius, cap, colorTrackers);
    tracker.configure(currentImage); // configure tracker

    // get chocolate
    Candy foundCandy = tracker.getCandyOfColor(color, 60);

    // predict position
    Coordinates predictedPosition = foundCandy.predictPosition(90);
    cv::Point predictPositionPoint(predictedPosition.getX() + centerX, predictedPosition.getY() + centerY);
	*/
	cv::Mat currentImage;
    cap >> currentImage;

    for (int i = 0; i < 90; i--) {
        cap >> currentImage;
		cv::imshow("Martin <3", currentImage);

		std::cout << currentImage.size() << std::endl;

		if (cv::waitKey(5) == 27) {
			break;
		}
    }



    // mark predicted point
  /*  cv::circle(currentImage, predictPositionPoint, 10, cv::Scalar(255, 0, 0), 12);
    cv::imshow("test", currentImage);
    cv::waitKey(0);
	*/
    return 0;
} 
