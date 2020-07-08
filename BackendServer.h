#ifndef BACKENDSERVER_H
#define BACKENDSERVER_H
#include <iostream>
#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"
#include "cpprest/json.h"
#include "BackendModel.h"
#include <vector>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;


class handler
{
public:
    handler();
    handler(utility::string_t url);
    virtual ~handler();

    pplx::task<void>open() { return m_listener.open(); }
    pplx::task<void>close() { return m_listener.close(); }


protected:

private:
    BackendModel backend_model = BackendModel(IDLE, U(""));
    void handle_post(http_request message);
    void handle_options(http_request message);
    void handle_error(pplx::task<void>& t);
    http_listener m_listener;
   
};

#endif // BACKENDSERVER_H