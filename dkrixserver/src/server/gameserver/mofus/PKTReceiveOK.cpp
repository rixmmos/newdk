/////////////////////////////////////////////////////////////////////////////
// Filename : PKTReceiveOK.cpp

/////////////////////////////////////////////////////////////////////////////

// include files
#include "PKTReceiveOK.h"

#include "MPacketID.h"


PKTReceiveOK::PKTReceiveOK() {
    nSize = szPKTReceiveOK - szMPacketSize;
}


void PKTReceiveOK::read(SocketInputStream& iStream) {
    iStream.read((char*)this, szPKTReceiveOK);

    // change order - network to host
    //	nSize		= ntohl( nSize );
    //	nCode		= ntohl( nCode );
}


void PKTReceiveOK::write(SocketOutputStream& oStream) {
    nCode = getID();

    // change order - host to network
    //	nSize		= htonl( nSize );
    //	nCode		= htonl( nCode );

    oStream.write((const char*)this, szPKTReceiveOK);

    // restore order
    //	nSize		= ntohl( nSize );
    //	nCode		= ntohl( nCode );
}

// debug message
string PKTReceiveOK::toString() const {
    return "ReceiveOK()";
}
