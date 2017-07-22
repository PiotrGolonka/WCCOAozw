#include "WCCOAozwInternalDp.hxx"
#include "WCCOAozwResources.hxx"
#include "WCCOAozwHWService.hxx"
#include "WCCOAozwDrv.hxx"

#include <DpIdValueList.hxx>

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
	    std::cout<<"Sending new nodes "<<std::endl;
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
  case inResetCounter :
    str = WCCOAozwResources::ozwDrviverInternalDpName + ".ResetCounter:_original.._value";
    break;
  case inStartValue :
    str = WCCOAozwResources::ozwDrviverInternalDpName + ".StartValue:_original.._value";
    break;
  case outCounter :
    str = WCCOAozwResources::ozwDrviverInternalDpName + ".Counter:_original.._value";
    break;
  case outStatus :
    str = WCCOAozwResources::ozwDrviverInternalDpName + ".Status:_original.._value";
    break;
  case nodesIds :
    str = WCCOAozwResources::ozwDrviverInternalDpName + ".Nodes.Ids:_original.._value";
    break;
  case nodesQueryStages :
    str = WCCOAozwResources::ozwDrviverInternalDpName + ".Nodes.QueryStages:_original.._value";
    break;
  case homeID :
    str = WCCOAozwResources::ozwDrviverInternalDpName + ".HomeID:_original.._value";
    break;
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


void WCCOAozwInternalDp::hotLink2Internal(int index, Variable* varPtr)
{
    std::cerr<<"Got hotlink at index "<<index<<","<<varPtr<<std::endl;
      // Send new value
      DpIdentifier dpId = getId(outStatus);
      TextVar statusText("Hello at index ");
      statusText+=index;
      statusText+="=>";
      statusText+=(varPtr->formatValue());
      Manager::dpSet(dpId, statusText);



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

