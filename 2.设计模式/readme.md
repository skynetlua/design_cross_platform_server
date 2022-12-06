**交流QQ群：963944097**

**技术框架实质就是建立一种标准的工作流，让更多的人参与，更低成本地实现目标。**
**目前我们所规定的标准：**
1. 使用CMake管理项目，以Linux开源系统为标准，如果在windows上缺少的库，就模仿Linux造一个，实现一份代码跑全部平台。
2. 采用utf8字符编码，调用win32接口，需要把unicode转utf8。
3. 采用骆驼峰形式代码风格
```cpp
class DemoClass //类名首字母大写，骆驼峰
{
    int parramArg_; //变量名末尾有_，非静态变量首字母小写
    static int ParramArg_;//变量名末尾有_，静态变量首字母大写
  static void FuncA(){}//函数名末尾无有_，静态函数首字母小写
    void funcA(){}//函数名末尾无有_，非静态函数首字母小写
};
```
4. 设计模式众多，选择人脑比较容易接受的设计模式：面向对象、状态机和组件设计等。模块用组件实现，可达到组件通用化，避免重复造轮子。

## 一、面向对象设计
编程设计多种多样，计算机只是人脑的工具，作为人脑的工具，那么这个工具要更容易被人脑使用，而不是人脑无法驾驭，否则，这个工具就会被抛弃。我们希望做出的工具，有更多的人愿意去使用，这样我们才有机会拿来卖钱买饭吃。

面向对象设计，把业务逻辑封装类似实物世界，以一种人脑熟悉的方式，让人脑更容易接受和使用。制作工具一定要符合用户习惯。

我们设计的服务器框架，服务器主要功能是加工数据、提供数据和数据通讯，就像一座数据工厂，里面有各种各样的数据加工机器人。我们的第一个对象就是机器人，机器人有最简单的两点：属性和行为。

我们用变量记录和描述属性，用函数描述行为。
```cpp
机器人
  属性：编号，名称，类型
    行为：
   行走（）
   ｛
      //执行行走
   ｝
   工作（）
   ｛
      // 执行工作任务
   ｝
```
如果有资金支持，我们可以开发一个汉语编译器工具，把上述内容编译成二进制程序，就可以在电脑上执行。很可惜，排得上号的编译器（gcc、cl、llvm、ndk等）都不是国产的。
从上面可以看出，面向对象只是一种逻辑，跟编程语言无关，我们可以用C语言和C++语言各实现一个robot对象。
C语言版本：
```c
struct Robot
{
  int id;
  const char* name;
    int type;
}
void Robot_move(Robot* this)
{
  printf("move:id = %d\n", this->id);
}
void Robot_work(Robot* this)
{
  printf("work:id = %d\n", this->id);
}
int main()
{
  Robot robot;
  Robot_move(&robot);
  Robot_work(&robot);
  return 0;
}
```
C++版本：

```cpp
struct Robot
{
  int id_;
    std::string name_;
    int type_;
    void move()
   {
      std::cout << "move: id = " << id_ << std::endl;
    }
   void work()
   {
      std::cout << "work: id = " << id_ << std::endl;
    }
}
int main()
{
  Robot robot;
  robot.move();
  robot.work();
}
```
通过两个版本的分析，可以看出C++编译器对面向对象提供了更好的语法便利。这就是C++语法糖，代价就是损失一些性能，好处就是提高写代码效率，代码更少，更容易维护。

## 二、状态机设计
计算机可以算是一种状态机，通过输入设备输入操作，就会改变状态。
**比如HTTP服务器的状态：**
1.监听到客户端连接，执行accept，建立链接状态；
2.解析HTTP头状态；
3.解析Cookie，处理session状态
4.验证modify和etag缓存状态
5.路由到具体url业务状态
6.获取到数据进行文件渲染状态
7.向客户端发送数据状态
8.最后关闭连接状态
上述把HTTP服务器复杂的请求过程，细分成各种具体的状态，大幅降低了问题的复杂度，转换人脑更加容易熟悉的状态。程序发生bug，可以快速定位；对性能调优，监测每个状态的消耗时间，带来极大便利性。
**1. 简单状态机**
我们对数据机器人建立状态机。两个对象Machine和State。Machine管理和控制状态State，一个State状态有三个方法，进入状态，状态刷新，退出状态。

在./src目录创建源文件simplemachine.cpp
```cpp
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
```
CmakeLists.txt文件为
```powershell
CMAKE_MINIMUM_REQUIRED(VERSION 3.12.1)
PROJECT(server)
add_executable(simplemachine src/simplemachine.cpp)
```
最后在./build目录执行:

```powershell
cmake ../
make
simplemachine
```
具体操作，请看工程构建章节。

**2. 状态机简单例子**
robot工作状态：接收消息状态，查询数据库状态，返回消息状态，等待消息状态。
1. robot一开始，处在等待状态：EStateWaitMsg
2. 有消息来，进入到接收消息状态：EStateWaitMsg=>EStateReceiveMsg
3. 获取数据库数据，进入查询数据库状态：EStateReceiveMsg=>EStateQueryDB
4. 拿到数据后，进入发送消息状态：EStateQueryDB=>EStateSendMsg
5. 消息发送完毕，又回到等待状态：EStateSendMsg=>EStateWaitMsg
把复杂的逻辑，划分成多种状态，简化问题，让人脑更容易理解和接受。
新建src/demo.cpp源文件，实现代码如下：

```cpp
//等待消息状态
struct Machine;
struct State
{
    virtual void enter(Machine* machine);
    virtual void update(Machine* machine);
    virtual void exit(Machine* machine);
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
```

在CMakeFile.txt文件加上add_executable(demo src/demo.cpp)。
在./build目录执行cmake..，可以用IDE打开工程，执行编译运行。


## 三、组件设计
我们的机器人需要接收消息，接收到消息以后，需要进行加工，然后把加工的数据，通过消息发出去。
我们有成千上万种数据加工业务，我们需要设计各种各样的机器人。
如果按照继承的方式，robot作为父类，需要实现各种各样的子类，这种方式虽然也可以做到业务分离，但是复用和共享很困难。
我们希望像汽车一样，标准化各种零部件，想要什么价位和性能的汽车，就选择组装什么样的零件。这个就是组件设计。
设计一个类Com，作为组件基类，把单一功能的逻辑，作为一个组件。Robot作为集合类，需要什么样的Robot，就组装什么样的组件。

由于篇幅，详细请见下一节《组件设计——从零设计跨平台C/C++服务器框架》。
