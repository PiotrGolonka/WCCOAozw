#include <WCCILozwAlertService.hxx>
#include <AlertObject.hxx>
#include <HWObject.hxx>
#include <WCCILozwResources.hxx>

//----------------------------------------------------------------------------------
WCCILozwAlertService::WCCILozwAlertService()
{
}

//----------------------------------------------------------------------------------
WCCILozwAlertService::~WCCILozwAlertService()
{
}

//---------------------------------------------------------------------------------------------
PVSSboolean WCCILozwAlertService::ackAlertInHW(AlertObject *alertPtr, HWObject *objPtr)
{
  if (Resources::isDbgFlag(DrvRsrce::DBG_DRV_ALERT))
  {
      cerr << "got alert ack request for HW " << objPtr->getAddress() << endl;
      alertPtr->debugPrint();
      cerr << "----------------------------------------------" << endl;
  }

  alertPtr->setEvent(AlertObject::EA_ACK);
  alertPtr->setComment("simulated ACK from PLC");
  setAlert(alertPtr, objPtr);

  return PVSS_TRUE;
}


void WCCILozwAlertService::ackConfirmCB(const AlertObject *alertPtr, const HWObject *adrPtr)
{
  if (Resources::isDbgFlag(DrvRsrce::DBG_DRV_ALERT))
  {
      cerr << "got ack confirm CB for HW " << adrPtr->getAddress() << endl;
      alertPtr->debugPrint();
      cerr << "----------------------------------------------" << endl;
  }
}

void WCCILozwAlertService::changeNotificationCB(const AlertObject *alertPtr, const HWObject *adrPtr)
{
  if (Resources::isDbgFlag(DrvRsrce::DBG_DRV_ALERT))
  {
      cerr << "got change notification CB for HW " << adrPtr->getAddress() << endl;
      alertPtr->debugPrint();
      cerr << "----------------------------------------------" << endl;
  }
}

void WCCILozwAlertService::invisibleConfirmCB(const AlertObject *alertPtr, const HWObject *adrPtr)
{
  if (Resources::isDbgFlag(DrvRsrce::DBG_DRV_ALERT))
  {
      cerr << "got invisible confirm CB for HW " << adrPtr->getAddress() << endl;
      alertPtr->debugPrint();
      cerr << "----------------------------------------------" << endl;
  }
}
