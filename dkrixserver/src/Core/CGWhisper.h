//////////////////////////////////////////////////////////////////////
//
// Filename    : CGWhisper.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_WHISPER_H__
#define __CG_WHISPER_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGWhisper;
//


//
//////////////////////////////////////////////////////////////////////

class CGWhisper : public Packet {
public:
    CGWhisper() {};
    virtual ~CGWhisper() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_WHISPER;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Name.size() + szuint + szBYTE + m_Message.size();
    }

    // get packet name
    string getPacketName() const {
        return "CGWhisper";
    }

    // get packet's debug string
    string toString() const;

    // get/set Name
    string getName() const {
        return m_Name;
    }
    void setName(const string& Name) {
        m_Name = Name;
    }

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
    string m_Name;

    // text color
    uint m_Color;

    // chatting message
    string m_Message;
};


//////////////////////////////////////////////////////////////////////
//
// class CGWhisperFactory;
//
// Factory for CGWhisper
//
//////////////////////////////////////////////////////////////////////

class CGWhisperFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGWhisper();
    }

    // get packet name
    string getPacketName() const {
        return "CGWhisper";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_WHISPER;
    }

    // get packet's max body size
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 10 + szuint + szBYTE + 128;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGWhisperHandler;
//
//////////////////////////////////////////////////////////////////////

class CGWhisperHandler {
public:
    // execute packet's handler
    static void execute(CGWhisper* pPacket, Player* pPlayer);
};

#endif
