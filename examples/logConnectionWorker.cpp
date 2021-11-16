#include "Crazyflie.h"


// bool logCallback(const std::map<TocItem,void*>& tocItemsAndValues, uint32_t period)
// {
//     if(id == 0)
//     {
//         std::cout << "Period: " << period << " - ";
//         for(uint8_t byte : data)
//         {
//             std::cout << (int) byte<<" ";
//         }
//         std::cout << std::endl;
//     }
//     return true;
// }

int main()
{
    Crazyflie cf("usb://0");
    cf.init();
    // cf.addLogCallback(logCallback);
    int i = 0;
    std::cin >> i;
    return 0;
}