#include "Crazyflie.h"

// typedef std::function<bool(const uint8_t *, uint8_t)> AppChannelCallback;
// typedef std::function<bool(uint16_t, const ParamValue &)> ParamValueCallback;
// typedef std::function<bool(const char *)> ConsoleCallback;

int main() 
{
    Crazyflie crazyflie("usb://0");
    crazyflie.init();
    crazyflie.addAppChannelCallback((AppChannelCallback)[](const uint8_t * data, uint8_t dataLen){
        std::string res = "";
        for (uint8_t i = 0; i < dataLen; ++i)
        {
            res += (int)data[i];
        }
        std::cout << "appchannel: " <<res << std::endl;
        return true;
    });
        
    
    crazyflie.addConsoleCallback((ConsoleCallback)[](const char * str){
        std::cout << "console: " <<str << std::endl;
        return true;
    });
        
    int res = crazyflie.createLogBlock({
                                    // {".tof_mxRj", "bucket0"},
                                    {"motion", "deltaX"},
                                    {"kalman_pred", "tof_mxRj"},
                                    {"stabilizer","pitch"},
                                },
                                "test");

    if (res < 0)
        std::cout << "creation Error: " << res << std::endl;
    res = crazyflie.startLogBlock(10, "test");
    if (res < 0)
        std::cout << "starting Error: " << res << std::endl;
    crazyflie.addLogCallback((LogBlockCallback)[]
    (const std::map<TocItem,boost::spirit::hold_any>& tocItemsAndValues, uint32_t period)
    {
        for(auto element : tocItemsAndValues)
        {
            std::cout <<"period: "<<period << element.first._groupName<<"."<<element.first._name << " val="<< element.second<<std::endl;
        }
        return true;
    },"test");
    
    return 0;
}