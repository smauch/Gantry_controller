#pragma once
#include "Status.h"
#include <vector>
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
    BackendModel(Status curr_status, utility::string_t error_str);
    value getCurrentStatusJSON();
    Status curr_status;
    std::vector<Colors> candies_to_serve;
    std::set<Colors> available_candies;
    utility::string_t error_str;
    int uptime;
    int processed_candies;
    

private:

};

