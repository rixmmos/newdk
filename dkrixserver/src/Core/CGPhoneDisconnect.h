//////////////////////////////////////////////////////////////////////
//
// Filename    : CGPhoneDisconnect.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_PHONE_DISCONNECT_H__
#define __CG_PHONE_DISCONNECT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGPhoneDisconnect;
//
//////////////////////////////////////////////////////////////////////

class CGPhoneDisconnect : public Packet {
public:
    // constructor
    CGPhoneDisconnect();

    // destructor
    ~CGPhoneDisconnect();

public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_PHONE_DISCONNECT;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szSlotID;
    }

    // get packet name
    string getPacketName() const {
        return "CGPhoneDisconnect";
    }

    // get packet's debug string
    string toString() const;

public:
    // get / set phoneNumber
    SlotID_t getSlotID() const {
        return m_SlotID;
    }
    void setSlotID(SlotID_t SlotID) {
        m_SlotID = SlotID;
    }

private:
    // SlotID
    SlotID_t m_SlotID;
};


//////////////////////////////////////////////////////////////////////
//
// class CGPhoneDisconnectFactory;
//
// Factory for CGPhoneDisconnect
//
//////////////////////////////////////////////////////////////////////

class CGPhoneDisconnectFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGPhoneDisconnect();
    }

    // get packet name
    string getPacketName() const {
        return "CGPhoneDisconnect";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_PHONE_DISCONNECT;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szSlotID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGPhoneDisconnectHandler;
//
//////////////////////////////////////////////////////////////////////

class CGPhoneDisconnectHandler {
public:
    // execute packet's handler
    static void execute(CGPhoneDisconnect* pPacket, Player* player);
};

#endif
