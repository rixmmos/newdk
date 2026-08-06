/////////////////////////////////////////////////////////////////////////////
// Filename : PKTError.cpp



/////////////////////////////////////////////////////////////////////////////

// include files
#include "PKTError.h"

#include "MPacketID.h"


PKTError::PKTError() {
    nSize = szPKTError - szMPacketSize;
}


void PKTError::read(SocketInputStream& iStream) {
    iStream.read((char*)this, szPKTError);

    // change order - network to host
    //	nSize		= ntohl( nSize );
    //	nCode		= ntohl( nCode );
    //	nError		= ntohl( nError );
}


void PKTError::write(SocketOutputStream& oStream) {
    nCode = getID();

    // change order - host to network
    //	nSize		= htonl( nSize );
    //	nCode		= htonl( nCode );
    //	nError		= htonl( nError );

    oStream.write((const char*)this, szPKTError);

    // restore order
    //	nSize		= ntohl( nSize );
    //	nCode		= ntohl( nCode );
    //	nError		= ntohl( nError );
}

// debug message
string PKTError::toString() const {
    StringStream msg;
    msg << "Result("
        << "ErrorCode:" << nError << ")";

    return msg.toString();
}
