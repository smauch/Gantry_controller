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
    handler(utility::string_t url, BackendModel *model);
    virtual ~handler();

    pplx::task<void>open() { return m_listener.open(); }
    pplx::task<void>close() { return m_listener.close(); }


protected:

private:
    std::set<Colors> test = { GREEN, LIGHT_BLUE };
    BackendModel *backend_model;
    void handle_post(http_request message);
    void handle_options(http_request message);
    void handle_error(pplx::task<void>& t);
    http_listener m_listener;

    const string_t REQ_STATE_EP = U("/gantry/reqState");
    const string_t GET_STATUS_EP = U("/gantry/getStatus");
    const string_t GET_CANDY_EP = U("/candy/getCandy");

    http_response req_state_endpoint(json::value body, http_response response);
    http_response get_status_endpoint(json::value body, http_response response);
    http_response get_candy_endpoint(json::value body, http_response response);

};

#endif // BACKENDSERVER_H