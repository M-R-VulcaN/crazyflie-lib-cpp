#include <algorithm>
#include <chrono>
#include <errno.h>
#include "Crazyflie.h"

using bitcraze::crazyflieLinkCpp::Connection;
using bitcraze::crazyflieLinkCpp::Packet;

// #define CONTROL_APPEND_BLOCK    1
#define CONTROL_DELETE_BLOCK 2
#define CONTROL_START_BLOCK 3
#define CONTROL_STOP_BLOCK 4
#define CONTROL_RESET 5
#define CONTROL_CREATE_BLOCK_V2 6
#define CONTROL_APPEND_BLOCK_V2 7

#define CONTROL_CH 1
#define CRTP_PORT_LOG 0x05
// #define MAX_LEN_NAME 32

enum UserChoices
{
    EXIT_CHOICE,
    CREATE_BLOCK_CHOICE,
    APPEND_BLOCK_CHOICE,
    DELETE_BLOCK_CHOICE,
    START_BLOCK_CHOICE,
    STOP_BLOCK_CHOICE,
    BLOCK_RESET,
    LOG_RECEIVE,
    LOG_TOC_PRINT
};

std::pair<uint16_t,std::string> enterIdOrName(uint16_t defaultId)
{
    char userInputStr[MAX_LEN_NAME];

    std::cout << "Enter log block id or group and name: (group.name) " << std::endl;
    uint16_t logBlockId = defaultId;
    std::cin.getline(userInputStr, MAX_LEN_NAME - 1, '\n');
    bool hasOnlyDigits = (std::string(userInputStr).find_first_not_of( "0123456789" ) == std::string::npos);
    if(hasOnlyDigits)
        logBlockId = std::stoi(userInputStr);
    else
    {
        std::cout << defaultId << std::endl;
        return {logBlockId,userInputStr};
    }
    std::cin.ignore(INT32_MAX, '\n');
    std::cout << "Enter log group and name in the following format:(group.name): " << std::endl;
    userInputStr[MAX_LEN_NAME] = 0;

    std::cin.getline(userInputStr, MAX_LEN_NAME - 1, '\n');

    return {logBlockId,userInputStr};
}

int main()
{
    Crazyflie crazyflie("usb://0");
    
    const Toc &tocRef = crazyflie.getLogToc();
    crazyflie.init();
    Log& log = crazyflie._log;
    uint16_t currentId = 0;
    while (true)
    {
        uint16_t userInput = 0;

        std::cout << "===================================" << std::endl;
        std::cout << "Menu:" << std::endl;
        std::cout << "1 - Create Log Block" << std::endl;
        std::cout << "2 - Append To Log Block" << std::endl;
        std::cout << "3 - Delete Log Block" << std::endl;
        std::cout << "4 - Start receiving from Log Block" << std::endl;
        std::cout << "5 - Stop receiving from Log Block" << std::endl;
        std::cout << "6 - log reset - delete all log blocks" << std::endl;
        std::cout << "7 - log receive - receive the log blocks which are started" << std::endl;
        std::cout << "8 - log toc print - simply printing all TOC parameters that in the log" << std::endl;
        std::cout << "0 - Exit" << std::endl;
        std::cout << ">> ";
        std::cin >> userInput;

        switch (userInput)
        {
        case EXIT_CHOICE:
            return 0;

        case CREATE_BLOCK_CHOICE:
            std::cout << "Enter log group and name in the following format:(group.name): " << std::endl;
            std::cin.ignore(INT32_MAX, '\n');
            {
                char userInputStr[MAX_LEN_NAME];
                userInputStr[MAX_LEN_NAME] = 0;

                std::cin.getline(userInputStr, MAX_LEN_NAME - 1, '\n');

                std::string temp = std::string(userInputStr);
                std::string groupName = temp.substr(0, temp.find("."));
                std::string paramName = temp.substr(groupName.length() + 1);

                uint16_t logId = tocRef.getItemId(groupName, paramName);
                auto tocItem = tocRef.getItem(groupName, paramName);

                int response = log.createLogBlock(tocItem._type._type,logId);

                if(response<0)
                {
                    std::cout << "An Error Occured: "<< -response <<std::endl;
                }
                else
                {
                    std::cout << "Successfully created a new log block! Log Block Id = " << response <<std::endl;
                    currentId = response;
                }
            }
            break;

        case APPEND_BLOCK_CHOICE:
            std::cin.ignore(INT32_MAX, '\n');
            {
                auto res = enterIdOrName(currentId);
                int logBlockId = res.first;

                std::string temp = res.second;
                std::string groupName = temp.substr(0, temp.find("."));
                std::string paramName = temp.substr(groupName.length() + 1);

                uint16_t logId = tocRef.getItemId(groupName, paramName);
                auto tocItem = tocRef.getItem(groupName, paramName);
                int response = log.appendLogBlock(logBlockId,tocItem._type._type,logId);
                if(response<0)
                {
                    std::cout << "An Error Occured: "<< -response<<std::endl;
                }
                else
                {
                    std::cout << "Successfully appended log block!     Log Block Id = " <<response<<std::endl;
                }
            }
            break;

        case DELETE_BLOCK_CHOICE:
            {
                int id = 0;
                std::cout << "Enter log id:" << std::endl;
                std::cin >> id;
                int response = log.deleteLogBlock(id);
                if(response<0)
                {
                    std::cout << "An Error Occured: "<< -response<<std::endl;
                }
                else
                {
                    std::cout << "Successfully deleted log block!     Log Block Id = " <<response<<std::endl;
                }
                break;
            }

        case START_BLOCK_CHOICE:
            {
                int id = 0;
                int PeriodTimeInMs = 0;
                std::cout << "Enter log id:" << std::endl;
                std::cin >> id;
                std::cout << "Enter perion time in ms:" << std::endl;
                std::cin >> PeriodTimeInMs;
                if(PeriodTimeInMs/10 > 0)
                {
                    int response = log.startLogBlock(id, PeriodTimeInMs/10);
                    if(response<0)
                    {
                        std::cout << "An Error Occured: "<< -response<<std::endl;
                    }
                    else
                    {
                        std::cout << "Successfully started logging!     Log Block Id = " <<response<<std::endl;
                    }
                    break;
                }
                else
                {   
                    std::cout << "Error! perion time should be more than 10! (in ms)" << std::endl;
                }
                break;    
            }

        case STOP_BLOCK_CHOICE:
            {
                int id = 0;
                std::cout << "Enter log id:" << std::endl;
                std::cin >> id;
                
                int response = log.stopLogBlock(id);
                if(response<0)
                    {
                        std::cout << "An Error Occured: "<< -response<<std::endl;
                    }
                    else
                    {
                        std::cout << "Successfully Stopped logging!     Log Block Id = " <<response<<std::endl;
                    }
                break;
            }

        case BLOCK_RESET:
            {
                int response = log.resetLogBlocks();
                if(response<0)
                    {
                        std::cout << "An Error Occured: "<< -response<<std::endl;
                    }
                    else
                    {
                        std::cout << "Success! Finished reseting all blocks"<<std::endl;
                    }
                break;
            }

        case LOG_RECEIVE:
{
            // std::cin.ignore(INT32_MAX, '\n');

            // std::mutex mu;
            // std::unique_lock<std::mutex> lock(mu);
            // std::mutex *muPtr = &mu;
            // std::condition_variable waitTillFinished;
            // std::condition_variable *waitTillFinishedPtr = &waitTillFinished;
            // std::atomic<bool> isFinished(false);
            // std::atomic<bool> *isFinishedPtr = &isFinished;
            // std::atomic<bool> isCallbackFinished(true);
            // std::atomic<bool> *isCallbackFinishedPtr = &isCallbackFinished;
            // for(int i = 0; i <MAX_UINT8; ++i)
            // {

            // }
            // log.addLogCallback(,[log,isFinishedPtr,muPtr,waitTillFinishedPtr,isCallbackFinishedPtr]
            // (const std::map<TocItem,void*>& tocItemsAndValues, uint32_t period){
            //     *isCallbackFinishedPtr = false;
            //     std::lock_guard<std::mutex> lock(*muPtr);
            //     std::cout <<"  period:  " << period << "  val=  ";
            //             for(auto element : tocItemsAndValues)
            //             {
                        
            //                 if(to_string(element.first._type).find("uint")!=std::string::npos)
            //                 {
            //                     uint32_t res = 0;
            //                     std::memcpy(&res, element.second,element.first._type.size());
            //                     std::cout << res;
            //                 }
            //                 else if (to_string(element.first._type).find("int")!=std::string::npos)
            //                 {
            //                     int32_t res = 0;
            //                     std::memcpy(&res, element.second,element.first._type.size());
            //                     std::cout << res;
            //                 }
            //                 else if (element.first._type == "float")
            //                 {
            //                     std::cout << *(float*)element.second;
            //                 }
            //                 std::cout <<"  ";
            //             }
            //             std::cout << std::endl;

            //     if ((bool)*isFinishedPtr)
            //     {
            //         *isCallbackFinishedPtr = true;
            //         waitTillFinishedPtr->notify_all();
            //         return false;
            //     }
            //     return true;
            // }); 
            // std::cout << "Press enter to stop receiving"<<std::endl;
            // lock.unlock();
            // std::cin.getline(nullptr,0,'\n'); 
            // lock.lock(); 
            // isFinished = true;
            // waitTillFinished.wait(lock,[isCallbackFinishedPtr](){return (bool)*isCallbackFinishedPtr;});       
            break;
        }
        
        case LOG_TOC_PRINT:
            {
                crazyflie.printLogToc();
                break;
            }

        default:
            break;
        }
    }

    return 0;
}
