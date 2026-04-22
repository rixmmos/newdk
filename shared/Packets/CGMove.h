//////////////////////////////////////////////////////////////////////
//
// Filename    : CGMove.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_MOVE_H__
#define __CG_MOVE_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGMove : public Packet {
public:
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() { return PACKET_CG_MOVE; }
    PacketSize_t getPacketSize() const throw() { return szCoord + szCoord + szDir; }
    string getPacketName() const throw() { return "CGMove"; }
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

class CGMoveFactory : public PacketFactory {
public:
    Packet* createPacket() throw() { return new CGMove(); }
    string getPacketName() const throw() { return "CGMove"; }
    PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_MOVE; }
    PacketSize_t getPacketMaxSize() const throw() { return szCoord + szCoord + szDir; }
};

class CGMoveHandler {
public:
    static void execute(CGMove* pPacket, Player* player);
};

#endif
