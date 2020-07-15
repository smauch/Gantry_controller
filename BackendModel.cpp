#include <BackendModel.h>

BackendModel::BackendModel()
{
	readyChangeState = false;
}

BackendModel::BackendModel(Status curr_status, std::string error_str)
{
	this->currStatus = curr_status;
	this->errorStr = error_str;
	readyChangeState = false;
}

web::json::value BackendModel::getCurrentStatusJSON()
{
	web::json::value body_obj = web::json::value::object();
	body_obj[U("status")] = web::json::value(this->currStatus);
	std::wstring wErrStr = std::wstring(this->errorStr.begin(), this->errorStr.end());
	const wchar_t* widecstr = wErrStr.c_str();
	body_obj[U("errorMsg")] = web::json::value(widecstr);
	body_obj[U("currentJobs")] = web::json::value(this->candiesToServe.size());
	body_obj[U("uptime")] = web::json::value(this->uptime);
	body_obj[U("processedCandies")] = web::json::value(this->processed_candies);

	web::json::value available_candies = web::json::value::array();
	short i = 0;
	for (auto elem : this->availableCandies)
	{
		available_candies[i] = elem;
		i++;
	}
	body_obj[U("availableCandies")] = web::json::value(available_candies);
	return body_obj;
}

Status BackendModel::getStatus()
{
	return this->currStatus;
}

std::set<Colors> BackendModel::getAvailableCandies()
{
	return this->availableCandies;
}

std::string BackendModel::getErr()
{
	return this->errorStr;
}

bool BackendModel::getReadyChangeState()
{
	return this->readyChangeState;
}

void BackendModel::setErr(std::string err)
{
	this->errorStr = err;
}

void BackendModel::setAvailableCandies(std::set<Colors> availableCandies)
{
	this->availableCandies.clear();
	this->availableCandies = availableCandies;
}

void BackendModel::setReqStatus(Status reqStatus)
{
	if (currStatus != SHUTDOWN) {
		this->reqStatus.push(reqStatus);
	}
}

void BackendModel::setJobDone()
{
	if (reqStatus.front() == currStatus) {
		reqStatus.pop();
		readyChangeState = true;
	}
	else {
		// Error in state handling
		//TODO Log?
	}
	
}

void BackendModel::setJobRunning()
{

}



