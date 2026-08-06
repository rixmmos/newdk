//----------------------------------------------------------------------
//
// Filename    : PCInfo.h
// Writen By   : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __PC_INFO_H__
#define __PC_INFO_H__

// include files
#include "Exception.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "Types.h"

//----------------------------------------------------------------------
//
// class PCInfo;
//




//
//----------------------------------------------------------------------

class PCInfo {
public:
    // destructor
    virtual ~PCInfo() {}

    
    virtual PCType getPCType() const = 0;

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------

    // read data from socket input stream
    virtual void read(SocketInputStream& iStream) = 0;

    // write data to socket output stream
    virtual void write(SocketOutputStream& oStream) const = 0;

    // get size of object
    virtual uint getSize() const = 0;

    // get debug string
    virtual string toString() const = 0;
};

#endif
