//--------------------------------------------------------------------------------
//
// Filename    : CGConnectSetKey.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __CL_CONNECTSETKEY_H__
#define __CL_CONNECTSETKEY_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CGConnectSetKey;
//


//
//--------------------------------------------------------------------------------


class CGConnectSetKey : public Packet {
public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_ENCODE_KEY;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szWORD    // authentication key
               + szWORD; // Slayer or Vampire?
    }

    // get packet name
    string getPacketName() const {
        return "CGConnectSetKey";
    }

    // get packet's debug string
    string toString() const {
        return "CGConnectSetKey";
    }

public:
    // get/set key
    WORD getEncryptKey() const {
        return m_EncryptKey;
    }

    WORD getHashKey() const {
        return m_HashKey;
    }

    void setEncryptKey(WORD key) {
        m_EncryptKey = key;
    }

    void setHashKey(WORD key) {
        m_HashKey = key;
    }

private:
    WORD m_EncryptKey;

    WORD m_HashKey;
};


//--------------------------------------------------------------------------------
//
// class CLLoginFactory;
//
// Factory for CLLogin
//
//--------------------------------------------------------------------------------

class CGConnectSetKeyFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGConnectSetKey();
    }

    // get packet name
    string getPacketName() const {
        return "CGConnectSetKey";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_ENCODE_KEY;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szWORD + szWORD;
    }
};


//--------------------------------------------------------------------------------
//
// class CLLoginHandler;
//
//--------------------------------------------------------------------------------

class CGConnectSetKeyHandler {
public:
    // execute packet's handler
    static void execute(CGConnectSetKey* pPacket, Player* pPlayer);
};

#endif
