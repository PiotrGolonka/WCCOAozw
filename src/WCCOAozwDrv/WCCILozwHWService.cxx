#include <WCCILozwHWService.hxx>

#include <DrvManager.hxx>
#include <PVSSMacros.hxx>     // DEBUG macros

//--------------------------------------------------------------------------------
// called after connect to data

PVSSboolean WCCILozwHWService::initialize(int argc, char *argv[])
{
  // use this function to initialize internals
  // if you don't need it, you can safely remove the whole method
  // TODO

  // To stop driver return PVSS_FALSE
  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------
// called after connect to event

PVSSboolean WCCILozwHWService::start()
{
  // use this function to start your hardware activity.

  // TODO

  // return PVSS_FALSE on failure
  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------

void WCCILozwHWService::stop()
{
  // use this function to stop your hardware activity.

  // TODO
}

//--------------------------------------------------------------------------------

void WCCILozwHWService::workProc()
{
  HWObject obj;

  // TODO somehow receive a message from your device

  if ( 0 /* message received */ )
  {
    DEBUG_DRV_USR1("received message");

    // TODO if your message contains the timestamp use the following 2 lines
    obj.setOrgTime(TimeVar(/*seconds, milliSeconds*/));
    obj.setTimeOfPeriphFlag();

    // TODO else, if you don't read the timestamp, use current time
    obj.setOrgTime(TimeVar());  // current time

    CharString address;

    // TODO
    // put the address string into the HWObject so that the HWMapper can find
    // the corresponding HWObject (see below)
    obj.setAddress(address);

    // TODO
    // set the received data into the HWObject
    //obj.setData((PVSSchar*)data);
    //obj.setDlen(numberOfBytes);

    // a chance to see what's happening
    if ( Resources::isDbgFlag(Resources::DBG_DRV_USR1) )
      obj.debugPrint();

    // find the HWObject via the periphery address in the HWObject list,
    // e.g. for the low level old-new comparison
    HWObject *addrObj = DrvManager::getHWMapperPtr()->findHWObject(&obj);

    // ok, we found it; now send to the DPEs
    if ( addrObj )
      DrvManager::getSelfPtr()->toDp(&obj, addrObj);
  }
}

//--------------------------------------------------------------------------------
// we get data from PVSS and shall send it to the periphery

PVSSboolean WCCILozwHWService::writeData(HWObject *objPtr)
{
  if ( Resources::isDbgFlag(Resources::DBG_DRV_USR1) )
    objPtr->debugPrint();

  // TODO somehow send the data to your device

  // sending was successful
  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------
