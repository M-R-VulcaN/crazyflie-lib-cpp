#include "ConnectionWorker.h"
#include "Crazyflie.h"
#include <chrono>
#include <iostream>
using bitcraze::crazyflieLinkCpp::Connection;
using bitcraze::crazyflieLinkCpp::Packet;

bool func(const Packet &p_recv)
{

    if (p_recv)
        std::cout << "res1: " << p_recv.payloadSize() << std::endl;
  
    std::cout << "failed recv1" << std::endl;
    return true;
}

bool func2(Packet p_recv)
{
    if (p_recv)
        std::cout << "res2: " << p_recv << std::endl;
    else
        std::cout << "failed recv2" << std::endl;
    return true;
}

bool func3(const char *str)
{
 
    std::cout << "###" << str;
    return false;
}

bool func4(const char *str)
{

    std::cout << str;
    return true;
}

int main()
{
    Crazyflie cf("usb://0");
    cf.init();
    cf.printLogToc();
    cf.printParamToc();
   
    return 0;
}