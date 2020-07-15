#pragma once
#include "State.h"
class ServeState :
    protected State
{
    using State::State;
public:

protected:

private:
    void doJob();
    Colors reqCandy;
    int failedTrys;
};

