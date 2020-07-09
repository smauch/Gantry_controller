#pragma once
#include "Status.h"
#include <queue>
#include <set>
#include <string>
#include "Color.h"
#include "cpprest/json.h"

using namespace web;
using namespace json;

class BackendModel
{
public:
    BackendModel();
    BackendModel(Status curr_status, utility::string_t error_str, std::set<Colors> available_candies);
    value getCurrentStatusJSON();

    Status curr_status;
    Status req_status;
    std::queue<Colors> candies_to_serve;
    std::set<Colors> available_candies;
    utility::string_t error_str;
    int uptime;
    int processed_candies;
    

private:

};

