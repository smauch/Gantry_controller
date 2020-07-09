#include "BackendServer.h"
#include <chrono>
#include <thread>


handler::handler()
{
    //ctor
}
handler::handler(utility::string_t url, BackendModel* model) :m_listener(url)
{
    this->backend_model = model;
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

//
// Options handling for Preflight requests
//
void handler::handle_options(http_request message)
{
    http_response response(status_codes::OK);
    response.headers().add(U("Allow"), U("POST"));
    response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    response.headers().add(U("Access-Control-Allow-Headers"), U("*"));
    message.reply(response);
}


//
// Endpoint functions
//

http_response handler::req_state_endpoint(json::value body, http_response response)
{
    if (body.has_integer_field(U("state"))) {
        int req_state = body.at(U("state")).as_integer();
        set <Status> allowed_req_states = { SHUTDOWN,MAINTENANCE,WAIT_PAT };
        //Is current State == Req State?
        //TODO - Test Cast, what happens if number is larger than Enum
        const bool is_allowed = allowed_req_states.find(Status(req_state)) != allowed_req_states.end();
        if (is_allowed) {
            this->backend_model->req_status = Status(req_state);
            
            response.set_status_code(status_codes::OK);
        }
        else {
            response.set_status_code(status_codes::Forbidden);
        }
    }
    else {
        response.set_status_code(status_codes::BadRequest);
    }
    return response;
}

http_response handler::get_status_endpoint(json::value body, http_response response)
{
    response.set_status_code(status_codes::OK);
    response.set_body(this->backend_model->getCurrentStatusJSON());
   
    return response;
}

http_response handler::get_candy_endpoint(json::value body, http_response response)
{
    //What happens for unavailable or wrong ID
    if (body.has_array_field(U("id"))) {
        json::array candy_ids = body.at(U("id")).as_array();
        for (auto const& id : candy_ids) {
            if (id.is_integer()) {
                int color_id = id.as_integer();
                const bool is_available = this->backend_model->available_candies.find(Colors(color_id)) != this->backend_model->available_candies.end();
                if (is_available) {
                    this->backend_model->candies_to_serve.push(Colors(color_id));
                }
                else {
                    //Response Unavailables
                }
            }
        }
    }
    else if (body.has_integer_field(U("id"))) {
        int color_id = body.at(U("id")).as_integer();
        const bool is_available = this->backend_model->available_candies.find(Colors(color_id)) != this->backend_model->available_candies.end();
        if (is_available) {
            this->backend_model->candies_to_serve.push(Colors(color_id));
        }
        else {
            response.set_status_code(status_codes::NotFound);
            return response;
        }
    }
    else {
        response.set_status_code(status_codes::BadRequest);
        return response;
    }
    // TODO: Handle this with wait_until conditional variable and mutex to safe access 
    std::this_thread::sleep_for(5000ms);
    if (!backend_model->candies_to_serve.size()) {
        response.set_status_code(status_codes::OK);
        std::cout << "sucessfull" << std::endl;
    }
    else
    {
        // TODO: Err should contain if Candy is not found
        response.set_status_code(status_codes::NotFound);
        std::cout << "timeout" << std::endl;
    }
    return response;
}


//
// A POST request
//
void handler::handle_post(http_request message)
{
    json::value json_content;
    string_t path = http::uri::decode(message.relative_uri().path());
    try
    {
         json_content = message.extract_json().get();
    }
    catch (const std::exception&)
    {
        return;
    }

    http_response base_response;
    base_response.headers().add(U("Access-Control-Allow-Origin"), U("*"));

    http_response response;
    if (path.find(REQ_STATE_EP) != std::string::npos) {
        response = req_state_endpoint(json_content, base_response);
    }
    else if (path.find(GET_STATUS_EP) != std::string::npos) {
        response = get_status_endpoint(json_content, base_response);
    }
    else if (path.find(GET_CANDY_EP) != std::string::npos) {
        response = get_candy_endpoint(json_content, base_response);
    }
    else
    {
        response = base_response;
        response.set_status_code(status_codes::NotFound);
    }
    //Reply with generated response
    message.reply(response); 
};