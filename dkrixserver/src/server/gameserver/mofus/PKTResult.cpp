/////////////////////////////////////////////////////////////////////////////
// Filename : PKTResult.cpp


/////////////////////////////////////////////////////////////////////////////

// include files
#include "PKTResult.h"

#include "MPacketID.h"


PKTResult::PKTResult() {
    nSize = szPKTResult - szMPacketSize;
}


void PKTResult::read(SocketInputStream& iStream) {
    iStream.read((char*)this, szPKTResult);

    // change order - network to host
    //	nSize		= ntohl( nSize );
    //	nCode		= ntohl( nCode );
}


void PKTResult::write(SocketOutputStream& oStream) {
    nCode = getID();

    // change order - host to network
    //	nSize		= htonl( nSize );
    //	nCode		= htonl( nCode );

    oStream.write((const char*)this, szPKTResult);

    // restore order
    //	nSize		= ntohl( nSize );
    //	nCode		= ntohl( nCode );
}

// debug message
string PKTResult::toString() const {
    return "Result()";
}
