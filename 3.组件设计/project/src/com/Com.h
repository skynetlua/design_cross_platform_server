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