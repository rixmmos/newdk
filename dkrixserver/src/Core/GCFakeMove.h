//////////////////////////////////////////////////////////////////////
//
// Filename    : GCFakeMove.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_FAKE_MOVE_H__
#define __GC_FAKE_MOVE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCFakeMove;
//


//
//////////////////////////////////////////////////////////////////////

class GCFakeMove : public Packet {
public:
    // constructor
    GCFakeMove() {}
    GCFakeMove(ObjectID_t objectID, Coord_t x, Coord_t y, Coord_t x2, Coord_t y2)
        : m_ObjectID(objectID), m_ToX(x2), m_ToY(y2) {}
    ~GCFakeMove() {};


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_FAKE_MOVE;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szObjectID + 2 * szCoord;
    }

    // get packet's name
    string getPacketName() const {
        return "GCFakeMove";
    }

    // get packet's debug string
    string toString() const;


public:
    // get/set Creature ID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t objectID) {
        m_ObjectID = objectID;
    }

    void setXY(Coord_t x2, Coord_t y2) {
        m_ToX = x2;
        m_ToY = y2;
    }

    // get
    Coord_t getToX() const {
        return m_ToX;
    }
    Coord_t getToY() const {
        return m_ToY;
    }


private:
    ObjectID_t m_ObjectID; 
    Coord_t m_ToX, m_ToY;  
};


//////////////////////////////////////////////////////////////////////
//
// class GCFakeMoveFactory;
//
// Factory for GCFakeMove
//
//////////////////////////////////////////////////////////////////////

class GCFakeMoveFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCFakeMove();
    }

    // get packet name
    string getPacketName() const {
        return "GCFakeMove";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_FAKE_MOVE;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + 2 * szCoord;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCFakeMoveHandler;
//
//////////////////////////////////////////////////////////////////////

class GCFakeMoveHandler {
public:
    // execute packet's handler
    static void execute(GCFakeMove* pPacket, Player* pPlayer);
};

#endif
