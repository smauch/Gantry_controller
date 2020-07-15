
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
#include <State.h>
#include <AutoconfState.h>
#include <MaintenanceState.h>
#include <ServeState.h>


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

bool running = true;


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
	BackendModel statusModel(IDLE, "");

	utility::string_t m_port = U("34568");
	utility::string_t address = U("http://127.0.0.1:");
	address.append(m_port);
	serverOnInitialize(address, &statusModel);

	Camera camera;
	Pylon::PylonInitialize();
	Pylon::CTlFactory& tlFactory = Pylon::CTlFactory::GetInstance();
	Pylon::DeviceInfoList_t deviceList;
	Pylon::IPylonDevice* pDevice = NULL;
	Pylon::CInstantCamera baslerCamera = Pylon::CInstantCamera();
	std::vector<ColorTracker> colorTrackers;
	RotaryTable rotary;
	Tracker tracker;
	cv::Mat currentImage;
	Gantry gantry;
	Coordinates nextCandy;
	double myRadius;
	Candy detectedCandies;
	Colors requestedColor;

	// States
	AutoconfState confState;
	MaintenanceState maintState;
	ServeState serveState;

	statusModel.setReqStatus(AUTO_CONF);
	statusModel.setReqStatus(WAIT_PAT);

	while (running) {

	}

	statusModel.setReqStatus(SHUTDOWN);
	gantry.disable();
	serverOnShutdown();
	return 0;
}