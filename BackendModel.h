// BackendModel.h
#ifndef BACKENDMODEL_H // include guard
#define BACKENDMODEL_H
#include <Status.h>
#include <queue>
#include <set>
#include <string>
#include <Color.h>
#include "cpprest/json.h"


class BackendModel
{
public:
    BackendModel();
    BackendModel(Status curr_status, std::string error_str);

    // Getter
    web::json::value getCurrentStatusJSON();
    Status getStatus();
    std::set<Colors> getAvailableCandies();
    std::string getErr();
    bool getReadyChangeState();

    // Setter
    void setErr(std::string err);
    void setAvailableCandies(std::set<Colors> availableCandies);
    void setReqStatus(Status reqStatus);
    void setReadyChangeState(bool block);
    // Vars
    std::queue<Colors> candiesToServe;


private:
    bool readyChangeState;
    Status currStatus;
    Status reqStatus;
    std::set<Colors> availableCandies;
    std::string errorStr;
    int uptime;
    int processedCandies;
};
#endif /* BACKENDMODEL_H */
