#include <DrvManager.hxx>
#include <PVSSMacros.hxx>

#include "WCCOAozwHWService.hxx"
#include "WCCOAozwResources.hxx"
#include "WCCOAozwInternalDp.hxx"

#include <DynVar.hxx>
#include <MappingVar.hxx>
#include <UIntegerVar.hxx>



// These are from OpenZwave (mind an almost-conflict in names... luckily we have a namespace)
#include <Manager.h>
#include <Options.h>
#include <Driver.h>

#include <Node.h>
#include <Group.h>
#include <Notification.h>
#include <value_classes/ValueStore.h>
#include <value_classes/Value.h>
#include <value_classes/ValueBool.h>
#include <platform/Log.h>

static pthread_mutex_t g_criticalSection;
static pthread_cond_t  initCond  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;
static uint32 g_homeId;
static bool   g_initFailed = false;

MappingVar WCCOAozwHWService::s_nodes;
bool       WCCOAozwHWService::s_nodesUpdated=false;
std::mutex WCCOAozwHWService::s_nodesUpdatedMutex;


std::list<WCCOAozwValueChange>  WCCOAozwHWService::s_changesToSend;
std::mutex                      WCCOAozwHWService::s_changesMutex;

void OnNotification( OpenZWave::Notification const* _notification, void* _context)
{


//WCCOAozwHWService::idp->setStatusText("Got nofification");
//    printf("Got OnNotification\n");
    // Must do this inside a critical section to avoid conflicts with the main thread
    pthread_mutex_lock( &g_criticalSection );

//    std::cout<<_notification->GetAsString()<<std::endl;

    switch( _notification->GetType() ){ 
               case OpenZWave::Notification::Type_DriverReady:
                {
                        g_homeId = _notification->GetHomeId();
			printf ("driver ready with home id %i\n",g_homeId);
			WCCOAozwInternalDp::setStatusText(CharString("Driver ready with homeID ")+CharString(g_homeId));
			WCCOAozwInternalDp::updateHomeId(g_homeId);
                        break;
                }

                case OpenZWave::Notification::Type_DriverFailed:
                {
			printf("driver failed\n");
			WCCOAozwInternalDp::setStatusText("Driver failed");
                        g_initFailed = true;
                        pthread_cond_broadcast(&initCond);
                        break;
                }
                
                case OpenZWave::Notification::Type_AwakeNodesQueried:
                {
			printf("\n\n --- awake nodes queried OK --- \n\n");
			WCCOAozwInternalDp::setStatusText("Awake Nodes queried OK");
                        pthread_cond_broadcast(&initCond);
                        break;
                }
                case OpenZWave::Notification::Type_AllNodesQueried:
                {
			printf("\n\n --- All nodes queried OK --- \n\n");
			WCCOAozwInternalDp::setStatusText("Nodes queried OK");
                        pthread_cond_broadcast(&initCond);
                        break;
                }
                case OpenZWave::Notification::Type_AllNodesQueriedSomeDead:
                {
			printf("\n\n --- All nodes queried some dead --- \n\n");
			WCCOAozwInternalDp::setStatusText("Nodes queried, some dead");
                        pthread_cond_broadcast(&initCond);
                        break;
                }

		case OpenZWave::Notification::Type_NodeAdded:
		{
		    std::cout<<"Node added"<<(int)_notification->GetNodeId()<<std::endl;
		    std::cout<<" -> Stage:"<<OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId())<<" / ";
		    std::cout<<" -> Product:"<<OpenZWave::Manager::Get()->GetNodeProductName(g_homeId,_notification->GetNodeId())<<" / ";
		    std::cout<<" -> Type:"<<OpenZWave::Manager::Get()->GetNodeType(g_homeId,_notification->GetNodeId())<<std::endl;
		    std::lock_guard<std::mutex> guard(WCCOAozwHWService::s_nodesUpdatedMutex);
		    WCCOAozwHWService::s_nodes.setAt(UIntegerVar(_notification->GetNodeId()), TextVar("ADDED"));
		    WCCOAozwHWService::s_nodesUpdated=true;


		    std::lock_guard<std::mutex> guardVC(WCCOAozwHWService::s_changesMutex);
		    WCCOAozwValueChange vc;
		    vc.ts=TimeVar();
		    vc.addr=CharString(_notification->GetNodeId())+".QS";
		    vc.value=new TextVar("ADDED");
		    WCCOAozwHWService::s_changesToSend.push_back(vc);
		    break;
		}
		case OpenZWave::Notification::Type_NodeNew:
		{
		    std::cout<<"Node NEW"<<(int)_notification->GetNodeId()<<std::endl;
		    std::cout<<" -> Stage:"<<OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId())<<" / ";
		    std::cout<<" -> Product:"<<OpenZWave::Manager::Get()->GetNodeProductName(g_homeId,_notification->GetNodeId())<<" / ";
		    std::cout<<" -> Type:"<<OpenZWave::Manager::Get()->GetNodeType(g_homeId,_notification->GetNodeId())<<std::endl;
		    std::lock_guard<std::mutex> guard(WCCOAozwHWService::s_nodesUpdatedMutex);
		    WCCOAozwHWService::s_nodes.setAt(UIntegerVar(_notification->GetNodeId()), TextVar("NEW"));
		    WCCOAozwHWService::s_nodesUpdated=true;

		    std::lock_guard<std::mutex> guardVC(WCCOAozwHWService::s_changesMutex);
		    WCCOAozwValueChange vc;
		    vc.ts=TimeVar();
		    vc.addr=CharString(_notification->GetNodeId())+".QS";
		    vc.value=new TextVar(OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId()).c_str());
		    WCCOAozwHWService::s_changesToSend.push_back(vc);


		    break;
		}
		case OpenZWave::Notification::Type_NodeProtocolInfo:
		{
		    std::cout<<"Node protocol info complete "<<(int)_notification->GetNodeId()<<std::endl;
		    std::cout<<" -> Stage:"<<OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId())<<" / ";
		    std::cout<<" -> Product:"<<OpenZWave::Manager::Get()->GetNodeProductName(g_homeId,_notification->GetNodeId())<<" / ";
		    std::cout<<" -> Type:"<<OpenZWave::Manager::Get()->GetNodeType(g_homeId,_notification->GetNodeId())<<std::endl;
		    std::lock_guard<std::mutex> guard(WCCOAozwHWService::s_nodesUpdatedMutex);
		    WCCOAozwHWService::s_nodes.setAt(UIntegerVar(_notification->GetNodeId()),  TextVar(OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId()).c_str()));
		    WCCOAozwHWService::s_nodesUpdated=true;


		    std::lock_guard<std::mutex> guardVC(WCCOAozwHWService::s_changesMutex);
		    WCCOAozwValueChange vc;
		    vc.ts=TimeVar();
		    vc.addr=CharString(_notification->GetNodeId())+".QS";
		    vc.value=new TextVar(OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId()).c_str());
		    WCCOAozwHWService::s_changesToSend.push_back(vc);
		    break;
		}
                case OpenZWave::Notification::Type_EssentialNodeQueriesComplete:
		{
		    std::cout<<"Essential Node queries complete "<<(int)_notification->GetNodeId()<<std::endl;
		    std::cout<<" -> Stage:"<<OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId())<< " / ";
		    std::cout<<" -> Product:"<<OpenZWave::Manager::Get()->GetNodeProductName(g_homeId,_notification->GetNodeId());
		    std::cout<<std::endl;
		    std::lock_guard<std::mutex> guard(WCCOAozwHWService::s_nodesUpdatedMutex);
		    WCCOAozwHWService::s_nodes.setAt(UIntegerVar(_notification->GetNodeId()),  TextVar(OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId()).c_str()));
		    WCCOAozwHWService::s_nodesUpdated=true;

		    std::lock_guard<std::mutex> guardVC(WCCOAozwHWService::s_changesMutex);
		    WCCOAozwValueChange vc;
		    vc.ts=TimeVar();
		    vc.addr=CharString(_notification->GetNodeId())+".QS";
		    vc.value=new TextVar(OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId()).c_str());
		    WCCOAozwHWService::s_changesToSend.push_back(vc);
		    break;
		}
                case OpenZWave::Notification::Type_NodeQueriesComplete:
		{
		    std::cout<<"ALL Node queries complete "<<(int)_notification->GetNodeId()<<std::endl;
		    std::cout<<" -> Stage:"<<OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId())<< " / ";
		    std::cout<<" -> Product:"<<OpenZWave::Manager::Get()->GetNodeProductName(g_homeId,_notification->GetNodeId());
		    std::cout<<std::endl;
		    std::lock_guard<std::mutex> guard(WCCOAozwHWService::s_nodesUpdatedMutex);
		    WCCOAozwHWService::s_nodes.setAt(UIntegerVar(_notification->GetNodeId()),  TextVar(OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId()).c_str()));
		    WCCOAozwHWService::s_nodesUpdated=true;

		    std::lock_guard<std::mutex> guardVC(WCCOAozwHWService::s_changesMutex);
		    WCCOAozwValueChange vc;
		    vc.ts=TimeVar();
		    vc.addr=CharString(_notification->GetNodeId())+".QS";
		    vc.value=new TextVar(OpenZWave::Manager::Get()->GetNodeQueryStage(g_homeId,_notification->GetNodeId()).c_str());
		    WCCOAozwHWService::s_changesToSend.push_back(vc);
		    break;
		}
                case OpenZWave::Notification::Type_NodeRemoved:
		{
		    std::cout<<"Node removed "<<(int)_notification->GetNodeId()<<std::endl;
		    std::cout<<std::endl;
		    std::lock_guard<std::mutex> guard(WCCOAozwHWService::s_nodesUpdatedMutex);
		    Variable *removedItem=WCCOAozwHWService::s_nodes.cutAt(UIntegerVar(_notification->GetNodeId()));
		    if (removedItem) delete removedItem;
		    WCCOAozwHWService::s_nodesUpdated=true;
		    std::cout<<"WE SHOULD PROBABLY UNREGISTER THE ADDRESS SOMEHOW?"<<std::endl;
		    break;
		}

		case OpenZWave::Notification::Type_ValueAdded:
		{
		    string sVal;
		    sVal.reserve(64);
		    OpenZWave::Manager::Get()->GetValueAsString(_notification->GetValueID(),&sVal);
//		    std::cout<<"Value Added for node:"<<(int)_notification->GetNodeId()<<" ValId:"<<_notification->GetValueID().GetId()<<" Val:"<<sVal<<std::endl;
		    break;
		}

		case OpenZWave::Notification::Type_ValueChanged:
		{
		    string sVal;
		    sVal.reserve(64);
		    OpenZWave::Manager::Get()->GetValueAsString(_notification->GetValueID(),&sVal);
//		    std::cout<<"Value Changed for node:"<<(int)_notification->GetNodeId()<<" ValId:"<<_notification->GetValueID().GetId()<<" Val:"<<sVal<<std::endl;
		    break;
		}
                        
                case OpenZWave::Notification::Type_Notification:
		{
		    std::cout<<"NOTIFICATION for node:"<<(int)_notification->GetNodeId()<<" # ";
		    switch(_notification->GetNotification()) {
		    case OpenZWave::Notification::Code_MsgComplete: 
			std::cout<<"Message Complete"<<std::endl;
			break;
		    case OpenZWave::Notification::Code_Timeout: 
			std::cout<<"Timeout"<<std::endl;
			break;
		    case OpenZWave::Notification::Code_NoOperation: 
			std::cout<<"NoOperation"<<std::endl;
			break;
		    case OpenZWave::Notification::Code_Awake: 
			std::cout<<"Awake"<<std::endl;
			break;
		    case OpenZWave::Notification::Code_Sleep: 
			std::cout<<"Sleep"<<std::endl;
			break;
		    case OpenZWave::Notification::Code_Dead: 
			std::cout<<"Dead"<<std::endl;
			break;
		    case OpenZWave::Notification::Code_Alive: 
			std::cout<<"Alive"<<std::endl;
			break;
		    default:
			std::cout<<"UNKNOWN CODE "<<(int)_notification->GetNotification() <<std::endl;
		    }
		    break;
		}
                case OpenZWave::Notification::Type_DriverReset:
                case OpenZWave::Notification::Type_NodeNaming:
                default:
                {
			//WCCOAozwInternalDp::setStatusText("Other notification");
                }
    }
    pthread_mutex_unlock( &g_criticalSection );
}

void WCCOAozwHWService::unlockOnSignal()
{
    pthread_cond_broadcast(&initCond);
}

//--------------------------------------------------------------------------------
// called after connect to data
PVSSboolean WCCOAozwHWService::initialize(int argc, char *argv[])
{
  // use this function to initialize internals
  // if you don't need it, you can safely remove the whole method
  // TODO
    ErrHdl::error(ErrClass(ErrClass::PRIO_INFO, ErrClass::ERR_SYSTEM, ErrClass::NOERR,
                             "We will connect to zw intfce ",CharString(WCCOAozwResources::getZwaveInterface()),""));


    string port(WCCOAozwResources::getZwaveInterface().c_str());
    pthread_mutexattr_t mutexattr;

   // Set up mutual exclusion so that this thread has priority
    pthread_mutexattr_init ( &mutexattr );
    pthread_mutexattr_settype( &mutexattr, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init( &g_criticalSection, &mutexattr );
    pthread_mutexattr_destroy( &mutexattr );

    pthread_mutex_lock( &initMutex );
    
    printf("\n Creating Options \n");
    
    OpenZWave::Options::Create( "/usr/local/etc/openzwave", "../meta/", "" );
    OpenZWave::Options::Get()->AddOptionInt( "SaveLogLevel", OpenZWave::LogLevel_Detail );
    OpenZWave::Options::Get()->AddOptionInt( "QueueLogLevel", OpenZWave::LogLevel_Debug );
    OpenZWave::Options::Get()->AddOptionInt( "DumpTrigger", OpenZWave::LogLevel_Error );
    OpenZWave::Options::Get()->AddOptionInt( "PollInterval", 500 );

    // want console logging?
    OpenZWave::Options::Get()->AddOptionBool( "ConsoleOutput", false );

    OpenZWave::Options::Get()->AddOptionBool( "IntervalBetweenPolls", true );
    OpenZWave::Options::Get()->AddOptionBool("ValidateValueChanges", true);
    OpenZWave::Options::Get()->Lock();

    printf("\n Creating Manager \n");

    OpenZWave::Manager::Create();
    OpenZWave::Manager::Get()->AddWatcher( OnNotification, NULL );
    OpenZWave::Manager::Get()->AddDriver( port );

//WCCOAozwInternalDp::setStatusText(CharString("Initializing"));
//    WCCOAozwResources::idp.setStatusText("Initializing");

  // To stop driver return PVSS_FALSE
  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------
// called after connect to event

PVSSboolean WCCOAozwHWService::start()
{
  // use this function to start your hardware activity.

//    WCCOAozwResources::idp.setStatusText("Starting");
WCCOAozwInternalDp::setStatusText(CharString("Starting"));


printf("In WCCOAozwHWService::start: NOT waiting for mutex cond wait\n");
    // Release the critical section
//    pthread_cond_wait( &initCond, &initMutex );

//    WCCOAozwResources::idp.setStatusText("Started");

  // TODO
  // return PVSS_FALSE on failure
  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------

void WCCOAozwHWService::stop()
{
//    WCCOAozwResources::idp.setStatusText("Stopping");

WCCOAozwInternalDp::setStatusText(CharString("Stopping"));


  // use this function to stop your hardware activity.
    string port(WCCOAozwResources::getZwaveInterface().c_str());

    // program exit (clean up)
    if( strcasecmp( port.c_str(), "usb" ) == 0 )
      {
	OpenZWave::Manager::Get()->RemoveDriver( "HID Controller" );
      }
    else
      {
	OpenZWave::Manager::Get()->RemoveDriver( port );
      }
    OpenZWave::Manager::Get()->RemoveWatcher( OnNotification, NULL );
    OpenZWave::Manager::Destroy();
    OpenZWave::Options::Destroy();


//    pthread_mutex_destroy( &g_criticalSection );


//    WCCOAozwResources::idp.setStatusText("");


/*
OpenZWave::Manager::Destroy();
printf("OpenZWaveManager destroyed\n");

OpenZWave::Options::Destroy();
*/

  // TODO
}

//--------------------------------------------------------------------------------
static int MYCOUNT=0;

void WCCOAozwHWService::workProc()
{
//  HWObject obj;

  // TODO somehow receive a message from your device

/*
  if ( MYCOUNT<100) // message received 
  {
    DEBUG_DRV_USR1("received message");
  MYCOUNT++;

    // TODO if your message contains the timestamp use the following 2 lines
    obj.setOrgTime(TimeVar());
    obj.setTimeOfPeriphFlag();

    // TODO else, if you don't read the timestamp, use current time
    obj.setOrgTime(TimeVar());  // current time

    CharString address;

    if (MYCOUNT%2==1) address="1";
    else  address="7";

    // TODO
    // put the address string into the HWObject so that the HWMapper can find
    // the corresponding HWObject (see below)
    obj.setAddress(address);

    // TODO
    // set the received data into the HWObject
    char* data=(char*)malloc(10);
    snprintf(data,10,"SOME%02d",MYCOUNT);
    int numberOfBytes=strlen(data);;

    obj.setData((PVSSchar*)data);
    obj.setDlen(numberOfBytes);

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
*/


    {
	std::lock_guard<std::mutex> guardVC(WCCOAozwHWService::s_changesMutex);
	if (!WCCOAozwHWService::s_changesToSend.empty()) {
	    for (auto vci = WCCOAozwHWService::s_changesToSend.begin(); vci!=WCCOAozwHWService::s_changesToSend.end();) {
		WCCOAozwValueChange &vc=*vci;
		HWObject obj;
		obj.setAddress(vc.addr);
		obj.setOrgTime(vc.ts);
		char* data=(char*)malloc(32);
		snprintf(data,32,"%s",(const char*)vc.value->formatValue());
		int numberOfBytes=strlen(data);
		obj.setData((PVSSchar*)data);
		obj.setDlen(numberOfBytes);
		HWObject *addrObj = DrvManager::getHWMapperPtr()->findHWObject(&obj);
		delete vc.value; // because we were allocating it in another thread...
		vc.value=0;
		if ( addrObj ) DrvManager::getSelfPtr()->toDp(&obj, addrObj);
		
		vci = WCCOAozwHWService::s_changesToSend.erase(vci); // advances and erases at the same time
		

/*		    WCCOAozwValueChange vc;
		    vc.ts=TimeVar();
		    vc.addr=CharString(_notification->GetNodeId())+".QS";
		    vc.value=new TextVar("added");
		    WCCOAozwHWService::s_changesToSend.push_back(vc);
*/	     }
	}
    }
/*

    if (WCCOAozwHWService::s_nodesUpdated) {

	std::lock_guard<std::mutex> guard(WCCOAozwHWService::s_nodesUpdatedMutex);
	obj.setOrgTime(TimeVar());
//    obj.setTimeOfPeriphFlag();
    CharString address="1.something";
    obj.setAddress(address);
    char* data=(char*)malloc(32);
    snprintf(data,32,"%s",(const char*)WCCOAozwHWService::s_nodes.getAt(UIntegerVar(3))->formatValue());
    int numberOfBytes=strlen(data);

    obj.setData((PVSSchar*)data);
    obj.setDlen(numberOfBytes);
    HWObject *addrObj = DrvManager::getHWMapperPtr()->findHWObject(&obj);
    if ( addrObj )
      DrvManager::getSelfPtr()->toDp(&obj, addrObj);



    }
*/

    WCCOAozwInternalDp::updateNodesIds();

    sleep(1);
}

//--------------------------------------------------------------------------------
// we get data from PVSS and shall send it to the periphery

PVSSboolean WCCOAozwHWService::writeData(HWObject *objPtr)
{
  if ( Resources::isDbgFlag(Resources::DBG_DRV_USR1) ) {
    DEBUG_DRV_USR1("we have a value to send...");
    objPtr->debugPrint();
  }
  // TODO somehow send the data to your device

  // sending was successful
  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------
