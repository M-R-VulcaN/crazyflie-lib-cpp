#include "TocWrapper.h"

using bitcraze::crazyflieLinkCpp::Connection;

TocWrapper::TocWrapper(Toc &toc, ConnectionWrapper &conWpr) : WrapperTemplate<Toc, TocWrapper>(toc), _conWpr(&conWpr)
{
}

TocWrapper::~TocWrapper()
{
}

void TocWrapper::initToc()
{
    // ask for the toc info
    uint8_t cmd = CMD_TOC_INFO_V2;

    TocInfo cfTocInfo(_conWpr->sendRecvData(0, cmd));

    uint16_t num_of_elements = cfTocInfo._numberOfElements;
    for (uint16_t i = 0; i < num_of_elements; i++)
    {
        _core->insert(getTocItemFromCrazyflie(i));
    }
}

TocItem TocWrapper::getTocItemFromCrazyflie(uint16_t id) const
{
    uint8_t cmd = CMD_TOC_ITEM_V2;
    // ask for a param with the given id
    struct __attribute__((packed))
    {
        uint8_t _cmd;
        uint16_t _id;
    } data = {cmd, id};
    return TocItem(_conWpr->sendRecvData(0, data));
}
