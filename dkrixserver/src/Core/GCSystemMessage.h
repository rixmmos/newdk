//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSystemMessage.h
// Written By  : Reiot
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_SYSTEM_MESSAGE_H__
#define __GC_SYSTEM_MESSAGE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


enum SystemMessageType {
    SYSTEM_MESSAGE_NORMAL = 0,
    SYSTEM_MESSAGE_OPERATOR,    
    SYSTEM_MESSAGE_MASTER_LAIR, 
    SYSTEM_MESSAGE_COMBAT,      
    SYSTEM_MESSAGE_INFO,        
    SYSTEM_MESSAGE_HOLY_LAND,   
    SYSTEM_MESSAGE_RANGER_SAY,  
    SYSTEM_MESSAGE_PLAYER,      
    SYSTEM_MESSAGE_MAX
};

//////////////////////////////////////////////////////////////////////
//
// class GCSystemMessage;
//



//
//////////////////////////////////////////////////////////////////////

class GCSystemMessage : public Packet {
public:
    GCSystemMessage() : m_Color(0x006040E8), m_Type(SYSTEM_MESSAGE_NORMAL) {}

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_SYSTEM_MESSAGE;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Message.size() + szuint + szBYTE;
    }

    // get packet name
    string getPacketName() const {
        return "GCSystemMessage";
    }

    // get packet's debug string
    string toString() const;

    // get/set chatting message
    string getMessage() const {
        return m_Message;
    }
    void setMessage(const string& msg) {
        m_Message = msg;
    }

    // get/set text color
    uint getColor() const {
        return m_Color;
    }
    void setColor(uint color) {
        m_Color = color;
    }

    SystemMessageType getType() const {
        return m_Type;
    }
    void setType(SystemMessageType Type) {
        m_Type = Type;
    }

    // get/set race
    Race_t getRace() const {
        return m_Race;
    }
    void setRace(Race_t race) {
        m_Race = race;
    }

private:
    // chatting message
    string m_Message;

    // text color
    uint m_Color;

    SystemMessageType m_Type;

    // race
    Race_t m_Race;
};


//////////////////////////////////////////////////////////////////////
//
// class GCSystemMessageFactory;
//
// Factory for GCSystemMessage
//
//////////////////////////////////////////////////////////////////////

class GCSystemMessageFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCSystemMessage();
    }

    // get packet name
    string getPacketName() const {
        return "GCSystemMessage";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_SYSTEM_MESSAGE;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 256 + szuint + szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCSystemMessageHandler;
//
//////////////////////////////////////////////////////////////////////

class GCSystemMessageHandler {
public:
    // execute packet's handler
    static void execute(GCSystemMessage* pPacket, Player* pPlayer);
};

#endif
