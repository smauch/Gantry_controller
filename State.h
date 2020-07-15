#pragma once
#include "Observer.h"
class State : public Observer
{
public:
    State(BackendModel* mod, Status status) : Observer(mod) {
        this->status = status;
    }
    Status status;
    virtual void execute() {
        getModel()->setReadyChangeState(false);
        getModel()->setCurrentStatus(this->status);
        std::cout << "Current Status" << getModel()->getStatus();
        this->doJob();
        getModel()->setReqStatus(IDLE);
        std::cout << "Current Status" << getModel()->getStatus();
        getModel()->setReadyChangeState(true);
    }
    virtual void doJob() = 0;

    void update() {
        if (getModel()->getReadyChangeState() && (getModel()->getReqStatus() == this->status)) {
            this->execute();
        }
    }
};

