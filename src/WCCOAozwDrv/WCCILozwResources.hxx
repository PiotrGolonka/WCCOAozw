#ifndef WCCILozwRESOURCES_H_
#define WCCILozwRESOURCES_H_

// Our Resources class
// This class has two tasks:
//  - Interpret commandline and read config file
//  - Be an interface to internal datapoints

#include <DrvRsrce.hxx>

class WCCILozwResources : public DrvRsrce
{
  public:
    static void init(int &argc, char *argv[]); // Initialize statics
    static PVSSboolean readSection();          // read config file

    // Get our static Variables
    // TODO you likely have different things in the config file
    static unsigned short int getTCPPort() { return tcpPort; }

    // Get the number of names we need the DpId for
    virtual int getNumberOfDpNames();

    // TODO in this template we do not use internal DPs in the driver
    // If you need DPs, then also some other methods must be implemented

  private:
    // TODO you likely have different things in the config file
    static unsigned short int tcpPort;
};

#endif
