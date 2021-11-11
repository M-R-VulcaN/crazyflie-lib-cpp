#pragma once

#include "WrapperTemplate.h"
#include "Toc.h"
#include "ConnectionWrapper.h"

#define CMD_TOC_ELEMENT 0 // original version: up to 255 entries
#define CMD_TOC_INFO 1    // original version: up to 255 entries
#define CMD_TOC_ITEM_V2 2 // version 2: up to 16k entries
#define CMD_TOC_INFO_V2 3 // version 2: up to 16k entries

class TocWrapper : WrapperTemplate<Toc, TocWrapper>
{
private:
    ConnectionWrapper *_conWpr;
    TocItem getTocItemFromCrazyflie(uint16_t id) const;

public:
    TocWrapper(Toc &toc, ConnectionWrapper &con);
    void initToc();

    virtual TocWrapper &operator=(Toc &toc)
    {
        WrapperTemplate<Toc, TocWrapper>::_core = &toc;
        return *this;
    }

    virtual ~TocWrapper();
};
