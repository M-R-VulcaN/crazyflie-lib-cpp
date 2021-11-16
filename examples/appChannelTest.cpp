#include "Crazyflie.h"

using namespace bitcraze::crazyflieLinkCpp;
bool appChannelCallback(const uint8_t *data, uint8_t dataLen)
{
    for (int i = 0; i < dataLen; i++)
    {
        std::cout << (int)data[i] << " ";
    }
    std::cout << std::endl;
    return true;
}
int main()
{
    Crazyflie crazyflie("usb://0");

    crazyflie.init();

    crazyflie.setParamByName("usd", "logging", 0);
    crazyflie.setParamByName("usd", "sendAppChannle", 1);

    
    crazyflie.addAppChannelCallback(appChannelCallback);
    int i = 0;
    std::cin >> i;


    return 0;
}