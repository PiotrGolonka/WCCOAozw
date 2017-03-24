#ifndef WCCILozwDRV_H_
#define WCCILozwDRV_H_

#include <DrvManager.hxx>

class WCCILozwDrv : public DrvManager
{
  public:
    virtual void install_HWMapper();
    virtual void install_HWService();
    virtual void install_AlertService();

    virtual HWObject *getHWObject() const; 
};

#endif
