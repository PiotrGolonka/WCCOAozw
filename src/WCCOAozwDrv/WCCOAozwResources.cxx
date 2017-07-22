// Our Resource class.
// This class will interpret the command line and read the config file

#include <ErrHdl.hxx>
#include <PVSSMacros.hxx> // to get debugs, for instance

#include "WCCOAozwResources.hxx"
#include "WCCOAozwConnection.hxx"

// TODO you likely have different things in the config file
unsigned short int WCCOAozwResources::tcpPort = 12401;

//CharString WCCOAozwResources::zwaveInterface = "/dev/ttyAMA0"; // for RPI2
CharString WCCOAozwResources::zwaveInterface = "/dev/ttyS0";

CharString WCCOAozwResources::ozwDrviverInternalDpName = ""; // from config file or auto-created _OZWDriver_<num> (in ::init())

//WCCOAozwInternalDp WCCOAozwResources::idp;


HmiConnection* WCCOAozwResources::getNewHmiConnection(DpIdType id) const
{
  return new WCCOAozwConnection(id);
}



//-------------------------------------------------------------------------------
// init is a wrapper around begin, readSection and end

void WCCOAozwResources::init(int &argc, char *argv[])
{
  // Prepass of commandline arguments, e.g. get the arguments needed to
  // find the config file.
  begin(argc, argv);

  // Read the config file
  while ( readSection() || generalSection() ) ;

  // Postpass of the commandline arguments, e.g. get the arguments that
  // will override entries in the config file
  end(argc, argv);

  // if not defined in the config file, construct the name for the
  // internal datapoint
  if (ozwDrviverInternalDpName == "") {
    ozwDrviverInternalDpName = CharString("_OZWDriver_") + CharString(Resources::getManNum());
  }
}

//-------------------------------------------------------------------------------

PVSSboolean WCCOAozwResources::readSection()
{
  // Are we in our section ?
  if ( !isSection("OZW") ) return PVSS_FALSE;

  // skip "[OZW]"
  getNextEntry();

  // Now read the section until new section or end of file
  while ( (cfgState != CFG_SECT_START) && (cfgState != CFG_EOF) )
  {
    // TODO whatever you have to read from the config file
    if      ( keyWord.icmp("zwaveInterface") == 0 ) cfgStream >> zwaveInterface;
    else if ( keyWord.icmp("tcpPort")        == 0 ) cfgStream >> tcpPort;
    else if ( keyWord.icmp("driverDpName")        == 0 ) cfgStream >> ozwDrviverInternalDpName;
    else if ( !commonKeyWord() )
    {
      ErrHdl::error(ErrClass::PRIO_WARNING,    // Not that bad
                    ErrClass::ERR_PARAM,       // Error is with config file, not with driver
                    ErrClass::ILLEGAL_KEYWORD, // Illegal keyword in res.
                    keyWord);

      // signal Error, so we stop later
      cfgError = PVSS_TRUE;
    }

    // get next entry
    getNextEntry();
  }

  // So the loop will stop at the end of the file
  return cfgState != CFG_EOF;
}

// Inteface to HWService: get the dpId of an command
const DpIdentifier & WCCOAozwResources::getInternalDpId(int idx)
{
  return idp.getId(idx);
}

PVSSboolean WCCOAozwResources::isReadyToConnect ()
{
  printf("isReadyToConnect ? %i %i \n",idp.getState(),idp.getError());
  if (idp.getState() == DrvIntDp::Ready &&  !idp.getError())
    return PVSS_TRUE;
  else
    return PVSS_FALSE;
}

//-------------------------------------------------------------------------------
// Interface to internal Datapoints
// Get the number of names we need the DpId for
/*
int WCCOAozwResources::getNumberOfDpNames()
{
  // TODO if you use internal DPs
  return 0;
}
*/
//-------------------------------------------------------------------------------
