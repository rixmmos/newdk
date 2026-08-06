//////////////////////////////////////////////////////////////////////
//
// Filename    : CLSelectWorld.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CL_SELECT_WORLD_H__
#define __CL_SELECT_WORLD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CLSelectWorld;
//
//////////////////////////////////////////////////////////////////////

class CLSelectWorld : public Packet {
public:
    CLSelectWorld() {};
    virtual ~CLSelectWorld() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CL_SELECT_WORLD;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szWorldID;
    }

    // get packet name
    string getPacketName() const {
        return "CLSelectWorld";
    }

    // get / set WorldID
    WorldID_t getWorldID() const {
        return m_WorldID;
    }
    void setWorldID(WorldID_t WorldID) {
        m_WorldID = WorldID;
    }

    // get packet's debug string
    string toString() const {
        return "CLSelectWorld";
    }

private:
    WorldID_t m_WorldID;
};


//////////////////////////////////////////////////////////////////////
//
// class CLSelectWorldFactory;
//
// Factory for CLSelectWorld
//
//////////////////////////////////////////////////////////////////////

class CLSelectWorldFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CLSelectWorld();
    }

    // get packet name
    string getPacketName() const {
        return "CLSelectWorld";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CL_SELECT_WORLD;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szWorldID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CLSelectWorldHandler;
//
//////////////////////////////////////////////////////////////////////

class CLSelectWorldHandler {
public:
    // execute packet's handler
    static void execute(CLSelectWorld* pPacket, Player* player);
};

#endif
