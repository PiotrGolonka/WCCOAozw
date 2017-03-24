#ifndef WCCILozwHWMAPPER_H_
#define WCCILozwHWMAPPER_H_

#include <HWMapper.hxx>

class WCCILozwHWMapper : public HWMapper
{
  public:
    virtual PVSSboolean addDpPa(DpIdentifier &dpId, PeriphAddr *confPtr);
    virtual PVSSboolean clrDpPa(DpIdentifier &dpId, PeriphAddr *confPtr);
};

#endif
