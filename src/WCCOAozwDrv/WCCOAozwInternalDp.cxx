#include "WCCOAozwInternalDp.hxx"
#include "WCCOAozwResources.hxx"
#include "WCCOAozwHWService.hxx"
#include "WCCOAozwDrv.hxx"

#include <DpIdValueList.hxx>

#include <Manager.h>

#include <assert.h>

WCCOAozwInternalDp* WCCOAozwInternalDp::theIDP=0; // at the beginning, it is uninitialized

WCCOAozwInternalDp::WCCOAozwInternalDp()
  : DrvIntDp(maxDP, maxConnect) // note that maxDP and maxConnect are declared in the enum
{

    assert (theIDP==0); // make sure that only one instance is ever created
    theIDP=this;
}

WCCOAozwInternalDp::~WCCOAozwInternalDp()
{
    theIDP=0;
}


void WCCOAozwInternalDp::setStatusText(const CharString &statusText)
{
      DpIdentifier dpId = theIDP->getId(outStatus);
      TextVar txt(statusText);
      Manager::dpSet(dpId, txt);
}


void WCCOAozwInternalDp::updateHomeId(unsigned theHomeID)
{
      DpIdentifier dpId = theIDP->getId(homeID);
      UIntegerVar hid(theHomeID);
      Manager::dpSet(dpId, hid);
}


void WCCOAozwInternalDp::updateNodesIds()
{
    if (WCCOAozwHWService::s_nodesUpdated) {
        DpIdValueList dpl;
	{// additional scope to get the mutex
//	    std::cout<<"Sending new nodes "<<std::endl;
	    std::lock_guard<std::mutex> guard(WCCOAozwHWService::s_nodesUpdatedMutex);

	    // we wanted a sorted list...
	    DynVar ids(UINTEGER_VAR);
	    DynVar qs(TEXT_VAR);
	    for (unsigned i=0;i<WCCOAozwHWService::s_nodes.getNumberOfItems();i++) {
		ids.append(*(WCCOAozwHWService::s_nodes.getKey(i)));
	    }
	    ids.unique(); // just in case
	    ids.sort();
	    // note that we iterate over a dyn- here; index from one!
	    for (unsigned i=1;i<=ids.getNumberOfItems();i++) {
		qs.append( *(WCCOAozwHWService::s_nodes [ *(ids[i]) ] ));
	    }

	    // note that appendItem does the deep copy
    	    dpl.appendItem(theIDP->getId(nodesIds),ids);
    	    dpl.appendItem(theIDP->getId(nodesQueryStages),qs);

	    WCCOAozwHWService::s_nodesUpdated=false;
	}
	Manager::dpSet(dpl);
    }
}



const CharString& WCCOAozwInternalDp::getDpName4Query(int index)
{
  static CharString str("Uknown_OZW_DP_Element");

  switch (index)
  {
  case inRequestNodeData:	str = WCCOAozwResources::ozwDrviverInternalDpName + ".NodeQuery.NodeId:_original.._value"; break;
  case inCommandGet:		str = WCCOAozwResources::ozwDrviverInternalDpName + ".Command.Get:_original.._value"; break;
  case inCommandSet:		str = WCCOAozwResources::ozwDrviverInternalDpName + ".Command.Set:_original.._value"; break;
  case outNodeData:       	str = WCCOAozwResources::ozwDrviverInternalDpName + ".NodeQuery.Data:_original.._value";   break;
  case outStatus:         	str = WCCOAozwResources::ozwDrviverInternalDpName + ".Status:_original.._value";           break;
  case outCmdClassesIds:  	str = WCCOAozwResources::ozwDrviverInternalDpName + ".NodeQuery.CmdClasses.Ids:_original.._value";break;
  case outCmdClassesNames:  	str = WCCOAozwResources::ozwDrviverInternalDpName + ".NodeQuery.CmdClasses.Names:_original.._value";break;
  case outCmdClassesVersions:	str = WCCOAozwResources::ozwDrviverInternalDpName + ".NodeQuery.CmdClasses.Versions:_original.._value";break;
  case outCommandResult:	str = WCCOAozwResources::ozwDrviverInternalDpName + ".Command.Result:_original.._value"; break;
  case nodesIds:          	str = WCCOAozwResources::ozwDrviverInternalDpName + ".Nodes.Ids:_original.._value";        break;
  case nodesQueryStages:  	str = WCCOAozwResources::ozwDrviverInternalDpName + ".Nodes.QueryStages:_original.._value";break;
  case homeID :                 str = WCCOAozwResources::ozwDrviverInternalDpName + ".HomeID:_original.._value";    break;
    // Default not needed, but make compiler happy
  default :
    ;
  }

  return str;
}

void WCCOAozwInternalDp::answer4DpId(int index, Variable* varPtr)
{
    std::cerr<<"Got answer4DPID at index "<<index<<","<<varPtr<<std::endl;
    delete varPtr;
/*
  // if we get a start value initialise the counter
  if (index  == inStartValue)
  {
    if (varPtr->isA() == UINTEGER_VAR)
      ((SampleHWService*)SampleDrvManager::getHWServicePtr())->setCounter(((UIntegerVar*)varPtr)->getValue());
  }
  delete varPtr;
*/
}


void WCCOAozwInternalDp::serveNodeDataRequest(unsigned nodeId)
{
//    DpIdentifier dpIdResult = getId(outNodeData);
    unsigned homeId=WCCOAozwHWService::g_homeId;
    DynVar nodeData(TEXT_VAR);
    char buf[65];

    snprintf (buf,64,"ID:%i",nodeId);
    nodeData.append(TextVar(buf));

    snprintf(buf,64,"MANUFNAME:%s",OpenZWave::Manager::Get()->GetNodeManufacturerName(homeId,nodeId).c_str());
    nodeData.append(TextVar(buf));

    snprintf(buf,64,"PRODUCTNAME:%s",OpenZWave::Manager::Get()->GetNodeProductName(homeId,nodeId).c_str());
    nodeData.append(TextVar(buf));

    snprintf(buf,64,"QUERYSTAGE:%s",OpenZWave::Manager::Get()->GetNodeQueryStage(homeId,nodeId).c_str());
    nodeData.append(TextVar(buf));

    if (OpenZWave::Manager::Get()->IsNodeListeningDevice(homeId,nodeId)) nodeData.append(TextVar("LISTENING:TRUE"));
    else nodeData.append(TextVar("LISTENING:FALSE"));

    // now get the command classes;
    DynVar cmdClassIds(UINTEGER_VAR);
    DynVar cmdClassNames(TEXT_VAR);
    DynVar cmdClassVersions(UINTEGER_VAR);

    for (unsigned i=1;i<=255;i++) { // we start at 1, because we do not need COMMAND_CLASS_NO_OPERATION (==0)
	string className;
	uint8 classVersion;
	if (OpenZWave::Manager::Get()->GetNodeClassInformation(homeId,nodeId,i,&className,&classVersion)) {
	    cmdClassIds.append(UIntegerVar(i));
	    cmdClassNames.append(TextVar(className.c_str()));
	    cmdClassVersions.append(UIntegerVar(classVersion));
	}
    }

//Manager::dpSet(dpIdResult,nodeData);
    DpIdValueList dpl;
    dpl.appendItem(getId(outNodeData),nodeData);
    dpl.appendItem(getId(outCmdClassesIds),cmdClassIds);
    dpl.appendItem(getId(outCmdClassesNames),cmdClassNames);
    dpl.appendItem(getId(outCmdClassesVersions),cmdClassVersions);

    Manager::dpSet(dpl);

}

void WCCOAozwInternalDp::serveCmdGetRequest(const TextVar &addr)
{

  DpIdentifier dpIdResult = getId(outCommandResult);

  auto it=WCCOAozwHWService::s_addrToValues.find(addr);

  if (it==WCCOAozwHWService::s_addrToValues.end()) {
     ErrHdl::error(
        ErrClass::PRIO_SEVERE,             // Data will be lost
        ErrClass::ERR_PARAM,               // Wrong parametrization
        ErrClass::UNEXPECTEDSTATE,         // Nothing else appropriate
        "WCCOAozwInternalDp::serveCmdGetRequest",      // File and function name
        "Unrecognized address",                // Message
        addr                       // faulty address
    );
    Manager::dpSet(dpIdResult,TextVar("WRONG_ADDRESS"));
    return;

  }

  OpenZWave::ValueID &valueId=it->second.valueId;

  std::string result;
  OpenZWave::Manager::Get()->GetValueAsString(valueId,&result);


  Manager::dpSet(dpIdResult,TextVar(result.c_str()));

}

void WCCOAozwInternalDp::serveCmdSetRequest(const TextVar &addr, const TextVar &value)
{
    printf("WCCOAozwInternalDp::serveCmdSetRequest NOT IMPLEMENTED\n");
    DpIdentifier dpIdResult = getId(outCommandResult);
    Manager::dpSet(dpIdResult,TextVar("COMMANDS_AND_SETTINGS_NOT_IMPLEMENTED"));

}



// Called when a DPSet to internal datapoint was actioned...
void WCCOAozwInternalDp::hotLink2Internal(int index, Variable* varPtr)
{
    std::cerr<<"Detected IDP dpset at index "<<index<<","<<varPtr<<std::endl;
      // Send new value
      DpIdentifier dpId = getId(outStatus);
      TextVar statusText("Hello at index ");
      statusText+=index;
      statusText+="=>";
      statusText+=(varPtr->formatValue());
      Manager::dpSet(dpId, statusText);



    switch (index)
    {
	case inRequestNodeData:
	  {
	    UIntegerVar nodeId=(*(UIntegerVar*)varPtr);
	    serveNodeDataRequest(nodeId);
          }
	  break;

	case inCommandGet: 
	  {
	    TextVar addr=(*(TextVar*)varPtr);
	    serveCmdGetRequest(addr);
	  }
	  break;

	case inCommandSet: 
	  {
	    DynVar data=(*(DynVar*)varPtr);
	    TextVar& addr  = * (TextVar*)data[1];
	    TextVar& params= * (TextVar*)data[2];
	    serveCmdSetRequest(addr,params);
	  }
	  break;
	default:
	  break;
    }

  

/*
  switch (index)
  {
  case inResetCounter :
    // Debug-output
    if (Resources::isDbgFlag(Resources::DBG_WORK))
      std::cerr << "Got Hotlink: Reset counter" << std::endl;

    // Check for var-type.
    // If there is nothing serious wrong it must be a BIT_VAR
    // But we neglect the value here
    if (varPtr->isA() == BIT_VAR)
    {
      ((SampleHWService*)SampleDrvManager::getHWServicePtr())->setCounter(0);

      // Send new value
      DpIdentifier dpId = getId(outCounter);

      // The variable type shall match, there is no outomatic conversion !
      UIntegerVar  counterVar(0);

      // Call play Manager::dpSet
      Manager::dpSet(dpId, counterVar);
    }
    break;
  case inStartValue:

    if (varPtr->isA() == UINTEGER_VAR)
      ((SampleHWService*)SampleDrvManager::getHWServicePtr())->setCounter(((UIntegerVar*)varPtr)->getValue());

    break;

  default :
    // Cannot happen, just make some compilers happy.
    // Therefore don't even print an error message :)
    break;
  }
  delete varPtr;
*/
}

