
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
#include <windows.h>
#include <shellapi.h>

std::unique_ptr<handler> g_httpHandler;

void on_initialize(const string_t& address, BackendModel* model)
{
	uri_builder uri(address);
	auto addr = uri.to_uri().to_string();
	g_httpHandler = std::unique_ptr<handler>(new handler(addr, model));
	g_httpHandler->open().wait();
	ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;
	return;
}

void on_shutdown()
{
	g_httpHandler->close().wait();
	return;
}

int main() {
	BackendModel statusModel(IDLE, U(""), std::set<Colors>{GREEN, YELLOW, LIGHT_BLUE, DARK_BLUE, BROWN, RED });
	utility::string_t m_port = U("34568");
	int merker = 0;
	utility::string_t address = U("http://127.0.0.1:");
	address.append(m_port);

	on_initialize(address, &statusModel);
}