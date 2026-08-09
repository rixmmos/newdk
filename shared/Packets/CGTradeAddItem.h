////////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeAddItem.h

// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_TRADE_ADD_ITEM_H__
#define __CG_TRADE_ADD_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeAddItem;
//
////////////////////////////////////////////////////////////////////////////////

class CGTradeAddItem : public Packet {
public:
    CGTradeAddItem() {};
    virtual ~CGTradeAddItem() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_TRADE_ADD_ITEM;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID * 2;
    }
    string getPacketName() const {
        return "CGTradeAddItem";
    }
    string toString() const;

public:
    ObjectID_t getTargetObjectID() const {
        return m_TargetObjectID;
    }
    void setTargetObjectID(ObjectID_t id) {
        m_TargetObjectID = id;
    }

    ObjectID_t getItemObjectID() const {
        return m_ItemObjectID;
    }
    void setItemObjectID(ObjectID_t id) {
        m_ItemObjectID = id;
    }

private:
    ObjectID_t m_TargetObjectID; 
    ObjectID_t m_ItemObjectID;   
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeAddItemFactory;
//
////////////////////////////////////////////////////////////////////////////////

class CGTradeAddItemFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGTradeAddItem();
    }
    string getPacketName() const throw() {
        return "CGTradeAddItem";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_TRADE_ADD_ITEM;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID * 2;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeAddItemHandler;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeAddItem;
class Item;

// Server-only: CGTradeAddItemHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGTradeAddItemHandler {
public:
    static void execute(CGTradeAddItem* pPacket, Player* player);
    static void executeSlayer(CGTradeAddItem* pPacket, Player* player);
    static void executeVampire(CGTradeAddItem* pPacket, Player* player);
    static void executeOusters(CGTradeAddItem* pPacket, Player* player);
    static void makeGCTradeAddItemPacket(GCTradeAddItem* pPacket, ObjectID_t Sender, Item* pItem, CoordInven_t X,
                                         CoordInven_t Y);
    static void executeError(CGTradeAddItem* pPacket, Player* player, BYTE ErrorCode);
};
#endif

#endif
