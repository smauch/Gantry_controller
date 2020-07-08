#include "BackendServer.h"
#include <chrono>
#include <thread>


handler::handler()
{
    //ctor
}
handler::handler(utility::string_t url) :m_listener(url)
{
    m_listener.support(methods::POST, std::bind(&handler::handle_post, this, std::placeholders::_1));
    m_listener.support(methods::OPTIONS, std::bind(&handler::handle_options, this, std::placeholders::_1));
}
handler::~handler()
{
    //dtor
}

void handler::handle_error(pplx::task<void>& t)
{
    try
    {
        t.get();
    }
    catch (...)
    {
        // Ignore the error, Log it if a logger is available
    }
}

void handler::handle_options(http_request message)
{
    http_response response(status_codes::OK);
    response.headers().add(U("Allow"), U("POST"));
    response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    response.headers().add(U("Access-Control-Allow-Headers"), U("*"));
    message.reply(response);
}


//
// A POST request
//
void handler::handle_post(http_request message)

{
    ucout << message.headers().content_type() << endl;
    auto path = http::uri::decode(message.relative_uri().path());


    try
    {
        auto json_content = message.extract_json().get();
        ucout << json_content << endl;
        std::cout << "Correct Body" << endl;
        std::cout << message.extract_utf8string().get() << endl;
    }
    catch (const std::exception&)
    {
        std::cout << "failed with that body" << endl;
        std::cout << message.extract_utf8string().get() << endl;
    }
    auto json_content = message.extract_json().get();
    ucout << json_content << endl;
   

    std::cout << "got here" << endl;
    string_t gantry_endpoint = U("/gantry");
    string_t candy_endpoint = U("/candy");

    if (path.find(gantry_endpoint) != std::string::npos) {
        string_t start_wait_endpoint = U("/startWait");
        string_t get_status_endpoint = U("/getStatus");
        string_t shutdown_endpoint = U("/shutdown");
        string_t maintenance_endpoint = U("/maintenance");

        if (path.find(start_wait_endpoint) != std::string::npos) {
            //Do Waitpat
            message.reply(status_codes::OK);
        }
        else if (path.find(get_status_endpoint) != std::string::npos) {
            //Json parse status
            
            std::cout << "got request" << endl;
            json::value rep = json::value::boolean(true);
            http_response response(status_codes::OK);
            response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
            response.set_body(backend_model.getCurrentStatusJSON());
            message.reply(response);

        }
        else if (path.find(shutdown_endpoint) != std::string::npos) {
            std::cout << "setShutdown!" << '\n';
            try
            {
                bool go_shutdown = json_content.at(U("state")).as_bool();
                if (go_shutdown) {
                    //Call Gantry shutdown
                    std::cout << "Shutting down" << endl;
                    //get status
                    json::value rep = json::value::string(U("statusdata"));
                    message.reply(status_codes::OK);
                }

            }
            catch (const std::exception&)
            {
                http_response response(status_codes::InternalError);
                response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
                message.reply(response);
            }
        }
        else if (path.find(maintenance_endpoint) != std::string::npos) {
            std::cout << "setMaintenance!" << '\n';
            try
            {
                bool go_maintenance = json_content.at(U("state")).as_bool();
                if (go_maintenance) {
                    //Call Gantry Maintenance
                    std::cout << "going into maintenance" << endl;
                }
                else {
                    //Reset go to Do Nothing
                    std::cout << "Resuming after maintenance" << endl;
                }
                //get status
                json::value rep = json::value::string(U("statusdata"));
                message.reply(status_codes::OK, rep);
            }
            catch (const std::exception&)
            {
                http_response response(status_codes::InternalError);
                response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
                message.reply(response);
            }
        }
        else
        {
            http_response response(status_codes::NotFound);
            response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
            message.reply(response);
        }

    }
    else if (path.find(candy_endpoint) != std::string::npos)
    {
        std::cout << "Candy endpoint" << endl;
        string_t get_one_endpoint = U("/getOne");

        if (path.find(get_one_endpoint)) {
            try
            {
                int candy_id = json_content.at(U("id")).as_integer();
                std::cout << "try to get candy" << candy_id << endl;

            }
            catch (const std::exception&)
            {
                http_response response(status_codes::InternalError);
                response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
                message.reply(response);
            }
            //Gantry get x
            std::this_thread::sleep_for(std::chrono::seconds(3));
            //Su´ccesful?
            if (true) {
                http_response response(status_codes::OK);
                response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
                message.reply(response);
            }
            else
            {
                http_response response(status_codes::NotFound);
                response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
                message.reply(response);
            }
           
        }
        std::cout << "candy!" << '\n';
        message.reply(status_codes::OK);
    }
    else {
        message.reply(status_codes::NotFound);
    }

    return;
};

