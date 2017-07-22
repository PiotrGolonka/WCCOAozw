#include <WCCOAozwDrv.hxx>
#include <WCCOAozwHWMapper.hxx>
#include <WCCOAozwHWService.hxx>

#include <HWObject.hxx>

//------------------------------------------------------------------------------------

void WCCOAozwDrv::install_HWMapper()
{
  hwMapper = new WCCOAozwHWMapper;
}

//--------------------------------------------------------------------------------

void WCCOAozwDrv::install_HWService()
{
  hwService = new WCCOAozwHWService;
}

//--------------------------------------------------------------------------------

HWObject * WCCOAozwDrv::getHWObject() const
{
  return new HWObject();
}

//--------------------------------------------------------------------------------

void WCCOAozwDrv::install_AlertService()
{
  DrvManager::install_AlertService();
}

void WCCOAozwDrv::onShutdownSignal(int sig)
{
    printf(" received signal %i\n",sig);
    WCCOAozwHWService::unlockOnSignal();
    Manager::sigHdl(sig);
}