#ifndef WCCOAozwTRANS_H
#define WCCOAozwTRANS_H

#include <Transformation.hxx>

// Our Transformation class for Text
// As the Transformation really depends on the format of data you send and
// receive in your protocol (see HWService), this template is just an
// example.
// Things you have to change are marked with TODO

class WCCOAozwTrans : public Transformation
{
  public:
    // TODO probably your ctor looks completely different ...
    WCCOAozwTrans(VariableType type) : varType(type) {}

    virtual TransformationType isA() const;
    virtual TransformationType isA(TransformationType type) const;

    // (max) size of one item. This is needed by DrvManager to
    // create the buffer used in toPeriph and by the Low-Level-Compare
    // For our Text-Transformation we set it arbitrarly to 256 Bytes
    virtual int itemSize() const;

    // The type of Variable we are expecting here
    virtual VariableType getVariableType() const;

    // Clone of our class
    virtual Transformation *clone() const;

    // Conversion from PVSS to Hardware
    virtual PVSSboolean toPeriph(PVSSchar *dataPtr, PVSSuint len, const Variable &var, const PVSSuint subix) const;

    // Conversion from Hardware to PVSS
    virtual VariablePtr toVar(const PVSSchar *data, const PVSSuint dlen, const PVSSuint subix) const;

  private:
    // TODO whatever you have to store depends on your implementation
    VariableType varType;  // this instance handles this data type on the PVSS side; the Hardware
                           // always deliveres TEXT in this example
};

#endif
