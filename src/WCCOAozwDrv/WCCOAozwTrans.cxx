// Our transformation class PVSS <--> Hardware
#include <WCCOAozwTrans.hxx>
#include <ErrHdl.hxx>     // The Error handler Basics/Utilities
#include <TextVar.hxx>    // Our Variable type Basics/Variables

#include <stdio.h>

// Look also at the comments at WCCOAozwHWMapper.cxx
//
// We have only one transformation object, that is parameterizable,
// i.e. we could tell it what type we want to handle.
// Then it makes use of the fact that "any" WinCCOA variable can
// be casted to/from string, by simple assignment.
//
// Hence, we assume that communication with hardware is done only
// with string-typed data (ie. that OpenZWave will be able to
// send us and accept all possible data using strings). And
// we also set an arbitrary maximum data size to be 256 chars.
// At this moment (no support for dyn-values) this seems to be
// a sensible compromise.

//----------------------------------------------------------------------------

TransformationType WCCOAozwTrans::isA() const
{
  // TransUserType=1000 is declared in Configs/ConfigTypes.hxx
  // and marks the first enum element for user transformation types

  // for other transformations, if we ever needed more than one
  // we would need to use other constants > 1000
  return (TransformationType) TransUserType;
}

//----------------------------------------------------------------------------

TransformationType WCCOAozwTrans::isA(TransformationType type) const
{
  // This one is for compatibility with some deprecated APIs; see headers
  // of parent class
  if ( type == isA() )
    return type;
  else
    return Transformation::isA(type);
}

//----------------------------------------------------------------------------

Transformation *WCCOAozwTrans::clone() const
{
  return new WCCOAozwTrans(varType);
}

//----------------------------------------------------------------------------
// Our item size. The max we will use is 256 Bytes.
// This is an arbitrary value! A Transformation for a long e.g. would return 4

int WCCOAozwTrans::itemSize() const
{
  // TODO
  return 256;
}

//----------------------------------------------------------------------------
// Our preferred Variable type. Data will be converted to this type
// before toPeriph is called.

VariableType WCCOAozwTrans::getVariableType() const
{
  // TODO
  return TEXT_VAR;
}

//----------------------------------------------------------------------------
// Convert data from PVSS to Hardware.
// In this example the subindex is ignored

PVSSboolean WCCOAozwTrans::toPeriph(PVSSchar *buffer, PVSSuint len,
                                      const Variable &var, const PVSSuint subix) const
{
  // TODO  everything in here has to be adapted to your needs

  // Be paranoic, check variable type
  if ( var.isA() != TEXT_VAR )
  {
    // Throw error message
    ErrHdl::error(
      ErrClass::PRIO_SEVERE,             // Data will be lost
      ErrClass::ERR_PARAM,               // Wrong parametrization
      ErrClass::UNEXPECTEDSTATE,         // Nothing else appropriate
      "WCCOAozwTrans", "toPeriph",     // File and function name
      "Wrong variable type for data"     // Unfortunately we don't know which DP
    );

    return PVSS_FALSE;
  }

  // Check data len. TextVar::getString returns a CharString
  if (len < static_cast<const TextVar &>(var).getString().len() + 1)
  {
    // Throw error message
    ErrHdl::error(
      ErrClass::PRIO_SEVERE,             // Data will be lost
      ErrClass::ERR_IMPL,                // Mus be implementation
      ErrClass::UNEXPECTEDSTATE,         // Nothing else appropriate
      "WCCOAozwTrans::toPeriph",       // File and function name
      "Data buffer too small; need:" +
      CharString(static_cast<const TextVar &>(var).getString().len() + 1) +
      " have:" + CharString(len)
    );

    return PVSS_FALSE;
  }

  sprintf(reinterpret_cast<char *>(buffer), "%s", static_cast<const TextVar &>(var).getValue());

  return PVSS_TRUE;
}

//----------------------------------------------------------------------------
// Conversion from Hardware to PVSS

VariablePtr WCCOAozwTrans::toVar(const PVSSchar *buffer, const PVSSuint dlen,
                                   const PVSSuint /* subix */) const
{
  // TODO  everything in here has to be adapted to your needs

  // Return pointer to new PVSS Variable
  Variable *var = Variable::allocate(varType);

  (*var) = TextVar(reinterpret_cast<const char *>(buffer));   // virtual operator= in all Variables

  return var;
}

//----------------------------------------------------------------------------
