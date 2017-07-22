#ifndef WCCOAozwRESOURCES_H_
#define WCCOAozwRESOURCES_H_

// Our Resources class
// This class has two tasks:
//  - Interpret commandline and read config file
//  - Be an interface to internal datapoints


#include <DrvRsrce.hxx>
#include <CharString.hxx>
#include <HmiConnection.hxx>

#include "WCCOAozwInternalDp.hxx"

class WCCOAozwResources : public DrvRsrce
{
  public:
    static void init(int &argc, char *argv[]); // Initialize statics
    static PVSSboolean readSection();          // read config file

    virtual HmiConnection *getNewHmiConnection(DpIdType id) const;

    // Get our static Variables
    // TODO you likely have different things in the config file
    static unsigned short int getTCPPort() { return tcpPort; }
    static const CharString& getZwaveInterface() {return zwaveInterface;}

    static CharString ozwDrviverInternalDpName;

    PVSSboolean isReadyToConnect ();
    const DpIdentifier & getInternalDpId(int idx);

    // Get the number of names we need the DpId for
    //virtual int getNumberOfDpNames();

  private:
    static unsigned short int tcpPort;
    static CharString zwaveInterface;
  private:
    WCCOAozwInternalDp idp;
};
#endif
