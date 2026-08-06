//////////////////////////////////////////////////////////////////////
//
// Filename    : LCDeletePCOK.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LC_DELETE_PC_OK_H__
#define __LC_DELETE_PC_OK_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCDeletePCOK;
//
//////////////////////////////////////////////////////////////////////

class LCDeletePCOK : public Packet {
public:
    LCDeletePCOK() {};
    ~LCDeletePCOK() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LC_DELETE_PC_OK;
    }

    // get packet body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet's name
    string getPacketName() const {
        return "LCDeletePCOK";
    }

    // get packet's debug string
    string toString() const {
        return "LCDeletePCOK";
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCDeletePCOKFactory;
//
// Factory for LCDeletePCOK
//
//////////////////////////////////////////////////////////////////////

class LCDeletePCOKFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LCDeletePCOK();
    }

    // get packet name
    string getPacketName() const {
        return "LCDeletePCOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LC_DELETE_PC_OK;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCDeletePCOKHandler;
//
//////////////////////////////////////////////////////////////////////

class LCDeletePCOKHandler {
public:
    // execute packet's handler
    static void execute(LCDeletePCOK* pPacket, Player* pPlayer);
};

#endif
