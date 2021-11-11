#include "Toc.h"

const uint8_t AccessType::RW = 0;
const uint8_t AccessType::RO = 1;
const uint8_t AccessType::NOACC = 2;

TocItem::TocItem(const bitcraze::crazyflieLinkCpp::Packet &p_recv) 
{
    _id = 0;
    memcpy(&_id, &p_recv.payload()[1], sizeof(_id));
    uint8_t typeAndAccessType = 0;
    memcpy(&typeAndAccessType, &p_recv.payload()[3], sizeof(typeAndAccessType));

    _groupName = (const char *)(p_recv.payload()) + 4;
    _name = (const char *)(p_recv.payload()) + 4 + _groupName.length() + 1;
    if(p_recv.port() == LOG_PORT)
    {
        _type = typeAndAccessType;
        _type._isParam = false;
        _accessType = AccessType::NOACC;
    }
    _type = typeAndAccessType & ~ACCESS_TYPE_BYTE;
    if ((typeAndAccessType & ACCESS_TYPE_BYTE) == (ACCESS_TYPE_BYTE * ((int)AccessType::RO)))
    {
        _accessType = AccessType::RO;
    }
    else
    {
        _accessType = AccessType::RW;
    }
}
bool operator==(const std::string&val, const TocItemType& self)
{
    return self == val;
}

bool TocItem::operator<(const TocItem &other) const
{
    return ((unsigned)_id) < ((unsigned)other._id);
}

bool TocItem::operator>(const TocItem &other) const
{
    return ((unsigned)_id) > ((unsigned)other._id);
}
TocItem& TocItem::operator=(const TocItem& other)
{
    _groupName = other._groupName;
    _name = other._name;
    _type = other._type;
    _accessType = other._accessType;
    _id = other._id;
    return *this;
}


std::string to_string(AccessType const &self)
{
    if(self._accessType == AccessType::NOACC)
        return "";
    return AccessType::RO == self._accessType ? "RO" : "RW";
}

AccessType &AccessType::operator=(const std::string &strAccessType)
{
    if (strAccessType == "RW")
        _accessType = AccessType::RW;
    else if (strAccessType == "RO")
        _accessType = AccessType::RO;
    else if (strAccessType == "" || strAccessType == "NOACC")
        _accessType = AccessType::NOACC;
    return *this;
}
AccessType &AccessType::operator=(const uint8_t &accessType)
{
    _accessType = accessType;

    return *this;
}

std::ostream &operator<<(std::ostream &out, const TocItem &tocItem)
{
    out << tocItem._id << ": " << to_string(tocItem._accessType) << ":" << to_string(tocItem._type) << "  " << tocItem._groupName << "." << tocItem._name << "  val=";

    return out;
}

TocInfo::TocInfo(const bitcraze::crazyflieLinkCpp::Packet &p_recv)
{
    memcpy(&_numberOfElements, &p_recv.payload()[1], sizeof(_numberOfElements));
    memcpy(&_crc, &p_recv.payload()[3], sizeof(_crc));
}

std::ostream &operator<<(std::ostream &out, const TocInfo &tocInfo)
{
    out << "numberOfElements: " << (int)tocInfo._numberOfElements << std::endl;
    out << "crc: " << (int)tocInfo._crc << std::endl;
    return out;
}

std::string to_string(TocItemType const &self)
{   
    if(self._isParam)
    {
        auto res = PARAM_TYPES.find(self._type);

        if (res != PARAM_TYPES.end())
        {
            return res->second;
        }
    }
    else
    {
        auto res = LOG_TYPES.find(self._type);

        if (res != LOG_TYPES.end())
        {
            return res->second;
        }
    }
    
    return "? " + (int)self._type;
}

void TocItemType::setIsParam(bool isParam)
{
    if(_isParam != isParam)
    {
        bool prevIsParam = _isParam;
        std::string currStrType = "";
        _isParam = isParam;
        if(prevIsParam)
        {
            for (auto element : PARAM_TYPES)
            {
                if (element.first == _type)
                {   
                    currStrType = element.second;
                    break;
                }
            }
            for (auto element : LOG_TYPES)
            {
                if (element.second == currStrType)
                {   
                    _type = element.first;
                    break;
                }
            }
        }
        else
        {
            for (auto element : LOG_TYPES)
            {
                if (element.first == _type)
                {   
                    currStrType = element.second;
                    break;
                }
            }
            for (auto element : PARAM_TYPES)
            {
                if (element.second == currStrType)
                {   
                    _type = element.first;
                    break;
                }
            }
        }
    }
}

TocItemType &TocItemType::operator=(const std::string &strType)
{
    if(_isParam)
    {
        for (auto element : PARAM_TYPES)
        {
            if (element.second == strType)
            {   
                _type = element.first;
                break;
            }
        }
    }
    else
    {
        for (auto element : LOG_TYPES)
        {
            if (element.second == strType)
            {   
                _type = element.first;
                break;
            }
        }
    }
    return *this;
}

TocItemType &TocItemType::operator=(const uint8_t &paramType)
{
    _type = paramType;
    return *this;
}

uint8_t TocItemType::size() const
{
    if(*this== "uint8_t" || *this== "int8_t")
        return 1;
    if(*this== "uint16_t" || *this== "int16_t"|| *this== "FP16")
        return 2;
    if(*this== "uint32_t" || *this== "int32_t"|| *this== "float")
        return 4;
    return 0;
}

TocInfo::TocInfo()
{
}

TocItem::~TocItem()
{
}
uint8_t TocItem::size() const
{
    return _type.size();
}

TocItem::TocItem()
{
}
TocItem::operator bool() const
{
    return !("" == _groupName  || "" == _name);
}

TocInfo::~TocInfo()
{
}
TocItem::TocItem(const TocItem &other)
{
    _groupName = other._groupName;
    _name = other._name;
    _type = other._type;
    _accessType = other._accessType;
    _id = other._id;
}
void Toc::insert(const TocItem &tocItem)
{
    _tocItems.insert({{tocItem._groupName, tocItem._name}, tocItem});
}
TocItem Toc::getItem(uint16_t id, bool caching ) const
{
    for(auto item : _tocItemsCache)
    {
        if(item.second._id == id)
        {
            return item.second;
        }
    }
    for(auto item : _tocItems)
    {
        if(item.second._id == id)
        {
            if(caching)
                const_cast<std::map<StrPair, TocItem> &>(_tocItemsCache).insert(item);

            return item.second;
        }
    }
    return TocItem();
}


TocItem Toc::getItem(const std::string &groupName, const std::string &paramName, bool caching) const
{
    auto res = _tocItemsCache.find({groupName, paramName});
    if (res == _tocItemsCache.end())
    {
        res = _tocItems.find({groupName, paramName});
        if(res == _tocItems.end())
            return TocItem();
        if (caching)
        {
            const_cast<std::map<StrPair, TocItem> &>(_tocItemsCache).insert(*res);
        }
    }
    return res->second;
}

uint16_t Toc::getItemId(const std::string &groupName, const std::string &paramName, bool caching) const
{
    return this->getItem(groupName, paramName, caching)._id;
}

std::vector<TocItem> Toc::getAllTocItems() const
{
    std::vector<TocItem> tocItemsVector;
    for (auto element : _tocItems)
    {
        TocItem tocItem = element.second;
        tocItemsVector.push_back(tocItem);
    }
    std::sort(tocItemsVector.begin(), tocItemsVector.end());
    return tocItemsVector;
}

std::vector<TocItem> Toc::getAllCachedTocItems() const
{
    std::vector<TocItem> tocItemsVector;
    for (auto element : _tocItemsCache)
    {
        TocItem tocItem = element.second;
        tocItemsVector.push_back(tocItem);
    }
    std::sort(tocItemsVector.begin(), tocItemsVector.end());
    return tocItemsVector;
}

void Toc::clearCache()
{
    _tocItemsCache.clear();
    _tocItems.clear();
}

void Toc::clearToc()
{
    _tocItemsCache.clear();
}

bool TocItemType::operator==(const std::string &val) const
{
    return std::string(*this) == val;
}

bool TocItemType::operator==(uint8_t val) const
{
    return _type == val;
}
TocItemType::operator std::string() const
{
    return to_string(*this);
}
TocItemType::TocItemType(uint8_t type, bool isParam) :_type(type),_isParam(isParam)
{
}
TocItemType::~TocItemType()
{}
TocItemType::TocItemType()
{}