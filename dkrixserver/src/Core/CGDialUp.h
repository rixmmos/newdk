//////////////////////////////////////////////////////////////////////
//
// Filename    : CGDialUp.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_DIAL_UP_H__
#define __CG_DIAL_UP_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGDialUp;
//
//////////////////////////////////////////////////////////////////////

class CGDialUp : public Packet {
public:
    // constructor
    CGDialUp();

    // destructor
    ~CGDialUp();

public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_DIAL_UP;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szPhoneNumber;
    }

    // get packet name
    string getPacketName() const {
        return "CGDialUp";
    }

    // get packet's debug string
    string toString() const;

public:
    // get / set phoneNumber
    PhoneNumber_t getPhoneNumber() const {
        return m_PhoneNumber;
    }
    void setPhoneNumber(PhoneNumber_t PhoneNumber) {
        m_PhoneNumber = PhoneNumber;
    }

private:
    // SlotID
    PhoneNumber_t m_PhoneNumber;
};


//////////////////////////////////////////////////////////////////////
//
// class CGDialUpFactory;
//
// Factory for CGDialUp
//
//////////////////////////////////////////////////////////////////////

class CGDialUpFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGDialUp();
    }

    // get packet name
    string getPacketName() const {
        return "CGDialUp";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_DIAL_UP;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szPhoneNumber;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGDialUpHandler;
//
//////////////////////////////////////////////////////////////////////

class CGDialUpHandler {
public:
    // execute packet's handler
    static void execute(CGDialUp* pPacket, Player* player);
};

#endif
