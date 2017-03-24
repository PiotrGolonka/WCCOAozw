#ifndef WCCOAozwHWSERVICE_H_
#define WCCOAozwHWSERVICE_H_

#include <HWService.hxx>

class WCCOAozwHWService : public HWService
{
  public:
    virtual PVSSboolean initialize(int argc, char *argv[]);
    virtual PVSSboolean start();
    virtual void stop();
    virtual void workProc();
    virtual PVSSboolean writeData(HWObject *objPtr);
};

#endif
