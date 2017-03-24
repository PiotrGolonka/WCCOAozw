// WCCILozwConnection.cxx

#include <PVSSMacros.hxx>

#include <WCCILozwConnection.hxx>
#include <WCCILozwHWService.hxx>
#include <WCCILozwDrv.hxx>
#include <WCCILozwResources.hxx>

WCCILozwConnection::WCCILozwConnection(DpIdType id) : HmiConnection(id)
{
  DEBUG_MSG("creating new connection with  id " << id);
}

WCCILozwConnection::~WCCILozwConnection()
{
}

/** the open connection callback.
  * possible state changes are HMI_CS_Failure and HMI_CS_Connected
  */
void WCCILozwConnection::open()
{
  DEBUG_MSG("opening connection with name " << getMyName());
  requestStateChange(HMI_CS_Connected);
}

/** the close connection callback.
  * there are no active state changes possible here
  */
void WCCILozwConnection::close()
{
  DEBUG_MSG("closing connection with name " << getMyName());
}

/** the open server side callback.
  * possible state changes are HMI_CS_Failure and HMI_CS_Listening
  */
void WCCILozwConnection::openServer()
{
  DEBUG_MSG("opening server connection with name " << getMyName() << " - will fail");
  requestStateChange(HMI_CS_Failure);
}

/** the close server side callback.
  * there are no active state changes possible here
  */
void WCCILozwConnection::closeServer()
{
  DEBUG_MSG("closing server connection with name " << getMyName());
}

/** the general query callback.
  * this callback is triggered whenever a general query should be done.
  * the default implementation issues singleQuery calls for all input and IO elements.
  */
void WCCILozwConnection::doGeneralQuery()
{
  DEBUG_MSG("GQ requested on connection with name " << getMyName());
}

/** the internal general query callback.
  * this callback is triggered whenever an internal general query should be done.
  * the default implementation issues write requests calls for all output and IO elements.
  */
void WCCILozwConnection::doInternalGeneralQuery()
{
  DEBUG_MSG("IGQ requested on connection with name " << getMyName());
}

/** the alert sync callback.
  * this callback is triggered whenever a general alert sync should be done.
  */
void WCCILozwConnection::doAlertSync()
{
  DEBUG_MSG("AlertSync requested on connection with name " << getMyName());
}

bool WCCILozwConnection::isConnError()
{
  static int cnt = 0;
  
  DEBUG_MSG("connError check on connection with name " << getMyName());

  // return connError every 5 times (e.g. 100 secs by default settings)
  if (++cnt == 5)
  {
    DEBUG_MSG("connError check fails");
    cnt = 0;
    return true;
  }

  DEBUG_MSG("connError check succeeds");
  return false;   // no error
}

void WCCILozwConnection::singleQuery(HWObject *objPtr)
{
  // get value from PLC
}

PVSSboolean WCCILozwConnection::writeData(HWObject *objPtr)
{
  // write value to PLC
  return PVSS_TRUE;
}

void WCCILozwConnection::workFunc()
{
  // check the incoming connection traffic for spontaneous updates
}

