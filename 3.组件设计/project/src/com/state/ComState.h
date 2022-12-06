#pragma once
#include <com/Com.h>

struct ComState;
struct State
{
    virtual void enter(ComState* machine){}
    virtual void update(ComState* machine){}
    virtual void exit(ComState* machine){}
};
struct ComState:public Com
{
    State* curState_;
    ComState():curState_(0) {}
    virtual ~ComState()
    {
        enterState(NULL);
    }
    virtual void start()
    {
    }
    void update()
    {
        if (curState_ != 0) curState_->update(this);
    }
    void enterState(State* state)
    {
        if (curState_ != 0)
        {
            curState_->exit(this);
            delete curState_;
            curState_ = 0;
        }
        if (state)
        {
            curState_ = state;
            curState_->enter(this);
        }
    }
};
