//////////////////////////////////////////////////////////////////////////////
// Filename    : CGThrowBomb.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_THROW_BOMB_H__
#define __CG_THROW_BOMB_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class CGThrowBomb;
//////////////////////////////////////////////////////////////////////////////

class CGThrowBomb : public Packet {
public:
    CGThrowBomb() {};
    virtual ~CGThrowBomb() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_THROW_BOMB;
    }
    PacketSize_t getPacketSize() const {
        return szCoord * 4 + szBYTE;
    }
    string getPacketName() const {
        return "CGThrowBomb";
    }
    string toString() const;

public:
    Coord_t getZoneX(void) const {
        return m_ZoneX;
    }
    void setZoneX(Coord_t X) {
        m_ZoneX = X;
    }

    Coord_t getZoneY(void) const {
        return m_ZoneY;
    }
    void setZoneY(Coord_t Y) {
        m_ZoneY = Y;
    }

    Coord_t getBombX(void) const {
        return m_BombX;
    }
    void setBombX(Coord_t X) {
        m_BombX = X;
    }

    Coord_t getBombY(void) const {
        return m_BombY;
    }
    void setBombY(Coord_t Y) {
        m_BombY = Y;
    }

    BYTE getAttackSlayerFlag(void) const {
        return m_AttackSlayerFlag;
    }
    void setAttackSlayerFlag(BYTE flag) {
        m_AttackSlayerFlag = flag;
    }

private:
    Coord_t m_ZoneX;
    Coord_t m_ZoneY;
    Coord_t m_BombX;
    Coord_t m_BombY;
    BYTE m_AttackSlayerFlag;
};

//////////////////////////////////////////////////////////////////////////////
// class CGThrowBombFactory;
// Factory for CGThrowBomb
//////////////////////////////////////////////////////////////////////////////

// Client Cpackets copy wrapped this whole class in
// #ifdef __DEBUG_OUTPUT__ (the client never needs a factory to decode
// its own outgoing CG packets, only to build debug tooling); adopting
// the server's unconditional canonical style here per Phase 12's
// reconciliation rules is behaviorally neutral -- the class has no
// side effects, it just becomes compilable in more configurations.
class CGThrowBombFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
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

//////////////////////////////////////////////////////////////////////////////
// class CGThrowBombHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGThrowBombHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGThrowBombHandler {
public:
    static void execute(CGThrowBomb* pCGThrowBomb, Player* pPlayer);
};
#endif

#endif
