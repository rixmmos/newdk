//////////////////////////////////////////////////////////////////////
//
// Filename    : CGWhisper.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_WHISPER_H__
#define __CG_WHISPER_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGWhisper : public Packet {
public:
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_WHISPER;
    }

    PacketSize_t getPacketSize() const throw() {
        return szBYTE + m_Name.size() + szuint + szBYTE + m_Message.size();
    }

    std::string getPacketName() const throw() {
        return "CGWhisper";
    }

    std::string toString() const throw();

    std::string getName() const throw() {
        return m_Name;
    }

    void setName(const std::string& name) throw() {
        m_Name = name;
    }

    uint getColor() const throw() {
        return m_Color;
    }

    void setColor(uint color) throw() {
        m_Color = color;
    }

    std::string getMessage() const throw() {
        return m_Message;
    }

    void setMessage(const std::string& msg) throw() {
        m_Message = msg;
    }

private:
    std::string m_Name;
    uint m_Color;
    std::string m_Message;
};

class CGWhisperFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGWhisper();
    }

    std::string getPacketName() const throw() {
        return "CGWhisper";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_WHISPER;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + 10 + szuint + szBYTE + 128;
    }
};

#ifndef __GAME_CLIENT__
class CGWhisperHandler {
public:
    static void execute(CGWhisper* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};
#endif

#endif
