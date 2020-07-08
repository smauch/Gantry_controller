#include "BackendModel.h"

BackendModel::BackendModel()
{
}

BackendModel::BackendModel(Status curr_status, utility::string_t error_str)
{
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
	
	Colors available_candies[7];
	short i = 0;
	for (auto elem : this->available_candies)
	{
		available_candies[i] = elem;
		i++;
	}
	body_obj[U("candies")] = value(available_candies);
	return body_obj;
}

