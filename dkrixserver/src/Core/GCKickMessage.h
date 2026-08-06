//////////////////////////////////////////////////////////////////////
//
// Filename    : GCKickMessage.h
// Written By  : Reiot
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_KICK_MESSAGE_H__
#define __GC_KICK_MESSAGE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


enum KickMessageType {
    KICK_MESSAGE_PAY_TIMEOUT = 0,
    KICK_MESSAGE_EXPIRE_FREEPLAY,
#ifdef __THAILAND_SERVER__
    KICK_MESSAGE_CHILDGUARD,
#endif
    KICK_MESSAGE_MAX
};

//////////////////////////////////////////////////////////////////////
//
// class GCKickMessage;
//



//
//////////////////////////////////////////////////////////////////////

class GCKickMessage : public Packet {
public:
    GCKickMessage() : m_Type(KICK_MESSAGE_PAY_TIMEOUT) {}
    ~GCKickMessage() {};

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_KICK_MESSAGE;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + szuint;
    }

    // get packet name
    string getPacketName() const {
        return "GCKickMessage";
    }

    // get packet's debug string
    string toString() const;

    // get/set Kick Message Type
    BYTE getType() const {
        return m_Type;
    }
    void setType(BYTE type) {
        m_Type = (KickMessageType)type;
    }

    // get/set seconds
    uint getSeconds() const {
        return m_Seconds;
    }
    void setSeconds(uint seconds) {
        m_Seconds = seconds;
    }

private:
    KickMessageType m_Type;

    // seconds
    uint m_Seconds;
};


//////////////////////////////////////////////////////////////////////
//
// class GCKickMessageFactory;
//
// Factory for GCKickMessage
//
//////////////////////////////////////////////////////////////////////

class GCKickMessageFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCKickMessage();
    }

    // get packet name
    string getPacketName() const {
        return "GCKickMessage";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_KICK_MESSAGE;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + szuint;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCKickMessageHandler;
//
//////////////////////////////////////////////////////////////////////

class GCKickMessageHandler {
public:
    // execute packet's handler
    static void execute(GCKickMessage* pPacket, Player* pPlayer);
};

#endif
