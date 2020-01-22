#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

#include "opencv2/opencv.hpp"

#include <pylon/PylonIncludes.h>
#include <pylon/PylonUtilityIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

#include "imageStuff.h"

/**
 * class for containing information about the camera
 */
class Camera {
	private:
        // the camera object
        Pylon::CInstantCamera* camera;
        

   public:
        // constructor
        Camera();
        Camera(Pylon::CInstantCamera *baslerCamera);
        // configures the camera with the given config file
        void configure(const char filename[]);
        // grabs either one image or multiple
        cv::Mat grab(bool singleFrame=false);
		// prints information about the camera
        void print() {
			std::cout << camera->GetDeviceInfo().GetModelName() << std::endl;
		}
        // getter camera
		Pylon::CInstantCamera* getCamera() { return camera; }
        
		
};
#endif /** CAMERA_H **/
