#include "Log.h"
#include <errno.h>
using bitcraze::crazyflieLinkCpp::Connection;
using bitcraze::crazyflieLinkCpp::Packet;
Log::Log(Toc &toc, ConnectionWorker &conWorker) : _tocPtr(&toc), _conWpr(conWorker), _conWorkerPtr(&conWorker)
{
    _conWpr.setPort(CRTP_PORT_LOG);
    _conWpr.setChannel(CONTROL_CH);
    std::fill_n(idsOccupied, UINT8_MAX, OccupiedStatus::UNKNOWN);
}

Log::~Log()
{
}

//  if(to_string(element.first._type).find("uint")!=std::string::npos)
//                             {
//                                 uint32_t res = 0;
//                                 std::memcpy(&res, element.second,element.first._type.size());
//                                 std::cout << res;
//                             }
//                             else if (element.first._type == "int8_t")
//                             {
//                                 std::cout << (int)*(int8_t*)element.second;
//                             }
//                              else if (element.first._type == "int16_t")
//                             {
//                                 std::cout << *(int16_t*)element.second;
//                             }
//                              else if(element.first._type == "int32_t")
//                             {
//                                 std::cout << *(int32_t*)element.second;
//                             }
//                             else if (element.first._type == "float")
//                             {
//                                 std::cout << *(float*)element.second;
//                             }

void Log::addLogCallback(uint8_t id,const LogBlockCallback &callback)
{
    std::list<TocItem>* logBlockPtr = &(_logBlocks.find(id)->second);
    auto func = (std::function<bool(bitcraze::crazyflieLinkCpp::Packet)>)[callback,logBlockPtr,id](Packet p_recv)
    {
        if(p_recv.payload()[0] != id) 
            return true;
        std::map<TocItem,boost::spirit::hold_any> result;
        uint8_t* dataPtr = (uint8_t*)p_recv.payload()+PAYLOAD_READ_LOG_DATA_START_INDEX;
        uint32_t period = 0;
        std::memcpy(&period,p_recv.payload()+1,3);
        for(TocItem tocItem : *logBlockPtr)
        {
            boost::spirit::hold_any value;
            if(to_string(tocItem._type).find("uint")!=std::string::npos)
            {
                uint32_t res = 0;
                std::memcpy(&res, dataPtr,tocItem.size());
                value = res;
            }
            else if (tocItem._type == "int8_t")
            {
                value =*(int8_t*)dataPtr;
            }
                else if (tocItem._type == "int16_t")
            {
                value =*(int16_t*)dataPtr;
            }
                else if(tocItem._type == "int32_t")
            {
                value = *(int32_t*)dataPtr;
            }
            else if (tocItem._type == "float")
            {
                value = *(float*)dataPtr;
            }
            result.insert(std::make_pair(tocItem,value));

            dataPtr = dataPtr + (int)tocItem.size();

        }
        return callback(result,period);
    };
    _conWorkerPtr->addCallback({LOG_PORT, DATA_CH, func});
}


int Log::createLogBlock(uint8_t logType, uint16_t logId)
{
    uint16_t i = 0;
    uint8_t data[] = {CONTROL_CREATE_BLOCK_V2, 0, logType, (uint8_t)(logId & 0xff), (uint8_t)(logId >> 8)};
    uint8_t failCode = 0;

    for (i = 0; i < UINT8_MAX; i++)
    {
        if (idsOccupied[i] != OccupiedStatus::OCCUPIED)
        {
            data[1] = i;
            Packet p_recv = _conWpr.sendRecvData(0, data);
            failCode = p_recv.payload()[2];
            if (EEXIST == failCode)
                continue;

            if (LOG_SUCCESS == failCode)
            {
                _logBlocks.insert({i,{_tocPtr->getItem(logId)}});
                idsOccupied[i] = OccupiedStatus::OCCUPIED;
                return i;
            }
            return -failCode;
        }

    }
    return -GENERIC_LOG_ERROR;
}

int Log::createLogBlock(uint8_t id, uint8_t logType, uint16_t logId)
{
    if (idsOccupied[id] != OccupiedStatus::OCCUPIED)
    {
        uint8_t data[] = {CONTROL_CREATE_BLOCK_V2, id, logType, (uint8_t)(logId & 0xff), (uint8_t)(logId >> 8)};

        uint8_t failCode = 0;

        Packet p_recv = _conWpr.sendRecvData(0, data);
        failCode = p_recv.payload()[2];
        if (LOG_SUCCESS == failCode)
        {
            _logBlocks.insert({id,{_tocPtr->getItem(logId)}});
            idsOccupied[id] = OccupiedStatus::OCCUPIED;
            return id;
        }
        return -failCode;
    }
    return -GENERIC_LOG_ERROR;
}

int Log::deleteLogBlock(uint8_t id)
{
    uint8_t data[] = {CONTROL_DELETE_BLOCK, id};

    uint8_t failCode = LOG_SUCCESS;

    if (idsOccupied[id] != OccupiedStatus::NOT_OCCUPIED)
    {
        Packet p_recv = _conWpr.sendRecvData(0, data);
        failCode = p_recv.payload()[2];
        if (ENOENT == failCode || 0 == failCode)
        {
            _logBlocks.erase(id);

            idsOccupied[id] = OccupiedStatus::NOT_OCCUPIED;
            return id;
        }
        return -failCode;
    }
    return -GENERIC_LOG_ERROR;
}

int Log::appendLogBlock(uint8_t id, uint8_t logType, uint16_t logId)
{
    uint8_t data[] = {CONTROL_APPEND_BLOCK_V2, id, logType, (uint8_t)(logId & 0xff), (uint8_t)(logId >> 8)};
    uint8_t failCode = 0;
    if (idsOccupied[id] != OccupiedStatus::NOT_OCCUPIED)
    {
        data[1] = id;
        Packet p_recv = _conWpr.sendRecvData(0, data);
        failCode = p_recv.payload()[2];
        if (LOG_SUCCESS == failCode)
        {
            _logBlocks[id].push_back(_tocPtr->getItem(logId));
            
            idsOccupied[id] = OccupiedStatus::OCCUPIED;
            return id;
        }
        return -failCode;
    }
    return -GENERIC_LOG_ERROR;
}
int Log::startLogBlock(uint8_t id, uint8_t period)
{
    if (idsOccupied[id] != OccupiedStatus::NOT_OCCUPIED)
    {

        uint8_t data[] = {CONTROL_START_BLOCK, id, period};

        Packet p_recv = _conWpr.sendRecvData(0, data);

        uint8_t failCode = p_recv.payload()[2];

        if (LOG_SUCCESS == failCode)
        {
            idsOccupied[id] = OccupiedStatus::OCCUPIED;
            return id;
        }

        return -failCode;
    }
    return -GENERIC_LOG_ERROR;
}

int Log::stopLogBlock(uint8_t id)
{
    if (idsOccupied[id] != OccupiedStatus::NOT_OCCUPIED)
    {
        uint8_t data[] = {CONTROL_STOP_BLOCK, id};

        uint8_t failCode = 0;

        Packet p_recv = _conWpr.sendRecvData(0, data);
        failCode = p_recv.payload()[2];
        if (LOG_SUCCESS == failCode)
        {
            return id;
        }

        return -failCode;
    }
    return -GENERIC_LOG_ERROR;
}

int Log::resetLogBlocks()
{
    uint8_t data[] = {CONTROL_RESET};
    uint8_t failCode = 0;

    Packet p_recv = _conWpr.sendRecvData(0, data);
    failCode = p_recv.payload()[2];

    std::fill(idsOccupied, idsOccupied + UINT8_MAX, OccupiedStatus::NOT_OCCUPIED);
    if(LOG_SUCCESS == failCode)
    {
        _logBlocks.clear();
    }
    return -failCode;
}

std::list<TocItem> Log::getLogBlock(uint8_t id) const
{
    auto res = _logBlocks.find(id);
    if(res == _logBlocks.end())
        return std::list<TocItem>();
    return res->second;
}
