//////////////////////////////////////////////////////////////////////
//
// Filename    : LCCreatePCOK.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LC_CREATE_PC_OK_H__
#define __LC_CREATE_PC_OK_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCCreatePCOK;
//

//
//////////////////////////////////////////////////////////////////////

class LCCreatePCOK : public Packet {
public:
    LCCreatePCOK() {};
    ~LCCreatePCOK() {};
    
    void read(SocketInputStream& iStream) {}

    
    void write(SocketOutputStream& oStream) const {}

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LC_CREATE_PC_OK;
    }

    // get packet body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet's name
    string getPacketName() const {
        return "LCCreatePCOK";
    }

    // get packet's debug string
    string toString() const {
        return "LCCreatePCOK";
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCCreatePCOKFactory;
//
// Factory for LCCreatePCOK
//
//////////////////////////////////////////////////////////////////////

class LCCreatePCOKFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LCCreatePCOK();
    }

    // get packet name
    string getPacketName() const {
        return "LCCreatePCOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LC_CREATE_PC_OK;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCCreatePCOKHandler;
//
//////////////////////////////////////////////////////////////////////

class LCCreatePCOKHandler {
public:
    // execute packet's handler
    static void execute(LCCreatePCOK* pPacket, Player* pPlayer);
};

#endif
