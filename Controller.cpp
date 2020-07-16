//States
#include <State.h>
// Own Classes 
#include <Gantry.h>
#include <Color.h>
#include <BackendModel.h>
#include <BackendServer.h>
#include <Camera.h>
#include <Rotarytable.h>
#include <Candy.h>
#include <ColorTracker.h>
#include <Tracker.h>
#include <WaitPattern.h>

//States
#include <AutoconfState.h>
#include <MaintenanceState.h>
#include <ServeState.h>
#include <WaiteState.h>

#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <pylon/Device.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <fstream>
#include <iostream>
//#include <WaitPattern.h>
//#include <windows.h>
//#include <shellapi.h>


std::unique_ptr<handler> g_httpHandler;


void serverOnInitialize(const utility::string_t& address, BackendModel* model)
{
	web::uri_builder uri(address);
	auto addr = uri.to_uri().to_string();
	g_httpHandler = std::unique_ptr<handler>(new handler(addr, model));
	g_httpHandler->open().wait();
	ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;
	return;
}

void serverOnShutdown()
{
	g_httpHandler->close().wait();
	return;
}


int main(int argc, const char* argv[]) {

	const char CANDIES_CONFIG[] = "./parameters/CADIES_CONFIG.json";
	const char TRACKER_CONFIG[] = "./parameters/TRACKER_CONFIG.json";
	const char CASCADE_FILE[] = "./parameters/cascade.xml";
	const char PYLON_CONFIG[] = "./parameters/basler_ace.pfs";
	const std::array<std::string,3> AMP_CONFIG{"./parameters/X-Axis.ccx", "./parameters/Y-Axis.ccx", "./parameters/Z-Axis.ccx"};

	Camera camera;
	Pylon::PylonInitialize();
	Pylon::CTlFactory& tlFactory = Pylon::CTlFactory::GetInstance();
	Pylon::DeviceInfoList_t deviceList;
	Pylon::IPylonDevice* pDevice = NULL;
	Pylon::CInstantCamera baslerCamera = Pylon::CInstantCamera();
	std::vector<ColorTracker> colorTrackers;
	
	Tracker tracker;
	cv::Mat currentImage;
	Gantry gantry;
	RotaryTable rotary;
	Coordinates nextCandy;
	double myRadius;
	Candy detectedCandies;
	Colors requestedColor;

	// States
	utility::string_t address = U("http://127.0.0.1:34568");
	BackendModel model(BOOT, "");
	serverOnInitialize(address, &model);
	

	//Initialize 
	pDevice = tlFactory.CreateFirstDevice();
	baslerCamera.Attach(pDevice);
	baslerCamera.Open();
	Pylon::CFeaturePersistence::Load(PYLON_CONFIG, &(baslerCamera.GetNodeMap()), true);
	camera = Camera(&baslerCamera);

	colorTrackers = {
			ColorTracker("Green", 99, 149, 115),
			ColorTracker("Red", 169, 161, 110),
			ColorTracker("Dark Blue", 132, 117, 76),
			ColorTracker("Yellow", 133, 181, 166),
			ColorTracker("Brown", 140, 136, 94),
			ColorTracker("Light Blue", 154, 81, 81),
	};

	tracker = Tracker::getTrackerFromJson(TRACKER_CONFIG, camera, colorTrackers, CASCADE_FILE);
	bool status = gantry.networkSetup();
	gantry.attachAmpConifg(AMP_CONFIG);
	status = gantry.initGantry(30000);

	status = rotary.initNetwork();
	bool worked = rotary.initMotor();

	
	//ShutdownState shutdown(&model, SHUTDOWN);
	//MaintState maitenance(&model, MAINTENANCE);

	AutoconfState autoConf(&model, AUTO_CONF, &gantry, &rotary, &tracker);
	ServeState serving(&model, SERVE, &gantry, &rotary, &tracker);
	WaitState waiting(&model, WAIT_PAT, &gantry, &rotary, &tracker);

	model.setAvailableCandies(std::set<Colors>{RED, GREEN, YELLOW, BROWN, LIGHT_BLUE, DARK_BLUE});
	model.setReqStatus(AUTO_CONF);


	while (model.getStatus() != SHUTDOWN)
	{
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}

	serverOnShutdown();
	return 0;
}