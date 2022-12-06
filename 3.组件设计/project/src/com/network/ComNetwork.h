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