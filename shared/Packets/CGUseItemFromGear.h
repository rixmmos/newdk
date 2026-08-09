//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseItemFromGear.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_ITEM_FROM_GEAR_H__
#define __CG_USE_ITEM_FROM_GEAR_H__

#include "Packet.h"
#include "PacketFactory.h"

class GamePlayer;

//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromGear;
//////////////////////////////////////////////////////////////////////////////

class CGUseItemFromGear : public Packet {
public:
    CGUseItemFromGear() {};
    virtual ~CGUseItemFromGear() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_USE_ITEM_FROM_GEAR;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE;
    }
    string getPacketName() const {
        return "CGUseItemFromGear";
    }
    string toString() const;

public:
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    BYTE getPart() const {
        return m_Part;
    }
    void setPart(BYTE part) {
        m_Part = part;
    }

private:
    ObjectID_t m_ObjectID; 
    BYTE m_Part;           
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromGearFactory;
//////////////////////////////////////////////////////////////////////////////

class CGUseItemFromGearFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGUseItemFromGear();
    }
    string getPacketName() const throw() {
        return "CGUseItemFromGear";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_USE_ITEM_FROM_GEAR;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromGearHandler;
//////////////////////////////////////////////////////////////////////////////

class Item;

// Server-only: CGUseItemFromGearHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGUseItemFromGearHandler {
public:
    static void execute(CGUseItemFromGear* pPacket, Player* player);

protected:
    static void executeCoupleRing(CGUseItemFromGear* pPacket, GamePlayer* pGamePlayer);
};
#endif

#endif
