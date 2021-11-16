#pragma once

template <typename CoreType, typename InheriterType>
class WrapperTemplate
{
protected:
    CoreType *_core;

public:
    WrapperTemplate(CoreType &core)
    {
        _core = &core;
    }
    virtual InheriterType &operator=(CoreType &core) = 0;
    CoreType &operator*()
    {
        return *_core;
    }

    CoreType *operator->()
    {
        return &_core;
    }

    virtual ~WrapperTemplate()
    {
    }
};
