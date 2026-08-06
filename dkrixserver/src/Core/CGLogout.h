//////////////////////////////////////////////////////////////////////
//
// Filename    : CGLogout.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_LOGOUT_H__
#define __CG_LOGOUT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGLogout;
//



//
//////////////////////////////////////////////////////////////////////

class CGLogout : public Packet {
public:
    CGLogout() {};
    ~CGLogout() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_LOGOUT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet name
    string getPacketName() const {
        return "CGLogout";
    }

    // get packet's debug string
    string toString() const {
        return "CGLogout";
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGLogoutFactory;
//
// Factory for CGLogout
//
//////////////////////////////////////////////////////////////////////

class CGLogoutFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGLogout();
    }

    // get packet name
    string getPacketName() const {
        return "CGLogout";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_LOGOUT;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGLogoutHandler;
//
//////////////////////////////////////////////////////////////////////

class CGLogoutHandler {
public:
    // execute packet's handler
    static void execute(CGLogout* pPacket, Player* player);
};

#endif
