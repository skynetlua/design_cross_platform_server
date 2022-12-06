github源码:https://github.com/skynetlua/design_cross_platform_server
我们通过CMake建立跨平台工程，以Linux做标准库，让我们的程序可以移植在任何操作系统上。
**我们也希望写的代码可以轻松地移植在任意一个项目中，甚至发到网上，轻松导入即可使用。**
**我们也希望可视化操作，人脑更容易接受的方式去开发程序。**
这就是组件设计。我们设计组件的核心目标是可视化操作，通过可视化方式组装我们的业务。
### 一、组件模块化机器人
对于数据机器人Robot，把它的功能拆分成各种组件Com，我们需要什么样的数据机器人，我们就组装这些组件进行定制。
数据机器人代码设计如下：
```cpp
struct Com
{
  int id_; //组件id
  int name_; //组件名称
  void start(){} //组件启动
  void stop(){} //组件停止
};
struct Robot
{
  Com com1_;
  Com com2_;
  void start()
  {
    com1_.start();
    com2_.start();
  }
  void stop()
  {
    com1_.stop();
    com2_.stop();
  }
};
int main()
{
  Robot robot;
  robot.start();
}
```
### 二、机器人组件化实现
数据机器人需要接收网络消息和发送网络消息，读取数据库数据。
可设计组件如下：
1. 网络组件（ComNetwork）:负责接收和发送网络消息；
2. 数据库组件（ComDB）:负责请求数据库数据；
3. 状态机组件（ComState）:状态机组件，我们把上一节的状态机也设计成组件。
创建./src/simple.cpp源文件，代码如下：
```cpp
#include <stdio.h>

struct Robot;
//Com
struct Com
{
    int id_; //组件id
    Robot* robot_;
    virtual ~Com() {}
    virtual void start(){} //组件启动
    virtual void stop(){} //组件停止
};

//ComDB////////////////////
struct ComDB:public Com
{
    ComDB():Com(){}
    virtual ~ComDB(){}
    void query()
    {
        printf("ComDB query data\n");
    }
};
//ComNetwork////////////////////
struct ComNetwork :public Com
{
    ComNetwork() :Com() {}
    virtual ~ComNetwork() {}
    void send()
    {
        printf("ComNetwork send\n");
    }
    void receive()
    {
        printf("ComNetwork receive\n");
    }
};
//ComState/////////////////
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
        enterState(0);
    }
    virtual void start(){}
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

struct StateWaitMsg:public State
{
    int index_;
    virtual void enter(ComState* machine);
    virtual void update(ComState* machine);
    virtual void exit(ComState* machine);
};
//接收消息状态
struct StateReceiveMsg:public State
{
    virtual void enter(ComState* machine);
    virtual void update(ComState* machine);
    virtual void exit(ComState* machine);
};
//查询数据库状态
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

int main()
{
    Robot robot;
    robot.start();
    robot.update();
    getchar();
    return 0;
}

```
在CMakeFile.txt文件加上add_executable(simple src/simple.cpp)。
在./build目录执行cmake..,在windows系统，cmake可以生成VS工程，在Mac可以生产XCode工程，在Linux可生产makefile文件。
实现组件化设计后，我们可以用组件来管理模块，虽然组装很麻烦，但是如果我们把它设计成配置表加载，再实现可视化编辑，那就是很美好的事情。
**组件设计的目标是实现少编码，配置表控制，可视化编辑，共享组件。**

### 三、组件化管理项目
我们的项目会有各种各样的组件，一个文件完全放不下的，我们需要制定一套标准文件结构去管理。
把上面的代码用一个源文件实现，非常不方便写代码和维护，我们用组件的方式进行拆分。

新建目录./src/com，专门存放组件。一个组件一个文件夹。
```bash
./src/com
./src/com/network
./src/com/db
./src/com/state
```

在./src/com文件夹，先创建我们的基类Com.h。

```cpp
#pragma once
#include <stdio.h>
struct Robot;
//Component base
struct Com
{
    int id_; //组件id
    Robot* robot_;
    virtual ~Com() {}
    virtual void start(){} //组件启动
    virtual void stop(){} //组件停止
};
```
我们把组件的基类放到./src/com文件夹，全部组件都继承它。

在./src/com/network文件夹，新建头文件ComNetwork.h

```cpp
#pragma once
#include <com/Com.h>
//network component
struct ComNetwork :public Com
{
    ComNetwork() :Com() {}
    virtual ~ComNetwork() {}
    void send()
    {
        printf("ComNetwork send\n");
    }
    void receive()
    {
        printf("ComNetwork receive\n");
    }
};
```
因为我们在CMakeLists.txt脚本include_directories(./src)导入了./src。cmake生成的工程文件，会对编译器添加-I./src选项。所以，我们可以`#include <com/Com.h>`进行导入。

我们推荐使用`#include <xxx.h>`，而不推荐使用`#include "xxx.h"`，使用尖括号导入头文件，可以让我们更了解头文件的具体的路径。
我们之所以用#include <com/Com.h>，是因为在CMakeList.txt中包含./src目录，即include_directories(./src)。编译器会查找./src目录，自动拼接就可以找到源文件./src/com/Com.h。

在./src/com/db文件夹，新建头文件ComDB.h
```cpp
#pragma once
#include <com/Com.h>
//DB query component
struct ComDB:public Com
{
    ComDB():Com(){}
    virtual ~ComDB(){}
    void query()
    {
        printf("ComDB query data\n");
    }
};
```

在./src/com/state文件夹，新建头文件ComState.h
```cpp
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

```
至此，我们创建完全部组件。接下来，我们需要创建机器人对象。把建好的组件组装到机器人上，最后设计它的状态，完全我们数据加工机器人的构建。

在./src文件夹，新建头文件Robot.h
```cpp
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

```

最后在./src文件夹，新建源文件文件main.cpp
```cpp
#include <Robot.h>
int main()
{
    Robot robot;
    robot.start();
    robot.update();
    getchar();
    return 0;
}
```
CMake的文件如下
```
CMAKE_MINIMUM_REQUIRED(VERSION 3.12.1)
PROJECT(server)

add_executable(simple src/simple.cpp)

include_directories(src)
#自动查找源文件
file(GLOB_RECURSE COM_LIST 
    src/com/*.cpp
    src/com/*.cc
    src/com/*.h
    src/com/*.hpp
    src/com/*.c
)
#执行程序和源代码
add_executable(demo src/main.cpp src/Robot.h  ${COM_LIST})
```
执行cmake构建工程，再选择对应的IDE进行编译调试即可。

### 四、组件配置表加载
功能实现组件化后，我们可以通过配置表来选择控制机器人加载组件，这样我们可以避免大量的写代码工作。具体详情请见往后章节。

### 五、git子模块管理组件
我们可以在工程根目录创建com文件夹，作为外部引用组件，这些外部组件可以当成一个git子模块。然后通过CMake来控制添加子模块，大大方便我们代码管理和共享交流。

我们实现组件化设计以后，接下来，我们将一一实现框架所需要的组件。
