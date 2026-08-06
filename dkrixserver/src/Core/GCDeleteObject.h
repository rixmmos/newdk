//--------------------------------------------------------------------------------
//
// Filename    : GCDeleteObject.h
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __GC_DELETE_OBJECT_H__
#define __GC_DELETE_OBJECT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class GCDeleteObject;
//


//

//




//
// *CAUTION*
//


//
//--------------------------------------------------------------------------------

class GCDeleteObject : public Packet {
public:
    // constructor
    GCDeleteObject() {}
    GCDeleteObject(ObjectID_t objectID) : m_ObjectID(objectID) {}
    ~GCDeleteObject() {};


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_DELETE_OBJECT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }

    // get packet name
    string getPacketName() const {
        return "GCDeleteObject";
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
// class GCDeleteObjectFactory;
//
// Factory for GCDeleteObject
//
//////////////////////////////////////////////////////////////////////

class GCDeleteObjectFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCDeleteObject();
    }

    // get packet name
    string getPacketName() const {
        return "GCDeleteObject";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_DELETE_OBJECT;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCDeleteObjectHandler;
//
//////////////////////////////////////////////////////////////////////

class GCDeleteObjectHandler {
public:
    // execute packet's handler
    static void execute(GCDeleteObject* pPacket, Player* player);
};

#endif
