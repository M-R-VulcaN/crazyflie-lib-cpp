#pragma once

#include <iostream>
#include "ConnectionWorker.h"

#define PAYLOAD_MAX_SIZE CRTP_MAXSIZE - 2

struct TocItemMessage
{
    uint8_t _cmd;
    uint16_t _id;
};

bool isBigEndian();

const bool IS_BIG_ENDIAN = isBigEndian();
class ConnectionWrapper
{
private:
    uint8_t _port;
    uint8_t _channel;

    ConnectionWorker *_conWorkerPtr;

public:
    ConnectionWrapper(ConnectionWorker &conWorker);

    ConnectionWrapper &operator=(bitcraze::crazyflieLinkCpp::Connection &con);
    template <class ValToSend>

    void send(const ValToSend &dataToSend, size_t sizeOfDataToSend = 0) const
    {
        bitcraze::crazyflieLinkCpp::Packet p_send;
        p_send.setPort(_port);
        p_send.setChannel(_channel);

        if (sizeOfDataToSend > 0)
        {
            std::memcpy(p_send.payload(), (const uint8_t *)&dataToSend, sizeOfDataToSend);
            p_send.setPayloadSize(sizeOfDataToSend);
        }
        else
        {
            std::memcpy(p_send.payload(), (const uint8_t *)&dataToSend, sizeof(dataToSend));
            p_send.setPayloadSize(sizeof(dataToSend));
        }
        _conWorkerPtr->send(p_send);
    }

    template <class ValToSend>
    bitcraze::crazyflieLinkCpp::Packet sendRecvData(int timeout, const ValToSend &dataToSend, size_t sizeOfDataToSend = 0) const
    {

        std::mutex mu;
        std::unique_lock<std::mutex> lock(mu);
        std::mutex *muPtr = &mu;
        std::condition_variable waitForRecv;
        std::condition_variable *waitForRecvPtr = &waitForRecv;
        std::atomic<bool> isResultReturned(false);
        std::atomic<bool> *isResultReturnedPtr = &isResultReturned;

        bitcraze::crazyflieLinkCpp::Packet res;
        bitcraze::crazyflieLinkCpp::Packet *resPtr = &res;

        PacketCallbackBundle callbackBundle = {this->_port, this->_channel, (PacketCallback)[resPtr, waitForRecvPtr, isResultReturnedPtr, muPtr](bitcraze::crazyflieLinkCpp::Packet p_recv){std::lock_guard<std::mutex> lock(*muPtr);
            *resPtr = p_recv;
            waitForRecvPtr->notify_all();
            *isResultReturnedPtr = true;
            return false;
        }};

        _conWorkerPtr->addCallback(callbackBundle);

        this->send(dataToSend, sizeOfDataToSend);

        if (isResultReturned)
            return res;
    
        if (0 == timeout)
        {
            waitForRecv.wait(lock, [isResultReturnedPtr]()
                            { return (bool)*isResultReturnedPtr; });
        }
        else
        {
            waitForRecv.wait_for(lock, std::chrono::milliseconds(timeout), [isResultReturnedPtr]()
                                { return (bool)*isResultReturnedPtr; });
        }

        return res;
    }


void setPort(int port);
void setChannel(int channel);

void sendData(const void *data1, const size_t &data1_len, const void *data2 = nullptr, const size_t &data2_len = 0) const;


~ConnectionWrapper();
}
;

class PacketData
{
private:
    std::array<uint8_t, CRTP_MAXSIZE> _data;
    uint8_t _currIndex = 0;

public:
    PacketData()
    {
    }
    ~PacketData()
    {
    }
    template <class T>
    PacketData(const T &data, const size_t &sizeOfData = 0)
    {
        this->appendData(data, sizeOfData);
    }
    template <class T>
    void appendData(const T &data, const size_t &sizeOfData = 0)
    {
        uint8_t sizeToCopy;

        if (sizeOfData > 0)
            sizeToCopy = std::min({(uint8_t)sizeOfData, (uint8_t)(CRTP_MAXSIZE - _currIndex), (uint8_t)sizeof(data)});
        else
            sizeToCopy = std::min((uint8_t)sizeof(data), (uint8_t)(CRTP_MAXSIZE - _currIndex));

        std::copy_n((uint8_t *)&data, _currIndex, _data);
        _currIndex += sizeToCopy;
    }
    std::array<uint8_t, CRTP_MAXSIZE> getData() const
    {
        return _data;
    }
    uint8_t size() const
    {
        return _currIndex + 1;
    }
};