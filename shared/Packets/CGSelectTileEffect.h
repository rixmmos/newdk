//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectTileEffect.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_TILE_EFFECT_H__
#define __CG_SELECT_TILE_EFFECT_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSelectTileEffect
//////////////////////////////////////////////////////////////////////////////

class CGSelectTileEffect : public Packet {
public:
    CGSelectTileEffect() {};
    virtual ~CGSelectTileEffect() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_SELECT_TILE_EFFECT;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }
    string getPacketName() const {
        return "CGSelectTileEffect";
    }
    string toString() const;

public:
    ObjectID_t getEffectObjectID(void) const {
        return m_EffectObjectID;
    }
    void setEffectObjectID(ObjectID_t id) {
        m_EffectObjectID = id;
    }

private:
    ObjectID_t m_EffectObjectID;
};


//////////////////////////////////////////////////////////////////////////////
// class CGSelectTileEffectFactory
//////////////////////////////////////////////////////////////////////////////

// Client Cpackets copy wrapped this whole class in
// #ifdef __DEBUG_OUTPUT__ (the client never needs a factory to decode
// its own outgoing CG packets, only to build debug tooling); adopting
// the server's unconditional canonical style here per Phase 12's
// reconciliation rules is behaviorally neutral -- the class has no
// side effects, it just becomes compilable in more configurations.
// createPacket()/getPacketName()/getPacketID()/getPacketMaxSize() are
// private (no `public:` before them) in both the server and client
// pre-migration copies -- preserved as-is; calls through the
// PacketFactory base pointer are unaffected by a derived class
// narrowing access on an override.
class CGSelectTileEffectFactory : public PacketFactory {
    Packet* createPacket() throw() {
        return new CGSelectTileEffect();
    }
    string getPacketName() const throw() {
        return "CGSelectTileEffect";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SELECT_TILE_EFFECT;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class CGSelectTileEffectHandler
//////////////////////////////////////////////////////////////////////////////

class Effect;

// Server-only: CGSelectTileEffectHandler::execute /
// ::executeVampirePortal have no client-side definition or use.
// Guarded (matching the client Cpackets copy's existing guard) since
// no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGSelectTileEffectHandler {
public:
    static void execute(CGSelectTileEffect* pCGSelectTileEffect, Player* pPlayer);
    static void executeVampirePortal(CGSelectTileEffect* pCGSelectTileEffect, Player* pPlayer, Effect* pEffect);
};
#endif

#endif
