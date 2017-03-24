#ifndef WCCOAozwHWMAPPER_H_
#define WCCOAozwHWMAPPER_H_

#include <HWMapper.hxx>

class WCCOAozwHWMapper : public HWMapper
{
  public:
    virtual PVSSboolean addDpPa(DpIdentifier &dpId, PeriphAddr *confPtr);
    virtual PVSSboolean clrDpPa(DpIdentifier &dpId, PeriphAddr *confPtr);
};

#endif
