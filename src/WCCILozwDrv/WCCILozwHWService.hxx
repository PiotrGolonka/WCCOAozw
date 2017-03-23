#ifndef WCCILozwHWSERVICE_H_
#define WCCILozwHWSERVICE_H_

#include <HWService.hxx>

class WCCILozwHWService : public HWService
{
  public:
    virtual PVSSboolean initialize(int argc, char *argv[]);
    virtual PVSSboolean start();
    virtual void stop();
    virtual void workProc();
    virtual PVSSboolean writeData(HWObject *objPtr);
};

#endif
