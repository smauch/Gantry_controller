#include "BackendModel.h"

BackendModel::BackendModel()
{
}

BackendModel::BackendModel(Status curr_status, utility::string_t error_str, std::set<Colors> available_candies)
{
	this->available_candies = available_candies;
	this->curr_status = curr_status;
	this->error_str = error_str;
}

value BackendModel::getCurrentStatusJSON()
{
	value body_obj = json::value::object();
	body_obj[U("status")] = value(this->curr_status);
	body_obj[U("errorMsg")] = value(this->error_str);
	body_obj[U("currentJobs")] = value(this->candies_to_serve.size());
	body_obj[U("uptime")] = value(this->uptime);
	body_obj[U("processedCandies")] = value(this->processed_candies);
	
	json::value available_candies = json::value::array();
	short i = 0;
	for (auto elem : this->available_candies)
	{
		available_candies[i] = elem;
		i++;
	}
	body_obj[U("availableCandies")] = value(available_candies);
	return body_obj;
}

