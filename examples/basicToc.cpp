#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <queue>
#include <vector>

#include "Crazyflie.h"

using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    Crazyflie crazyflie("usb://0");

    crazyflie.init();

    int i, value, newValueSize = 0;
    std::string group = "";
    std::string name = "";

    std::cout << "crazyflie connected"
              << "\n\n1 - print TOC parameters.\n2 - Save TOC parameteres to .csv file\n3 - Change parameter value\n\nenter your option: " << std::endl;
    std::cin >> i;

    while (i != 0)
    {
        if (i == 1)
        {
            crazyflie.printParamToc();
        }
        if (i == 2)
        {
            std::string path = "/home/makeruser/crazyflie-link-cpp-expanded";
            std::string fileName = "toc.csv";
            crazyflie.csvParamToc(path, fileName);
        }
        if (i == 3)
        {
            std::cout << "Enter group: " << std::endl;
            std::cin >> group;
            std::cout << "Enter name: " << std::endl;
            std::cin >> name;
            std::cout << "Enter value: " << std::endl;
            std::cin >> value;
            std::cout << "Enter value Size: " << std::endl;
            std::cin >> newValueSize;

            std::cout << "Initial value: " << crazyflie.getUIntByName(group, name) << std::endl;
            crazyflie.setParamByName(group, name, value);
            std::cout << "New value: " << crazyflie.getUIntByName(group, name) << std::endl;
        }

        std::cout << "\n\n1 - print TOC parameters.\n2 - Save TOC parameteres to .csv file\n3 - Change parameter value by name and group.\n\nenter your option: " << std::endl;
        std::cin >> i;
    }
    
    return 0;
}