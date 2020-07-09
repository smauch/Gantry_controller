#include "BackendServer.h"
#include "BackendModel.h"
//#include "cpprest/uri.h"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;


std::unique_ptr<handler> g_httpHandler;

void on_initialize(const string_t& address, BackendModel *model)
{

    uri_builder uri(address);
   

    auto addr = uri.to_uri().to_string();
    g_httpHandler = std::unique_ptr<handler>(new handler(addr,model));
    g_httpHandler->open().wait();

    ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;

    return;
}

void on_shutdown()
{
    g_httpHandler->close().wait();
    return;
}

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    utility::string_t port = U("34568");
    if (argc == 2)
    {
        port = argv[1];
    }

    utility::string_t address = U("http://127.0.0.1:");
    address.append(port);
    BackendModel model(IDLE, U(""), std::set<Colors>{RED, GREEN, YELLOW});
    on_initialize(address, &model);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        std::cout << model.candies_to_serve.size() << endl;
    }

    std::cout << "Press ENTER to exit." << std::endl;

    std::string line;
    std::getline(std::cin, line);

    on_shutdown();
    return 0;
}