#pragma once
#include <BackendModel.h>
class Observer
{

public:
    Observer(BackendModel* mod) {
        model = mod;
        model->attach(this);
    }
    virtual void update() = 0;
protected:
    BackendModel* getModel() {
        return model;
    }
private:
    BackendModel* model;
};

