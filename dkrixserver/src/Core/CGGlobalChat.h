//////////////////////////////////////////////////////////////////////
//
// Filename    : CGGlobalChat.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_GLOBAL_CHAT_H__
#define __CG_GLOBAL_CHAT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGGlobalChat;
//


//
//////////////////////////////////////////////////////////////////////

class CGGlobalChat : public Packet {
public:
    CGGlobalChat() {};
    ~CGGlobalChat() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_GLOBAL_CHAT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szuint + szBYTE + m_Message.size();
    }

    // get packet name
    string getPacketName() const {
        return "CGGlobalChat";
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


private:
    // text color
    uint m_Color;

    // chatting message
    string m_Message;
};


//////////////////////////////////////////////////////////////////////
//
// class CGGlobalChatFactory;
//
// Factory for CGGlobalChat
//
//////////////////////////////////////////////////////////////////////

class CGGlobalChatFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGGlobalChat();
    }

    // get packet name
    string getPacketName() const {
        return "CGGlobalChat";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_GLOBAL_CHAT;
    }

    // get packet's max body size
    
    PacketSize_t getPacketMaxSize() const {
        return szuint + szBYTE + 128;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGGlobalChatHandler;
//
//////////////////////////////////////////////////////////////////////

class CGGlobalChatHandler {
public:
    // execute packet's handler
    static void execute(CGGlobalChat* pPacket, Player* pPlayer);
};

#endif
