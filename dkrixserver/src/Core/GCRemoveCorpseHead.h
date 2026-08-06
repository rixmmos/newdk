//--------------------------------------------------------------------------------
//
// Filename    : GCRemoveCorpseHead.h
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __GC_REMOVE_CORPSE_HEAD_H__
#define __GC_REMOVE_CORPSE_HEAD_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class GCRemoveCorpseHead;
//


//

//




//
// *CAUTION*
//


//
//--------------------------------------------------------------------------------

class GCRemoveCorpseHead : public Packet {
public:
    // constructor
    GCRemoveCorpseHead() {}

    GCRemoveCorpseHead(ObjectID_t objectID) : m_ObjectID(objectID) {}
    ~GCRemoveCorpseHead() {};


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_REMOVE_CORPSE_HEAD;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }

    // get packet name
    string getPacketName() const {
        return "GCRemoveCorpseHead";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set object id
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t objectID) {
        m_ObjectID = objectID;
    }

private:
    // object id
    ObjectID_t m_ObjectID;
};


//////////////////////////////////////////////////////////////////////
//
// class GCRemoveCorpseHeadFactory;
//
// Factory for GCRemoveCorpseHead
//
//////////////////////////////////////////////////////////////////////

class GCRemoveCorpseHeadFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCRemoveCorpseHead();
    }

    // get packet name
    string getPacketName() const {
        return "GCRemoveCorpseHead";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_REMOVE_CORPSE_HEAD;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCRemoveCorpseHeadHandler;
//
//////////////////////////////////////////////////////////////////////

class GCRemoveCorpseHeadHandler {
public:
    // execute packet's handler
    static void execute(GCRemoveCorpseHead* pPacket, Player* player);
};

#endif
