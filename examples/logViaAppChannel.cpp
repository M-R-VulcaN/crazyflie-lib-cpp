#include "LoggingCrazyflieWrapper.h"
#include "utilsPlus.hpp"

#define ACK_DELAY_MICRO_SEC 30

using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    resetUSB();

    Crazyflie crazyflie("usb://0");
    // LoggingCrazyflieWrapper logging(crazyflie);
    // std::cout <<"pass"<<std::endl;

    crazyflie.init();

    LoggingCrazyflieWrapper loggingWrapper(crazyflie, "out.txt");
    loggingWrapper.start(true);
    return 0;
}
