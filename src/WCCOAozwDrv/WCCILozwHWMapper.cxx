#include <WCCILozwHWMapper.hxx>
#include <WCCILozwTrans.hxx>

#include <ErrHdl.hxx>         // error handler
#include <PVSSMacros.hxx>     // DEBUG macros

//--------------------------------------------------------------------------------
// We get new configs here. Create a new HW-Object on arrival and insert it.

PVSSboolean WCCILozwHWMapper::addDpPa(DpIdentifier &dpId, PeriphAddr *confPtr)
{
  // We don't use Subindices here, so its simple.
  // Otherwise we had to look if we already have a HWObject and adapt its length.

  DEBUG_DRV_USR1("addDpPa called for " << confPtr->getName());

  // tell the config how we will transform data to/from the device
  // by installing a Transformation object into the PeriphAddr
  // In this template, the Transformation type was set via the
  // configuration panel (it is already set in the PeriphAddr)

  // TODO this really depends on your protocol and is therefore just an example
  // in this example we use the ones from Pbus, as those can be selected
  // with the SIM driver parametrization panel
  switch ( confPtr->getTransformationType() )
  {
    case PbusTransVisibleType:
      confPtr->setTransform(new WCCILozwTrans(TEXT_VAR));
      break;

    case PbusTransInt32Type:
      confPtr->setTransform(new WCCILozwTrans(INTEGER_VAR));
      break;

    case PbusTransFloatType:
      confPtr->setTransform(new WCCILozwTrans(FLOAT_VAR));
      break;

    default :
      ErrHdl::error(
        ErrClass::PRIO_SEVERE,       // Severe error
        ErrClass::ERR_PARAM,         // It was a wrong type
        ErrClass::UNEXPECTEDSTATE,
        "WCCILozwHWMapper", "addDpPa", // Class and function
        CharString("Illegal transformation type ") +
        CharString((int)confPtr->getTransformationType())
      );

      // But add the config !
      return HWMapper::addDpPa(dpId, confPtr);
  }

  // First add the config, then the HW-Object
  if ( !HWMapper::addDpPa(dpId, confPtr) )  // FAILED !!
    return PVSS_FALSE;

  HWObject *hwObj = new HWObject;
  // Set Address and Subindex
  hwObj->setConnectionId(confPtr->getConnectionId());
  hwObj->setAddress(confPtr->getName());       // Resolve the HW-Address, too

  // Set the data type.
  hwObj->setType(confPtr->getTransform()->isA());

  // Set the len needed for data from _all_ subindices of this PVSS-Address.
  // Because we will deal with subix 0 only this is the Transformation::itemSize
  hwObj->setDlen(confPtr->getTransform()->itemSize());

  // Add it to the list
  addHWObject(hwObj);

  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------

PVSSboolean WCCILozwHWMapper::clrDpPa(DpIdentifier &dpId, PeriphAddr *confPtr)
{
  DEBUG_DRV_USR1("clrDpPa called for " << confPtr->getName());

  // Find our HWObject via a template
  HWObject adrObj;
  adrObj.setAddress(confPtr->getName());

  // Lookup HW-Object via the Name, not via the HW-Address
  // The class type isn't important here
  HWObject *hwObj = findHWAddr(&adrObj);

  if ( hwObj )
  {
    // Object exists, remove it from the list and delete it.
    clrHWObject(hwObj);
    delete hwObj;
  }

  // Call function of base class to remove config
  return HWMapper::clrDpPa(dpId, confPtr);
}

//--------------------------------------------------------------------------------
