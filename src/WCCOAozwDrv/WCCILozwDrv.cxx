#include <WCCILozwDrv.hxx>
#include <WCCILozwHWMapper.hxx>
#include <WCCILozwHWService.hxx>

#include <HWObject.hxx>

//------------------------------------------------------------------------------------

void WCCILozwDrv::install_HWMapper()
{
  hwMapper = new WCCILozwHWMapper;
}

//--------------------------------------------------------------------------------

void WCCILozwDrv::install_HWService()
{
  hwService = new WCCILozwHWService;
}

//--------------------------------------------------------------------------------

HWObject * WCCILozwDrv::getHWObject() const
{
  return new HWObject();
}

//--------------------------------------------------------------------------------

void WCCILozwDrv::install_AlertService()
{
  DrvManager::install_AlertService();
}
