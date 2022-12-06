#include <stdio.h>
#include <com/db/ComDB.h>
#include <com/state/ComState.h>
#include <com/network/ComNetwork.h>

struct StateWaitMsg:public State
{
    int index_;
    virtual void enter(ComState* machine);
    virtual void update(ComState* machine);
    virtual void exit(ComState* machine);
};
//Receive msg state
struct StateReceiveMsg:public State
{
    virtual void enter(ComState* machine);
    virtual void update(ComState* machine);
    virtual void exit(ComState* machine);
};
//Query DB state
struct StateQueryDB:public State
{
    int index_;
    virtual void enter(ComState* machine);
    virtual void update(ComState* machine);
    virtual void exit(ComState* machine);
};
//查询数据库状态
struct StateSendMsg:public State
{
    virtual void enter(ComState* machine);
    virtual void update(ComState* machine);
    virtual void exit(ComState* machine);
};

//机器人
struct Robot
{
    ComDB* comDB_;
    ComState* comState_;
    ComNetwork* comNetwork_;
    Robot()
    {
        comDB_ = new ComDB;
        comDB_->robot_ = this;
        comState_ = new ComState;
        comState_->robot_ = this;
        comNetwork_ = new ComNetwork;
        comNetwork_->robot_ = this;
    }
    ~Robot()
    {
        delete comDB_;
        delete comState_;
        delete comNetwork_;
    }
    void start()
    {
        comDB_->start();
        comNetwork_->start();
        
        comState_->enterState(new StateWaitMsg);
        comState_->start();
    }
    void update()
    {
        int count = 9;
        while(count--)
        {
            comState_->update();
        }
    }
    void stop()
    {
        comState_->stop();
        comDB_->stop();
        comNetwork_->stop();
    }
};

//StateWaitMsg
void StateWaitMsg::enter(ComState* machine)
{
    printf("enter StateWaitMsg\n");
    index_ = 0;
}
void StateWaitMsg::update(ComState* machine)
{
    printf("update StateWaitMsg index_ = %d\n", index_);
    if (index_++ > 1)
    {
        machine->enterState(new StateReceiveMsg);
    }
}
void StateWaitMsg::exit(ComState* machine)
{
    printf("exit StateWaitMsg\n");
}
//StateReceiveMsg
void StateReceiveMsg::enter(ComState* machine)
{
    //调用网络组件的方法
    machine->robot_->comNetwork_->receive();
}
void StateReceiveMsg::update(ComState* machine)
{
    printf("update StateReceiveMsg\n");
    machine->enterState(new StateQueryDB);
}
void StateReceiveMsg::exit(ComState* machine)
{
    printf("exit StateReceiveMsg\n");
}
//StateQueryDB
void StateQueryDB::enter(ComState* machine)
{
    //DB component method
    machine->robot_->comDB_->query();
    index_ = 0;
}
void StateQueryDB::update(ComState* machine)
{
    printf("update StateQueryDB index_ = %d\n", index_);
    if (index_++ > 1)
    {
        machine->enterState(new StateSendMsg);
    }
}
void StateQueryDB::exit(ComState* machine)
{
    printf("exit StateQueryDB\n");
}
//StateSendMsg
void StateSendMsg::enter(ComState* machine)
{
    machine->robot_->comNetwork_->send();
}
void StateSendMsg::update(ComState* machine)
{
    printf("update StateSendMsg\n");
    machine->enterState(new StateWaitMsg);
}
void StateSendMsg::exit(ComState* machine)
{
    printf("exit StateSendMsg\n");
}
