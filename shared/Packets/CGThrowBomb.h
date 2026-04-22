//////////////////////////////////////////////////////////////////////////////
// Filename    : CGThrowBomb.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_THROW_BOMB_H__
#define __CG_THROW_BOMB_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGThrowBomb : public Packet {
public:
    CGThrowBomb() throw() {}
    virtual ~CGThrowBomb() throw() {}

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_THROW_BOMB;
    }
    PacketSize_t getPacketSize() const throw() {
        return szCoord * 4 + szBYTE;
    }
    string getPacketName() const throw() {
        return "CGThrowBomb";
    }
    string toString() const throw();

    Coord_t getZoneX() const throw() {
        return m_ZoneX;
    }
    void setZoneX(Coord_t x) throw() {
        m_ZoneX = x;
    }

    Coord_t getZoneY() const throw() {
        return m_ZoneY;
    }
    void setZoneY(Coord_t y) throw() {
        m_ZoneY = y;
    }

    Coord_t getBombX() const throw() {
        return m_BombX;
    }
    void setBombX(Coord_t x) throw() {
        m_BombX = x;
    }

    Coord_t getBombY() const throw() {
        return m_BombY;
    }
    void setBombY(Coord_t y) throw() {
        m_BombY = y;
    }

    BYTE getAttackSlayerFlag() const throw() {
        return m_AttackSlayerFlag;
    }
    void setAttackSlayerFlag(BYTE flag) throw() {
        m_AttackSlayerFlag = flag;
    }

private:
    Coord_t m_ZoneX;
    Coord_t m_ZoneY;
    Coord_t m_BombX;
    Coord_t m_BombY;
    BYTE m_AttackSlayerFlag;
};

class CGThrowBombFactory : public PacketFactory {
public:
    CGThrowBombFactory() throw() {}
    virtual ~CGThrowBombFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGThrowBomb();
    }
    string getPacketName() const throw() {
        return "CGThrowBomb";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_THROW_BOMB;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szCoord * 4 + szBYTE;
    }
};

class CGThrowBombHandler {
public:
    static void execute(CGThrowBomb* pCGThrowBomb, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
