#include <stdio.h>

struct Machine;
struct State
{
    virtual void enter(Machine* machine)
    {
        printf("enter State\n");
    }
    virtual void update(Machine* machine)
    {
        printf("update State\n");
    }
    virtual void exit(Machine* machine)
    {
        printf("exit State\n");
    }
};

enum EState
{
    EStateNone,
    EStateWaitMsg,
    EStateReceiveMsg,
    EStateQueryDB,
    EStateSendMsg,
};

struct Machine
{
    State* curState_;
    Machine();
    ~Machine();
    void update();
    void enterState(EState estate);
};

//等待消息状态
struct StateWaitMsg:public State
{
    int index_;
    virtual void enter(Machine* machine)
    {
        printf("enter StateWaitMsg\n");
        index_ = 0;
    }
    virtual void update(Machine* machine)
    {
        printf("update StateWaitMsg index_ = %d\n", index_);
        if (index_++ > 1)
        {
            machine->enterState(EStateReceiveMsg);
        }
    }
    virtual void exit(Machine* machine)
    {
        printf("exit StateWaitMsg\n");
    }
};
//接收消息状态
struct StateReceiveMsg:public State
{
    virtual void enter(Machine* machine)
    {
        printf("enter StateReceiveMsg\n");
    }
    virtual void update(Machine* machine)
    {
        printf("update StateReceiveMsg\n");
        machine->enterState(EStateQueryDB);
    }
    virtual void exit(Machine* machine)
    {
        printf("exit StateReceiveMsg\n");
    }
};
//查询数据库状态
struct StateQueryDB:public State
{
    int index_;
    virtual void enter(Machine* machine)
    {
        printf("enter StateQueryDB\n");
        index_ = 0;
    }
    virtual void update(Machine* machine)
    {
        printf("update StateQueryDB index_ = %d\n", index_);
        if (index_++ > 1)
        {
            machine->enterState(EStateSendMsg);
        }
    }
    virtual void exit(Machine* machine)
    {
        printf("exit StateQueryDB\n");
    }
};
//查询数据库状态
struct StateSendMsg:public State
{
    virtual void enter(Machine* machine)
    {
        printf("enter StateSendMsg\n");
    }
    virtual void update(Machine* machine)
    {
        printf("update StateSendMsg\n");
        machine->enterState(EStateWaitMsg);
    }
    virtual void exit(Machine* machine)
    {
        printf("exit StateSendMsg\n");
    }
};

Machine::Machine():curState_(0){}
Machine::~Machine()
{
    enterState(EStateNone);
}
void Machine::update()
{
    if(curState_ != 0)
        curState_->update(this);
}
void Machine::enterState(EState estate)
{
    if(curState_ != 0)
    {
        curState_->exit(this);
        delete curState_;
        curState_ = 0;
    }
    switch(estate)
    {
        case EStateWaitMsg:
            curState_ = new StateWaitMsg;
            break;
        case EStateReceiveMsg:
            curState_ = new StateReceiveMsg;
            break;
        case EStateQueryDB:
            curState_ = new StateQueryDB;
            break;
        case EStateSendMsg:
            curState_ = new StateSendMsg;
            break;
        default:
            return;
    }
    curState_ ->enter(this);
}


int main()
{
    Machine machine;
    machine.enterState(EStateWaitMsg);
    int count = 9;
    while(count--)
    {
        machine.update();
    }
  return 0;
}