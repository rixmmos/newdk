//--------------------------------------------------------------------------------
//
// Filename    : CGConnectSetKey.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __CL_CONNECTSETKEY_H__
#define __CL_CONNECTSETKEY_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGConnectSetKey : public Packet {
public:
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_ENCODE_KEY;
    }

    PacketSize_t getPacketSize() const throw() {
        return szWORD + szWORD;
    }

    std::string getPacketName() const throw() {
        return "CGConnectSetKey";
    }

    std::string toString() const throw() {
        return "CGConnectSetKey";
    }

    WORD getEncryptKey() const throw() {
        return m_EncryptKey;
    }

    WORD getHashKey() const throw() {
        return m_HashKey;
    }

    void setEncryptKey(WORD key) throw() {
        m_EncryptKey = key;
    }

    void setHashKey(WORD key) throw() {
        m_HashKey = key;
    }

private:
    WORD m_EncryptKey;
    WORD m_HashKey;
};

class CGConnectSetKeyFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGConnectSetKey();
    }

    std::string getPacketName() const throw() {
        return "CGConnectSetKey";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_ENCODE_KEY;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szWORD + szWORD;
    }
};

class CGConnectSetKeyHandler {
public:
    static void execute(CGConnectSetKey* pPacket, Player* pPlayer);
};

#endif
