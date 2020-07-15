#pragma once
#include "State.h"
class MaintenanceState :
    protected State
{
private:
    void doJob();
};

