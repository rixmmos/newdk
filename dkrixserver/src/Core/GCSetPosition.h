//----------------------------------------------------------------------
//
// Filename    : GCSetPosition.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __GC_SET_POSITION_H__
#define __GC_SET_POSITION_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class GCSetPosition;
//


//
//----------------------------------------------------------------------

class GCSetPosition : public Packet {
public:
    GCSetPosition() {};
    ~GCSetPosition() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_SET_POSITION;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szCoord + szCoord + szDir;
    }

    // get packet name
    string getPacketName() const {
        return "GCSetPosition";
    }

    // get packet's debug string
    string toString() const;

    // get/set X Coordicate
    Coord_t getX() const {
        return m_X;
    }
    void setX(Coord_t x) {
        m_X = x;
    }

    // get/set Y Coordicate
    Coord_t getY() const {
        return m_Y;
    }
    void setY(Coord_t y) {
        m_Y = y;
    }

    // get/set Direction
    Dir_t getDir() const {
        return m_Dir;
    }
    void setDir(Dir_t dir) {
        m_Dir = dir;
    }

private:
    Coord_t m_X; 
    Coord_t m_Y; 
    Dir_t m_Dir; 
};


//////////////////////////////////////////////////////////////////////
//
// class GCSetPositionFactory;
//
// Factory for GCSetPosition
//
//////////////////////////////////////////////////////////////////////

class GCSetPositionFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCSetPosition();
    }

    // get packet name
    string getPacketName() const {
        return "GCSetPosition";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_SET_POSITION;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szCoord + szCoord + szDir;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCSetPositionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCSetPositionHandler {
public:
    // execute packet's handler
    static void execute(GCSetPosition* pPacket, Player* player);
};

#endif
