#pragma once
#include <BackendModel.h>
class State {
public:
    State(BackendModel* mod, Status status) {
        model = mod;
        model->attach(this);
        this->status = status;
    }

    void update() {
        if (!getModel()->getReqStatus().empty()) {
            if (getModel()->getReadyChangeState() && (getModel()->getReqStatus().front() == this->status)) {
                this->execute();
            }
        }
    }

    Status getStatus() {
        return status;
    }


protected:
    BackendModel* getModel() {
        return model;
    }
    virtual void doJob() = 0;

private:
    BackendModel* model;
    Status status;
    virtual void execute() {
        getModel()->setJobRunning(this->status);
        std::cout << "New Status" << getModel()->getStatus() << std::endl;;
        this->doJob();
        getModel()->setJobDone();
    }
};

