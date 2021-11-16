#include "ConnectionWrapper.h"
#include <functional>

using namespace bitcraze::crazyflieLinkCpp;

ConnectionWrapper::ConnectionWrapper(ConnectionWorker &conWorker) : _conWorkerPtr(&conWorker)
{
}

ConnectionWrapper::~ConnectionWrapper()
{
}

bool isBigEndian()
{
    union
    {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1;
}

void ConnectionWrapper::setPort(int port)
{
    _port = port;
}

void ConnectionWrapper::setChannel(int channel)
{
    _channel = channel;
}
