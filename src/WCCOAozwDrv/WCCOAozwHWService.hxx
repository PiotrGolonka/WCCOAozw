#ifndef WCCOAozwHWSERVICE_H_
#define WCCOAozwHWSERVICE_H_

#include <HWService.hxx>

#include <DynVar.hxx>
#include <MappingVar.hxx>
#include <UIntegerVar.hxx>

#include <mutex>
#include <list>

class WCCOAozwInternalDp;

struct WCCOAozwValueChange {
    TimeVar ts;
    CharString addr;
    Variable* value;
};

class WCCOAozwHWService : public HWService
{
  public:
    virtual PVSSboolean initialize(int argc, char *argv[]);
    virtual PVSSboolean start();
    virtual void stop();
    virtual void workProc();
    virtual PVSSboolean writeData(HWObject *objPtr);

  private:
    static void unlockOnSignal();
   friend class WCCOAozwDrv;


  public:
    static MappingVar s_nodes; // keys: IDs, values: query state
    static bool       s_nodesUpdated;
    static std::mutex s_nodesUpdatedMutex;

    static std::list<WCCOAozwValueChange>  s_changesToSend;// list of structures WCCOAozwValueChange
    static std::mutex                      s_changesMutex;

};

#endif
