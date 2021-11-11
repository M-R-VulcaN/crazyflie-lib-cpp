#include <iostream>
#include "ConnectionWrapper.h"
int main()
{
    std::cout << isBigEndian() <<std::endl;
    int res = system("echo \"from usb.core import find as finddev; dev=finddev(idVendor=0x0483,idProduct=0x5740); dev.reset()\" | python3");
    return res;
}
