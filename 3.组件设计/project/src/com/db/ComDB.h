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