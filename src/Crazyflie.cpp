#include "Crazyflie.h"
using namespace bitcraze::crazyflieLinkCpp;

Crazyflie::Crazyflie(const std::string &uri) : _con(uri), _conWorker(_con), _conWrapperParamRead(_conWorker),
                        _conWrapperParamWrite(_conWorker), _conWrapperParamToc(_conWorker), 
                        _conWrapperLogToc(_conWorker), _conWrapperAppchannel(_conWorker),
                        _logTocWpr(_logToc, _conWrapperLogToc), _paramTocWpr(_paramToc, 
                        _conWrapperParamToc), _log(_logToc,_conWorker)
{
    _isRunning = false;
}

void Crazyflie::sendAppChannelData(const void *data, const size_t &dataLen)
{
    _conWrapperAppchannel.send<uint8_t>((*(const uint8_t *)data), dataLen);
}

Connection &Crazyflie::getCon()
{
    return _con;
}

float Crazyflie::getFloatByName(const std::string &group, const std::string &name)
{
    return getParamValFromCrazyflie<float>(_paramToc.getItemId(group, name));
}

uint32_t Crazyflie::getUIntByName(const std::string &group, const std::string &name)
{
    return getParamValFromCrazyflie<uint32_t>(_paramToc.getItemId(group, name));
}

Crazyflie::~Crazyflie()
{
}

//print the TOC with values!
void Crazyflie::printParamToc()
{
    auto tocItemsVector = _paramToc.getAllTocItems();

    for (TocItem tocItem : tocItemsVector)
    {
        std::cout << tocItem;
        if (to_string(tocItem._type).find("int") != std::string::npos)
            std::cout << getParamValFromCrazyflie<uint32_t>(tocItem._id) << std::endl;
        else
            std::cout << getParamValFromCrazyflie<float>(tocItem._id) << std::endl;
    }
    std::cout << "Printed " << tocItemsVector.size() << " items total" << std::endl;
}

/*
save the TOC with values to a csv file.
input: path = the requested path.
       fileName = the requested file name. 
*/
void Crazyflie::csvParamToc(std::string path, std::string fileName)
{
    std::string filepath = path + "/" + fileName;
    std::ofstream file;
    file.open(filepath);

    auto tocItemsVector = _paramToc.getAllTocItems();

    for (TocItem tocItem : tocItemsVector)
    {
        //tocParamsFile << tocItem;
        file << (unsigned int)tocItem._id << "," << to_string(tocItem._accessType) << "," << to_string(tocItem._type) << "," << tocItem._groupName << "," << tocItem._name << ",";
        if (to_string(tocItem._type).find("int") != std::string::npos)
            file << getParamValFromCrazyflie<uint32_t>(tocItem._id) << std::endl;
        else
            file << getParamValFromCrazyflie<float>(tocItem._id) << std::endl;
    }
    std::cout << "saved " << tocItemsVector.size() << " parameters to a .csv file under the path: " << filepath << std::endl;
    file.close();
}

//print the TOC
void Crazyflie::printLogToc()
{
    auto tocItemsVector = _logToc.getAllTocItems();

    for (TocItem tocItem : tocItemsVector)
    {
        std::cout << tocItem << std::endl;
    }
    std::cout << "Printed " << tocItemsVector.size() << " items total" << std::endl;
    std::cout << "=============================================" << std::endl;
}

bool Crazyflie::init()
{
    _conWorker.start();
    _conWrapperLogToc.setPort(LOG_PORT);

    _conWrapperParamToc.setPort(PARAM_PORT);
    _conWrapperParamRead.setPort(PARAM_PORT);
    _conWrapperParamWrite.setPort(PARAM_PORT);
    _conWrapperAppchannel.setPort(APPCHANNEL_PORT);

    _conWrapperLogToc.setChannel(TOC_CHANNEL_LOG);
    _conWrapperParamToc.setChannel(TOC_CHANNEL_PARAM);
    _conWrapperParamRead.setChannel(PARAM_READ_CHANNEL);
    _conWrapperParamWrite.setChannel(PARAM_WRITE_CHANNEL);
    _conWrapperAppchannel.setChannel(APP_CHANNEL);

    _logTocWpr.initToc();

    _paramTocWpr.initToc();

    _isRunning = true;
    return true;
}
bool Crazyflie::isRunning() const
{
    return _isRunning;
}

std::vector<std::pair<TocItem, ParamValue>> Crazyflie::getTocAndValues()
{
    std::vector<std::pair<TocItem, ParamValue>> res;

    auto tocItems = _paramToc.getAllTocItems();

    for (TocItem tocItem : tocItems)
    {
        ParamValue val;
        if (tocItem._type == "float")
        {
            val._floatVal = this->getParamValFromCrazyflie<float>(tocItem._id);
        }
        else if (tocItem._type == "uint32_t")
        {
            val._floatVal = this->getParamValFromCrazyflie<uint32_t>(tocItem._id);
        }

        res.emplace_back(tocItem, val);
    }
    return res;
}

void Crazyflie::addAppChannelCallback(const AppChannelCallback &callback)
{
    auto func = (std::function<bool(bitcraze::crazyflieLinkCpp::Packet)>)[callback](Packet p_recv)
    {
        return callback(p_recv.payload(), p_recv.payloadSize());
    };
    _conWorker.addCallback({APPCHANNEL_PORT, APP_CHANNEL, func});
}

void Crazyflie::addParamReceivedCallback(const ParamValueCallback &callback)
{
      auto func = (std::function<bool(bitcraze::crazyflieLinkCpp::Packet)>)[callback](Packet p_recv)
    {
        ParamValue pVal = {0};
        uint16_t id = 0;

        std::memcpy(&pVal,p_recv.payload()+2, p_recv.payloadSize()-2);
        std::memcpy(&id,p_recv.payload(),sizeof(id));
        return callback( id,pVal);
    };
    _conWorker.addCallback({LOG_PORT, LOG_DATA_CHANNEL, func});
}

int Crazyflie::addLogCallback(const LogBlockCallback &callback, const std::string &logName)
{
    auto it = _logBlockNames.find(logName);

    if(_logBlockNames.end()== it)
        return -GENERIC_LOG_ERROR;

    uint8_t logBlockId = it->second;

    _log.addLogCallback(logBlockId,callback);
    return logBlockId;
}

void Crazyflie::addConsoleCallback(const ConsoleCallback &callback)
{

    auto func = (std::function<bool(bitcraze::crazyflieLinkCpp::Packet)>)[callback](Packet p_recv)
    {
        char data_str[33] = {0};
        data_str[p_recv.payloadSize()] = 0;
        std::memcpy(data_str, p_recv.payload(), p_recv.payloadSize());
        return callback(data_str);
    };
    _conWorker.addCallback({0, 0, func});
}


const Toc &Crazyflie::getParamToc() const
{
    return _paramToc;
}
const Toc &Crazyflie::getLogToc() const
{
    return _logToc;
}

int Crazyflie::createLogBlock(const std::vector<std::pair<std::string,std::string>>& logItemNames, const std::string &logName)
{
    TocItem tocItem =  _logToc.getItem(logItemNames.front().first,logItemNames.front().second);
    if(!tocItem)
    {
        return -GENERIC_LOG_ERROR;
    }
    int logBlockId = _log.createLogBlock(tocItem._type._type,tocItem._id);
    if(logBlockId<0) //If error
        return logBlockId;

    for(int i = 1; i < (int)logItemNames.size() ; i++)
    {
        tocItem =  _logToc.getItem(logItemNames[i].first,logItemNames[i].second);
        if(!tocItem)
        {
            _log.deleteLogBlock(logBlockId);
            
            return -GENERIC_LOG_ERROR;
        }
        int res =_log.appendLogBlock(logBlockId,tocItem._type._type,tocItem._id);
        if(res < 0)//if error
            return res;
    }
    _logBlockNames.insert({logName,logBlockId});
    return logBlockId;
}

int Crazyflie::deleteLogBlock( const std::string &logName)
{
    auto it = _logBlockNames.find(logName);

    if(_logBlockNames.end()== it)
        return -GENERIC_LOG_ERROR;

    uint8_t logBlockId = it->second;
    int res = _log.deleteLogBlock(logBlockId);

    if(res > 0) //not failed
    {
        _logBlockNames.erase(it);
    }

    return res;
}

int Crazyflie::startLogBlock( uint8_t period, const std::string &logName)
{
    auto it = _logBlockNames.find(logName);
    
    if(_logBlockNames.end()== it)
        return -GENERIC_LOG_ERROR;

    uint8_t logBlockId = it->second;

    return _log.startLogBlock(logBlockId, period);
}
int Crazyflie::stopLogBlock( const std::string &logName)
{
     auto it = _logBlockNames.find(logName);
    
    if(_logBlockNames.end()== it)
        return -GENERIC_LOG_ERROR;

    uint8_t logBlockId = it->second;

    return _log.stopLogBlock(logBlockId);
}

int Crazyflie::resetLogBlocks()
{
    return _log.resetLogBlocks();
}