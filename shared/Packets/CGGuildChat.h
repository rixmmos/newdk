//////////////////////////////////////////////////////////////////////
//
// Filename    : CGGuildChat.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_GUILD_CHAT_H__
#define __CG_GUILD_CHAT_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGGuildChat;
//
//////////////////////////////////////////////////////////////////////

class CGGuildChat : public Packet {
public:
    // Initialize packet by reading data from the incoming stream.
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);

    // Serialize packet data to the outgoing stream.
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);

    // execute packet's handler
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    // get packet id
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_GUILD_CHAT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const throw() {
        return szBYTE + szuint + // text color
               szBYTE +          // message size
               m_Message.size(); // chatting message
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGGuildChat";
    }

    // get packet's debug string
    string toString() const throw();

    BYTE getType() const throw() {
        return m_Type;
    }
    void setType(BYTE type) throw() {
        m_Type = type;
    }
    BYTE GetType() const throw() { return getType(); }
    void SetType(BYTE type) throw() { setType(type); }

    // get/set text color
    uint getColor() const throw() {
        return m_Color;
    }
    void setColor(uint color) throw() {
        m_Color = color;
    }

    // get/set chatting message
    string getMessage() const throw() {
        return m_Message;
    }
    void setMessage(const string& msg) throw() {
        m_Message = msg;
    }


private:
    BYTE m_Type;
    // text color
    uint m_Color;

    // chatting message
    string m_Message;
};


//////////////////////////////////////////////////////////////////////
//
// class CGGuildChatFactory;
//
// Factory for CGGuildChat
//
//////////////////////////////////////////////////////////////////////

class CGGuildChatFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() throw() {
        return new CGGuildChat();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGGuildChat";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_GUILD_CHAT;
    }

    // get packet's max body size
    // Depends on the maximum message length.
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + szuint + // text color
               szBYTE +          // message size
               128;              // chatting message
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGGuildChatHandler;
//
//////////////////////////////////////////////////////////////////////

class CGGuildChatHandler {
public:
    // execute packet's handler
    static void execute(CGGuildChat* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
