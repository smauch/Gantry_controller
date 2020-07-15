#pragma once
#include <Status.h>
#include <queue>
#include <vector>
#include <set>
#include <string>
#include <Color.h>
#include "cpprest/json.h"


class BackendModel
{

std::vector <class Observer*> views;

public:
    BackendModel();
    BackendModel(Status curr_status, std::string error_str);

    // For subscribers
    void attach(Observer* obs);
    void notify();

    // Getter
    web::json::value getCurrentStatusJSON();
    Status getStatus();
    std::set<Colors> getAvailableCandies();
    std::string getErr();
    bool getReadyChangeState();
    Status getReqStatus();
    std::queue<Colors> getCandiesToServe();

    // Setter
    void setErr(std::string err);
    void setAvailableCandies(std::set<Colors> availableCandies);
    void setReqStatus(Status reqStatus);
    void setCurrentStatus(Status currStatus);
    void setReadyChangeState(bool readyChangeState);
    void setCandiesToServe(std::vector<Colors> candies);
    void setCandyServeDone();
    // Vars
    // Vars should be accessed with getter setter
    std::queue<Colors> candiesToServe;
    //std::vector<std::array<uunit, Gantry::NUM_AMP>> candyBuffer;

private:
    bool readyChangeState;

    Status currStatus;
    Status reqStatus;
    std::set<Colors> availableCandies;

    std::queue<Colors> candiesToServe;

    std::string errorStr;
    int uptime;
    int processedCandies;
};
