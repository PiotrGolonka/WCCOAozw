// Our Resource class.
// This class will interpret the command line and read the config file

#include "WCCOAozwResources.hxx"
#include <ErrHdl.hxx>

// TODO you likely have different things in the config file
unsigned short int WCCOAozwResources::tcpPort = 12401;

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
}

//-------------------------------------------------------------------------------

PVSSboolean WCCOAozwResources::readSection()
{
  // Are we in our section ?
  if ( !isSection("WCCOAozw") ) return PVSS_FALSE;

  // skip "[WCCOAozw]"
  getNextEntry();

  // Now read the section until new section or end of file
  while ( (cfgState != CFG_SECT_START) && (cfgState != CFG_EOF) )
  {
    // TODO whatever you have to read from the config file
    if      ( keyWord.icmp("tcpPort") == 0 ) cfgStream >> tcpPort;

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

//-------------------------------------------------------------------------------
// Interface to internal Datapoints
// Get the number of names we need the DpId for

int WCCOAozwResources::getNumberOfDpNames()
{
  // TODO if you use internal DPs
  return 0;
}

//-------------------------------------------------------------------------------
