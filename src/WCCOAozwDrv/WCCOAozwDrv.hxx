#ifndef WCCOAozwDRV_H_
#define WCCOAozwDRV_H_

#include <DrvManager.hxx>

class WCCOAozwInternalDp;

class WCCOAozwDrv : public DrvManager
{
  public:
    virtual void install_HWMapper();
    virtual void install_HWService();
    virtual void install_AlertService();

    virtual HWObject *getHWObject() const; 

    static void onShutdownSignal(int signum);

  private:
};

#endif
