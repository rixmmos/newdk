//////////////////////////////////////////////////////////////////////
//
// Filename    : CGSetSlayerHotKey.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SET_SLAYER_HOT_KEY_H__
#define __CG_SET_SLAYER_HOT_KEY_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGSetSlayerHotKey;
//
//////////////////////////////////////////////////////////////////////

class CGSetSlayerHotKey : public Packet {
public:
    CGSetSlayerHotKey() {};
    virtual ~CGSetSlayerHotKey() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_SET_SLAYER_HOT_KEY;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szSkillType * 4;
    }

    // get packet name
    string getPacketName() const {
        return "CGSetSlayerHotKey";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set X Coordicate
    SkillType_t getHotKey(BYTE pos) const {
        return m_HotKey[pos];
    }
    void setHotKey(BYTE pos, SkillType_t SkillType) {
        m_HotKey[pos] = SkillType;
    }

private:
    SkillType_t m_HotKey[4];
};


//////////////////////////////////////////////////////////////////////
//
// class CGSetSlayerHotKeyFactory;
//
// Factory for CGSetSlayerHotKey
//
//////////////////////////////////////////////////////////////////////

class CGSetSlayerHotKeyFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGSetSlayerHotKey();
    }

    // get packet name
    string getPacketName() const {
        return "CGSetSlayerHotKey";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_SET_SLAYER_HOT_KEY;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szSkillType * 4;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGSetSlayerHotKeyHandler;
//
//////////////////////////////////////////////////////////////////////

class CGSetSlayerHotKeyHandler {
public:
    // execute packet's handler
    static void execute(CGSetSlayerHotKey* pPacket, Player* player);
};

#endif
