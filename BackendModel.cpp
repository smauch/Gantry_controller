#include <BackendModel.h>
#include <Observer.h>

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

void BackendModel::attach(Observer* obs)
{
	views.push_back(obs);
}

void BackendModel::notify()
{
	for (int i = 0; i < views.size(); i++) {
		views[i]->update();
	}
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
	body_obj[U("processedCandies")] = web::json::value(this->processedCandies);

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

Status BackendModel::getReqStatus()
{
	return this->reqStatus;
}

std::queue<Colors> BackendModel::getCandiesToServe()
{
	return this->candiesToServe;
}

void BackendModel::setErr(std::string err)
{
	this->errorStr = err;
	this->notify();
}

void BackendModel::setAvailableCandies(std::set<Colors> availableCandies)
{
	this->availableCandies.clear();
	this->availableCandies = availableCandies;
}

void BackendModel::setReqStatus(Status reqStatus)
{
	this->reqStatus = reqStatus;
	this->notify();
}

void BackendModel::setCurrentStatus(Status currStatus)
{
	this->currStatus = currStatus;
}

void BackendModel::setReadyChangeState(bool readyChangeState)
{
	this->readyChangeState = readyChangeState;
	this->notify();
}

void BackendModel::setCandiesToServe(std::vector<Colors> candies)
{
	for (auto const& elem : candies) {
		this->candiesToServe.push(elem);
	}
	this->notify();
}

void BackendModel::setCandyServeDone()
{
	if (this->candiesToServe.size()) {
		this->candiesToServe.pop();
	}	
}



