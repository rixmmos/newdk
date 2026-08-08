//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSelectRegenZone.h
// Written By  : excel96
// Description : Packet for requesting a new regen zone when moving to another zone.
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_REGEN_ZONE_H__
#define __CG_SELECT_REGEN_ZONE_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class CGSelectRegenZone;
//////////////////////////////////////////////////////////////////////////////

class CGSelectRegenZone : public Packet {
public:
    CGSelectRegenZone() {};
    virtual ~CGSelectRegenZone() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_SELECT_REGEN_ZONE;
    }
    PacketSize_t getPacketSize() const {
        return szBYTE;
    }
    string getPacketName() const {
        return "CGSelectRegenZone";
    }
    string toString() const;

public:
    BYTE getRegenZoneID() const {
        return m_RegenZoneID;
    }
    void setRegenZoneID(BYTE RegenZoneID) {
        m_RegenZoneID = RegenZoneID;
    }

private:
    BYTE m_RegenZoneID;
};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectRegenZoneFactory;
//////////////////////////////////////////////////////////////////////////////

class CGSelectRegenZoneFactory : public PacketFactory {
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGSelectRegenZone();
    }
    string getPacketName() const throw() {
        return "CGSelectRegenZone";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SELECT_REGEN_ZONE;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGSelectRegenZoneHandler;
//////////////////////////////////////////////////////////////////////////////

// Unlike most Phase 12 pairs, this Handler class is NOT guarded by
// #ifndef __GAME_CLIENT__: dkrix/Client/CGHandlersStub.cpp already
// provides an empty-body client-side definition of
// CGSelectRegenZoneHandler::execute (the "CGStoreOpen precedent").
// Keeping it unconditional here means CGHandlersStub.cpp needs no
// structural change — only its #include and exception spec were updated.
class CGSelectRegenZoneHandler {
public:
    static void execute(CGSelectRegenZone* pCGSelectRegenZone, Player* pPlayer);
};

#endif
