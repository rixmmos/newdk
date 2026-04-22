//////////////////////////////////////////////////////////////////////
// Filename    : CGAttack.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ATTACK_H__
#define __CG_ATTACK_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGAttack : public Packet {
public:
    CGAttack() throw();
    ~CGAttack() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_ATTACK;
    }
    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szCoord + szCoord + szDir;
    }
    string getPacketName() const throw() {
        return "CGAttack";
    }
    string toString() const throw();

    Coord_t getX() const throw() {
        return m_X;
    }
    void setX(Coord_t x) throw() {
        m_X = x;
    }

    Coord_t getY() const throw() {
        return m_Y;
    }
    void setY(Coord_t y) throw() {
        m_Y = y;
    }

    Dir_t getDir() const throw() {
        return m_Dir;
    }
    void setDir(Dir_t dir) throw() {
        m_Dir = dir;
    }

    ObjectID_t getObjectID() const throw() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t objectID) throw() {
        m_ObjectID = objectID;
    }

private:
    ObjectID_t m_ObjectID;
    Coord_t m_X;
    Coord_t m_Y;
    Dir_t m_Dir;
};

class CGAttackFactory : public PacketFactory {
public:
    CGAttackFactory() throw() {}
    virtual ~CGAttackFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGAttack();
    }
    string getPacketName() const throw() {
        return "CGAttack";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_ATTACK;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szCoord + szCoord + szDir;
    }
};

class CGAttackHandler {
public:
    static void execute(CGAttack* pCGAttack, Player* player) throw(ProtocolException, Error);
};

#endif
