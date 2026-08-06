//////////////////////////////////////////////////////////////////////
//
// Filename    : CGUnburrow.h
// Written By  : crazydog
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_UNBURROW_H__
#define __CG_UNBURROW_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGUnburrow;
//
//////////////////////////////////////////////////////////////////////

class CGUnburrow : public Packet {
public:
    CGUnburrow() {};
    virtual ~CGUnburrow() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_UNBURROW;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szCoord + szCoord + szDir;
    }

    // get packet name
    string getPacketName() const {
        return "CGUnburrow";
    }

    // get packet's debug string
    string toString() const;

public:
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
// class CGUnburrowFactory;
//
// Factory for CGUnburrow
//
//////////////////////////////////////////////////////////////////////

class CGUnburrowFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGUnburrow();
    }

    // get packet name
    string getPacketName() const {
        return "CGUnburrow";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_UNBURROW;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szCoord + szCoord + szDir;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGUnburrowHandler;
//
//////////////////////////////////////////////////////////////////////

class CGUnburrowHandler {
public:
    // execute packet's handler
    static void execute(CGUnburrow* pPacket, Player* player);
};

#endif
