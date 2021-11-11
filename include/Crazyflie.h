#pragma once

#include <map>
#include <vector>
#include <thread>
#include <fstream> //for saving .csv file

#include "ConnectionWrapper.h"
#include "TocWrapper.h"
#include "ConnectionWorker.h"
#include "Log.h"

#define PAYLOAD_VALUE_BEGINING_INDEX 3
#define NOT_FOUND 0

#define APP_CHANNEL 2

#define TOC_CHANNEL_LOG 0 //	Table of content access: Used for reading out the TOC

#define TOC_CHANNEL_PARAM 0
#define PARAM_READ_CHANNEL 1
#define PARAM_WRITE_CHANNEL 2

#define LOG_CONTROL_CHANNEL 1 //	Log control: Used for adding/removing/starting/pausing log blocks
#define LOG_DATA_CHANNEL 2    // 	Log data: Used to send log data from the Crazyflie to the client


#define APPCHANNEL_PORT 13

typedef std::function<bool(const uint8_t *, uint8_t)> AppChannelCallback;
typedef std::function<bool(uint16_t, const ParamValue &)> ParamValueCallback;
typedef std::function<bool(const char *)> ConsoleCallback;
// typedef std::function<bool(uint8_t,uint32_t, const std::vector<uint8_t>&)> LogBlockReceivedCallback;

class Crazyflie
{
private:
    Toc _paramToc;
    Toc _logToc;

    bitcraze::crazyflieLinkCpp::Connection _con;
    ConnectionWorker _conWorker;
    ConnectionWrapper _conWrapperParamRead;
    ConnectionWrapper _conWrapperParamWrite;
    ConnectionWrapper _conWrapperParamToc;
    ConnectionWrapper _conWrapperLogToc;
    ConnectionWrapper _conWrapperAppchannel;
    TocWrapper _logTocWpr;
    TocWrapper _paramTocWpr;
    bool _isRunning;
    std::thread _paramRecvThread;
    public:
    Log _log;
    private:
    std::map<std::string,uint8_t> _logBlockNames;
    template <class Val>

    bool setParamValCrazyflie(uint16_t paramId, const Val &newValue)
    {
        struct __attribute__((packed))
        {
            uint16_t _paramId;
            Val _newValue;
        } data = {paramId, newValue};

        _conWrapperParamWrite.sendRecvData(0, data);

        return true;
    }
    template <class Val>
    Val getParamValFromCrazyflie(uint16_t paramId)
    {
        Val res = 0;
        bitcraze::crazyflieLinkCpp::Packet p = _conWrapperParamRead.sendRecvData(0, paramId);
        std::memcpy(&res, p.payload() + PAYLOAD_VALUE_BEGINING_INDEX, std::min(sizeof(res), p.payloadSize() - PAYLOAD_VALUE_BEGINING_INDEX));

        return res;
    }
    void initParamToc();
    void initLogToc();

    void paramRecvThreadFunc();

public:
    void addAppChannelCallback(const AppChannelCallback &callback);
    void addParamReceivedCallback(const ParamValueCallback &callback);
    void addConsoleCallback(const ConsoleCallback &callback);
    int addLogCallback(const LogBlockCallback &callback, const std::string &logName);
    Crazyflie(const std::string &uri);
    ~Crazyflie();
    bool isRunning() const;
    bool init();
    bitcraze::crazyflieLinkCpp::Connection &getCon();

    uint32_t getUIntByName(const std::string &group, const std::string &name);
    float getFloatByName(const std::string &group, const std::string &name);
    template <class Val>

    bool setParamByName(const std::string &group, const std::string &name, Val newValue)
    {
        return setParamValCrazyflie<Val>(_paramToc.getItemId(group, name), newValue);
    }
    const Toc &getParamToc() const;
    const Toc &getLogToc() const;
    void printParamToc();
    void csvParamToc(std::string path, std::string fileName);
    void printLogToc();
    std::vector<std::pair<TocItem, ParamValue>> getTocAndValues();

    void sendAppChannelData(const void *data, const size_t &dataLen);

 

    /**
     * Creates the entire log block
     * if successful returns a non-negative intiger representing the logBlockId 
     * if unsuccessful it returns a negative integer representing the error. eg: -ENOENT = -2 
    */
    int createLogBlock(const std::vector<std::pair<std::string,std::string>>& logItemNames, const std::string &logName);

    /**
     * If successful returns non-negative integer representing the deleted logBlock id. eg: id = 5 or id = 0
     * If failed returns the error code as a negative integer. eg: -ENOENT = -2 
    */
    int deleteLogBlock( const std::string &logName);

    /**
     * If successful returns non-negative integer representing the logBlock id that was started. eg: id = 5 or id = 0
     * If failed returns the error code as a negative integer. eg: -ENOENT = -2 
    */
    int startLogBlock( uint8_t period, const std::string &logName);

    /**
     * If successful returns non-negative integer representing the logBlock id which was stopped. eg: id = 5 or id = 0
     * If failed returns the error code as a negative integer. eg: -ENOENT = -2 
    */
    int stopLogBlock( const std::string &logName);

    /**
     * If successful returns 0
     * If failed returns the error code as a negative integer. eg: -ENOENT = -2 
    */
    int resetLogBlocks();

};
