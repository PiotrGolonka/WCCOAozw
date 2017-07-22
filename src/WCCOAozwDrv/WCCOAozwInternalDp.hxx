#ifndef _WCCOAozwInternalDp_H_
#define _WCCOAozwInternalDp_H_

#include <DrvIntDp.hxx>
#include <TextVar.hxx>

class WCCOAozwInternalDp : public DrvIntDp
{
  public:

    enum
    {
      inResetCounter,          // [IN] Reset counter
      inStartValue,            // [IN] Start value of counter
      // append additional inputs here
      maxConnect,              // No. of DPE we will connect to => will be used in the constructor
      outCounter = maxConnect, // [OUT] Message counter
      outStatus,
      nodesIds,
      nodesQueryStages,
      homeID,
      maxDP                    // Total no. of DPE => will be used in the constructor
    };


    WCCOAozwInternalDp();

    ~WCCOAozwInternalDp();

    /** retrievs the names of the internal DPEs. The function must be overloaded.
        It should return the proper DPE string for the corresponding index */
    virtual const CharString& getDpName4Query(int index);

    /** called if there is an answer to a certain index */
    virtual void answer4DpId(int index, Variable* varPtr);

    /** called if there is a hotlink to a certain index */
    virtual void hotLink2Internal(int index, Variable* varPtr);

    static void setStatusText(const CharString &statusText);
    static void updateHomeId(unsigned theHomeId);
    static void updateNodesIds  ();

  private:
    static WCCOAozwInternalDp* theIDP; // pointer to the one and only instance of the IDP

};

#endif



