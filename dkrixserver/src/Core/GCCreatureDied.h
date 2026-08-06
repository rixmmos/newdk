//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCreatureDied.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_CREATURE_DIED_H__
#define __GC_CREATURE_DIED_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCCreatureDied;
//



//
//////////////////////////////////////////////////////////////////////

class GCCreatureDied : public Packet {
public:
    GCCreatureDied() {};
    ~GCCreatureDied() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_CREATURE_DIED;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }

    // get packet name
    string getPacketName() const {
        return "GCCreatureDied";
    }

    // get packet's debug string
    string toString() const;

    // get/set dead creature's creature id
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(const ObjectID_t& creatureID) {
        m_ObjectID = creatureID;
    }


private:
    // dead creature's creature id
    ObjectID_t m_ObjectID;
};


//////////////////////////////////////////////////////////////////////
//
// class GCCreatureDiedFactory;
//
// Factory for GCCreatureDied
//
//////////////////////////////////////////////////////////////////////

class GCCreatureDiedFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCCreatureDied();
    }

    // get packet name
    string getPacketName() const {
        return "GCCreatureDied";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_CREATURE_DIED;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCCreatureDiedHandler;
//
//////////////////////////////////////////////////////////////////////

class GCCreatureDiedHandler {
public:
    // execute packet's handler
    static void execute(GCCreatureDied* pPacket, Player* pPlayer);
};

#endif
