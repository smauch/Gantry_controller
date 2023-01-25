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
    
    BackendModel *backend_model;
    void handle_post(web::http::http_request message);
    void handle_options(web::http::http_request message);
    void handle_error(pplx::task<void>& t);
    web::http::experimental::listener::http_listener m_listener;

    const utility::string_t REQ_STATE_EP = U("/gantry/reqState");
    const utility::string_t GET_STATUS_EP = U("/gantry/getStatus");
    const utility::string_t GET_CANDY_EP = U("/candy/getCandy");

    web::http::http_response req_state_endpoint(web::json::value body, web::http::http_response response);
    web::http::http_response get_status_endpoint(web::json::value body, web::http::http_response response);
    web::http::http_response get_candy_endpoint(web::json::value body, web::http::http_response response);

};

#endif 
// BACKENDSERVER_H