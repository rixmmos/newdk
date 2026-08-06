//////////////////////////////////////////////////////////////////////
//
// Filename    : GCGlobalChat.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_GLOBAL_CHAT_H__
#define __GC_GLOBAL_CHAT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCGlobalChat;
//



//
//////////////////////////////////////////////////////////////////////

class GCGlobalChat : public Packet {
public:
    GCGlobalChat() {};
    ~GCGlobalChat() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_GLOBAL_CHAT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szuint + szBYTE + m_Message.size() + szBYTE;
    }

    // get packet name
    string getPacketName() const {
        return "GCGlobalChat";
    }

    // get packet's debug string
    string toString() const;

    // get/set text color
    uint getColor() const {
        return m_Color;
    }
    void setColor(uint color) {
        m_Color = color;
    }

    // get/set chatting message
    string getMessage() const {
        return m_Message;
    }
    void setMessage(const string& msg) {
        m_Message = msg;
    }

    // get/set chatting message
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

    // race
    Race_t m_Race;
};


//////////////////////////////////////////////////////////////////////
//
// class GCGlobalChatFactory;
//
// Factory for GCGlobalChat
//
//////////////////////////////////////////////////////////////////////

class GCGlobalChatFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCGlobalChat();
    }

    // get packet name
    string getPacketName() const {
        return "GCGlobalChat";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_GLOBAL_CHAT;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szuint + szBYTE + 128 + szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCGlobalChatHandler;
//
//////////////////////////////////////////////////////////////////////

class GCGlobalChatHandler {
public:
    // execute packet's handler
    static void execute(GCGlobalChat* pPacket, Player* pPlayer);
};

#endif
