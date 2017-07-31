#ifndef WCCOAozwHWSERVICE_H_
#define WCCOAozwHWSERVICE_H_

#include <HWService.hxx>

#include <DynVar.hxx>
#include <MappingVar.hxx>
#include <UIntegerVar.hxx>

#include <mutex>
#include <list>

#include <Manager.h>

class WCCOAozwInternalDp;

struct WCCOAozwValueChange {
    TimeVar ts;
    CharString addr;
    Variable* value;
};


struct ValueIDRecord
{
    uint8              nodeId;
    OpenZWave::ValueID valueId;
    void*              data;

    ValueIDRecord();
    static CharString ValueIdToAddress(OpenZWave::ValueID valueID);
    OpenZWave::ValueID::ValueType getType() {return valueId.GetType();}

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

   PVSSboolean handleNodeSpecialCommand(const char *sNodeId, const char *sCommand, const char *value);
   PVSSboolean handleNodeCommandClass(const CharString &address, const char *value);
  public:

    static uint32     g_homeId;
    static MappingVar s_nodes; // keys: IDs, values: query state
    static bool       s_nodesUpdated;
    static std::mutex s_nodesUpdatedMutex;

    static std::list<WCCOAozwValueChange>  s_changesToSend;// list of structures WCCOAozwValueChange
    static std::mutex                      s_changesMutex;

    static std::map<CharString,ValueIDRecord> s_addrToValues;

};

#endif
