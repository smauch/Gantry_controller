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

class Camera {
	private:
        Pylon::CInstantCamera* camera;
        

   public:
	    Camera();
        Camera(Pylon::CInstantCamera *baslerCamera);
        void configure(const char filename[]);
        cv::Mat grab(bool singleFrame=false);
		void print() {
			std::cout << camera->GetDeviceInfo().GetModelName() << std::endl;
		}
		Pylon::CInstantCamera* getCamera() { return camera; }
        
		
};
#endif /** CAMERA_H **/
