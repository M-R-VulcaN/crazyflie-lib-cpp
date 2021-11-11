#include "LoggingCrazyflieWrapper.h"
#include <list>
#include <chrono>
#include <thread>

#define ACK_DELAY_MICRO_SEC 30

using namespace bitcraze::crazyflieLinkCpp;

LoggingCrazyflieWrapper::LoggingCrazyflieWrapper(Crazyflie &crazyflie, const std::string &outputFilePath)
{
    _crazyflie = &crazyflie;
    _outputFilePath = outputFilePath;
}

LoggingCrazyflieWrapper::~LoggingCrazyflieWrapper()
{
}

void LoggingCrazyflieWrapper::start(bool withDebugging)
{
    std::mutex mu;
    std::mutex* muPtr = &mu;
    std::unique_lock<std::mutex> lock(mu);
    std::condition_variable waitForLoggingToFinish;
    std::condition_variable* waitForLoggingToFinishPtr = &waitForLoggingToFinish;

    _crazyflie->setParamByName<uint8_t>("usd", "logging", 0);
    _crazyflie->setParamByName<uint8_t>("usd", "sendAppChannle", 1);
  
    Debug::passFlag = true;
    Crazyflie* cfPtr = _crazyflie;

    std::ofstream outputFile(_outputFilePath);
    std::ofstream* outputFilePtr= &outputFile;
    std::atomic<bool> isFinished(false);
    std::atomic<bool>* isFinishedPtr = &isFinished;
    uint32_t currMemAddress = 0;
    uint32_t* currMemAddressPtr = &currMemAddress;
    uint32_t dataSize = 0;
    uint32_t* dataSizePtr = &dataSize;
    auto start = std::chrono::steady_clock::now();
    auto startTimePtr = &start;
    bool* withDebuggingPtr = &withDebugging;
    _crazyflie->addAppChannelCallback((AppChannelCallback)[withDebuggingPtr,cfPtr, outputFilePtr,currMemAddressPtr,dataSizePtr,startTimePtr,muPtr,waitForLoggingToFinishPtr,isFinishedPtr](const uint8_t* data, uint8_t dataLen)
    {
        uint32_t& currMemAddress = *currMemAddressPtr;
        uint32_t& dataSize = *dataSizePtr;
        auto& start = *startTimePtr;
        if (0==dataLen)
        {
            if (*withDebuggingPtr)
                std::cout << "Error Receiving from crazyflie" << std::endl;
        }
        uint8_t packetCode = data[0];
        if (*withDebuggingPtr)
            std::cout << (int)packetCode << "-> ";
        uint8_t response[5] = {0};
        unsigned int ackRequestMemAddress = 0;
        uint32_t crazyflieCurrMemAddress = 0;

        switch (packetCode)
        {
        case 0:
            std::copy_n(data + 1, sizeof(dataSize), (uint8_t *)&dataSize);
            if (*withDebuggingPtr)
                std::cout << "Data size: " << dataSize << std::endl;
            response[0] = 0;
            cfPtr->sendAppChannelData(&response[0], sizeof(uint8_t)); //size msg ack

            break;

        case 1:
            std::copy_n(data + 1,  sizeof(crazyflieCurrMemAddress), (uint8_t *)&crazyflieCurrMemAddress);

            //send not all data recieved
            if (crazyflieCurrMemAddress != currMemAddress)
            {
                break;
            }
            currMemAddress += dataLen - sizeof(currMemAddress) - sizeof(uint8_t);
            if (*withDebuggingPtr)
                std::cout << "Current Memory Address: " << (unsigned int)currMemAddress << std::endl;
            outputFilePtr->write(reinterpret_cast<const char *>(&data[5]), dataLen - 5);

            break;

        case 2:
            std::copy_n(data + 1,   sizeof(ackRequestMemAddress), (uint8_t *)&ackRequestMemAddress);

            std::this_thread::sleep_for(std::chrono::microseconds(ACK_DELAY_MICRO_SEC));

            if (ackRequestMemAddress == currMemAddress)
            {
                if(*withDebuggingPtr)
                    std::cout << "Ack Request Mem Address: " << ackRequestMemAddress << std::endl;

                response[0] = 0;
            }
            else
            {
                if (*withDebuggingPtr)
                    std::cout << "Wrong Memory Address: " << (unsigned int)currMemAddress << std::endl;

                response[0] = 2;
            }

            std::copy_n((uint8_t *)&currMemAddress, sizeof(currMemAddress), response + 1);
            cfPtr->sendAppChannelData(response, sizeof(response)); //data msg ack
            break;

        default:
            if (*withDebuggingPtr)
                std::cout << "Incorrect Msg type" << currMemAddress << std::endl;
            response[0] = 1;
            cfPtr->sendAppChannelData(response, sizeof(uint8_t));

            break;
        }
        if(currMemAddress >= dataSize)
        {
            outputFilePtr->close();
             std::chrono::duration<double> delta = std::chrono::steady_clock::now() - start;
            if (*withDebuggingPtr)
            {
                std::cout << "Time [sec]: " << delta.count() << std::endl;
                std::cout << "rate[bytes/sec] : " << dataSize / (delta.count()) << std::endl;
            }
            else
            {
                std::cout << dataSize / (delta.count()) << std::endl;
            }
            std::lock_guard<std::mutex> lock(*muPtr);
            *isFinishedPtr = true;
            waitForLoggingToFinishPtr->notify_all();
            return false;
        }
        return true;
    });
    waitForLoggingToFinish.wait(lock,[isFinishedPtr](){return (bool)*isFinishedPtr;});
}
