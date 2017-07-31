#include <WCCOAozwHWMapper.hxx>
#include <WCCOAozwTrans.hxx>

#include <ErrHdl.hxx>         // error handler
#include <PVSSMacros.hxx>     // DEBUG macros

//--------------------------------------------------------------------------------
// We get new configs here. Create a new HW-Object on arrival and insert it.

PVSSboolean WCCOAozwHWMapper::addDpPa(DpIdentifier &dpId, PeriphAddr *confPtr)
{

  // We don't use Subindices here, so its simple.
  // Otherwise we had to look if we already have a HWObject and adapt its length.

  DEBUG_DRV_USR1("We got a new config... addDpPa called for " << confPtr->getName()<<" trans type requested is "<<confPtr->getTransformationType() );

  // We need to set up a transformation object to be used for this new DPElement.
  // Transformation is usually set in the PARA module (and in the address config) and it is expressed by a constant;
  // then, for standard driver types we have constants defined in Configs/ConfigTypes.hxx .
  //
  // For our convenience we will reuse the contants of Profibus (as they are used also by SIM driver)
  // as well as OPC-UA; one could use either of these in the para module until we have a proper panel.
  // This is easier than maintaining our own set of constants, or trying to extend the enum.
  //
  // Following the example driver, we will have only one transformation, WCCOAozwTrans,
  // yet it is nicely paramerizable - we could pass the type we want to serve in its constructor.
  // Then, it assumes that the hardware handling is done through strings, which is pretty
  // convenient for OpenZWave.
  //
  // Note that a lot of data handling is done through the HWObject class, which may have
  // different roles. 


// FOR REFERENCE (will not be used here) we have the following value types in OpenZWave:
//	ValueType_Bool 		Boolean, true or false
//	ValueType_Byte 		8-bit unsigned value
//	ValueType_Decimal 	Represents a non-integer value as a string, to avoid floating point accuracy issues.
//	ValueType_Int 		32-bit signed value
//	ValueType_List 		List from which one item can be selected
//	ValueType_Schedule 	Complex type used with the Climate Control Schedule command class
//	ValueType_Short 	16-bit signed value
//	ValueType_String 	Text string
//	ValueType_Button 	A write-only value that is the equivalent of pressing a button to send a command to a device
//	ValueType_Raw 		A collection of bytes


  // note that we still miss something for dyn-typed DPEs...

  switch ( confPtr->getTransformationType() )
  {
    case PbusTransVisibleType:
    case OpcUaTransStringType:
      confPtr->setTransform(new WCCOAozwTrans(TEXT_VAR));
      break;

    case PbusTransBooleanType:
    case OpcUaTransBooleanType:
      confPtr->setTransform(new WCCOAozwTrans(BIT_VAR));
      break;

    case PbusTransUInt8Type:
    case PbusTransUInt16Type:
    case OpcUaTransByteType:
      confPtr->setTransform(new WCCOAozwTrans(CHAR_VAR));
      break;


    case PbusTransUInt32Type:
    case OpcUaTransUInt32Type:
      confPtr->setTransform(new WCCOAozwTrans(UINTEGER_VAR));
      break;

    case PbusTransInt32Type:
    case OpcUaTransInt32Type:
      confPtr->setTransform(new WCCOAozwTrans(INTEGER_VAR));
      break;

    case PbusTransFloatType:
    case OpcUaTransFloatType:
      confPtr->setTransform(new WCCOAozwTrans(FLOAT_VAR)); 
      break;

    case PbusTransDateType:
    case PbusTransTimeType: 
    case PbusTransTimeDifferType: // note we get all three profibus constants
    case OpcUaTransDateTimeType:
      confPtr->setTransform(new WCCOAozwTrans(TIME_VAR)); 
      break;

    case OpcUaTransInt64Type:
      confPtr->setTransform(new WCCOAozwTrans(LONG_VAR)); 
      break;

    case OpcUaTransUInt64Type:
      confPtr->setTransform(new WCCOAozwTrans(ULONG_VAR)); 
      break;

    case PbusTransOctetType:
    case OpcUaTransByteStringType:
      confPtr->setTransform(new WCCOAozwTrans(BLOB_VAR)); 
      break;

    default :
      ErrHdl::error(
        ErrClass::PRIO_SEVERE,       // Severe error
        ErrClass::ERR_PARAM,         // It was a wrong type
        ErrClass::UNEXPECTEDSTATE,
        "WCCOAozwHWMapper", "addDpPa", // Class and function
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

  // Set our transformation
  hwObj->setType(confPtr->getTransform()->isA());

  // Set the len needed for data from _all_ subindices of this PVSS-Address.
  // Because we will deal with subix 0 only this is the Transformation::itemSize
  hwObj->setDlen(confPtr->getTransform()->itemSize());

  // Add it to the list
  addHWObject(hwObj);

  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------

PVSSboolean WCCOAozwHWMapper::clrDpPa(DpIdentifier &dpId, PeriphAddr *confPtr)
{
  DEBUG_DRV_USR1("clrDpPa called for address " << confPtr->getName()<<" for dpid "<<dpId);
  // Look up the instance of our HWObject on the internal list;
  // we create an object that is similar, and then use a lookup function
  HWObject adrObjToFind;
  adrObjToFind.setAddress(confPtr->getName()); 

  // Lookup HW-Object via the Name, not via the HW-Address
  // The class type isn't important here
  HWObject *hwObj = findHWAddr(&adrObjToFind);

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
