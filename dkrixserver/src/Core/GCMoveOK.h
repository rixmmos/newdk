//////////////////////////////////////////////////////////////////////
//
// Filename    :  GCMoveOK.h
// Written By  :  Elca
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_MOVE_OK_H__
#define __GC_MOVE_OK_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class  GCMoveOK;
//


//
//////////////////////////////////////////////////////////////////////

class GCMoveOK : public Packet {
public:
    // constructor
    GCMoveOK() {}
    GCMoveOK(Coord_t x, Coord_t y, Dir_t dir) : m_X(x), m_Y(y), m_Dir(dir) {}
    ~GCMoveOK() {};


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_MOVE_OK;
    }

    // get packet body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szCoord + szCoord + szDir;
    }

    // get packet's name
    string getPacketName() const {
        return "GCMoveOK";
    }

    // get packet's debug string
    string toString() const;


public:
    // get/set X
    Coord_t getX() const {
        return m_X;
    }
    void setX(Coord_t x) {
        m_X = x;
    }

    // get/set Y
    Coord_t getY() const {
        return m_Y;
    }
    void setY(Coord_t y) {
        m_Y = y;
    }

    // get/set Dir
    Dir_t getDir() const {
        return m_Dir;
    }
    void setDir(Dir_t dir) {
        m_Dir = dir;
    }

    void setXYDir(Coord_t x, Coord_t y, Dir_t dir) {
        m_X = x;
        m_Y = y;
        m_Dir = dir;
    }


private:
    Coord_t m_X; 
    Coord_t m_Y; 
    Dir_t m_Dir; 
};


//////////////////////////////////////////////////////////////////////
//
// class GCMoveOKFactory;
//
// Factory for GCMoveOK
//
//////////////////////////////////////////////////////////////////////

class GCMoveOKFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCMoveOK();
    }

    // get packet name
    string getPacketName() const {
        return "GCMoveOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_MOVE_OK;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szCoord + szCoord + szDir;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class  GCMoveOKHandler;
//
//////////////////////////////////////////////////////////////////////

class GCMoveOKHandler {
public:
    // execute packet's handler
    static void execute(GCMoveOK* pPacket, Player* pPlayer);
};

#endif
