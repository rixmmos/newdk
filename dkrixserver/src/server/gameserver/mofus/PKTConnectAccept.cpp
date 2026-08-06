/////////////////////////////////////////////////////////////////////////////
// Filename : PKTConnectAccept.cpp

/////////////////////////////////////////////////////////////////////////////

// include files
#include "PKTConnectAccept.h"


PKTConnectAccept::PKTConnectAccept() {
    nSize = szPKTConnectAccept - szMPacketSize;
}


void PKTConnectAccept::read(SocketInputStream& iStream) {
    iStream.read((char*)this, szPKTConnectAccept);

    // change order - network to host
    //	nSize		= ntohl( nSize );
    //	nCode		= ntohl( nCode );
}


void PKTConnectAccept::write(SocketOutputStream& oStream) {
    nCode = getID();

    // change order - host to network
    //	nSize		= htonl( nSize );
    //	nCode		= htonl( nCode );

    oStream.write((const char*)this, szPKTConnectAccept);

    // restore order
    //	nSize		= ntohl( nSize );
    //	nCode		= ntohl( nCode );
}

// debug message
string PKTConnectAccept::toString() const {
    return "ConenctAccept()";
}
