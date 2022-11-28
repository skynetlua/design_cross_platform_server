#include <stdio.h>

struct Machine;
struct State
{
    void enter(Machine* machine)
    {
      	printf("enter State\n");
    }
    void update(Machine* machine)
    {
      	printf("update State\n");
    }
    void exit(Machine* machine)
    {
        printf("exit State\n");
    }
};
struct Machine
{
  	State* curState_;
  	Machine():curState_(0){}
  	~Machine()
  	{
      	if(curState_ != 0) curState_->exit(this);
      	curState_ = 0;
  	}
  	void enterState(State* state)
  	{
    	if(curState_ != 0) curState_->exit(this);
    	curState_ = state;
    	curState_ ->enter(this);
  	}
};
int main()
{
    State state1;
    State state2;
    Machine machine;
    machine.enterState(&state1);
    machine.enterState(&state2);
  return 0;
}