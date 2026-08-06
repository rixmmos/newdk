//////////////////////////////////////////////////////////////////////
//
// Filename    : CLLogout.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CL_LOGOUT_H__
#define __CL_LOGOUT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CLLogout;
//



//
//////////////////////////////////////////////////////////////////////

class CLLogout : public Packet {
public:
    CLLogout() {};
    virtual ~CLLogout() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CL_LOGOUT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet name
    string getPacketName() const {
        return "CLLogout";
    }

    // get packet's debug string
    string toString() const {
        return "CLLogout";
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CLLogoutFactory;
//
// Factory for CLLogout
//
//////////////////////////////////////////////////////////////////////

class CLLogoutFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CLLogout();
    }

    // get packet name
    string getPacketName() const {
        return "CLLogout";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CL_LOGOUT;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CLLogoutHandler;
//
//////////////////////////////////////////////////////////////////////

class CLLogoutHandler {
public:
    // execute packet's handler
    static void execute(CLLogout* pPacket, Player* player);
};

#endif
