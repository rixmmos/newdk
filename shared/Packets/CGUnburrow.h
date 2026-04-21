//////////////////////////////////////////////////////////////////////
// Filename    : CGUnburrow.h
// Written By  : crazydog
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_UNBURROW_H__
#define __CG_UNBURROW_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGUnburrow : public Packet {
public:
    CGUnburrow() {}
    virtual ~CGUnburrow() {}

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() { return PACKET_CG_UNBURROW; }
    PacketSize_t getPacketSize() const throw() { return szCoord + szCoord + szDir; }
    string getPacketName() const throw() { return "CGUnburrow"; }
    string toString() const throw();

    Coord_t getX() const throw() { return m_X; }
    void setX(Coord_t x) throw() { m_X = x; }
    Coord_t getY() const throw() { return m_Y; }
    void setY(Coord_t y) throw() { m_Y = y; }
    Dir_t getDir() const throw() { return m_Dir; }
    void setDir(Dir_t dir) throw() { m_Dir = dir; }

private:
    Coord_t m_X;
    Coord_t m_Y;
    Dir_t m_Dir;
};

class CGUnburrowFactory : public PacketFactory {
public:
    Packet* createPacket() throw() { return new CGUnburrow(); }
    string getPacketName() const throw() { return "CGUnburrow"; }
    PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_UNBURROW; }
    PacketSize_t getPacketMaxSize() const throw() { return szCoord + szCoord + szDir; }
};

class CGUnburrowHandler {
public:
    static void execute(CGUnburrow* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
