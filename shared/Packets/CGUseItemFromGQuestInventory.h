//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseItemFromGQuestInventory.h
// Written By  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_ITEM_FROM_GQUEST_INVENTORY_H__
#define __CG_USE_ITEM_FROM_GQUEST_INVENTORY_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromGQuestInventory;
//////////////////////////////////////////////////////////////////////////////

class CGUseItemFromGQuestInventory : public Packet {
public:
    CGUseItemFromGQuestInventory() {};
    virtual ~CGUseItemFromGQuestInventory() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_USE_ITEM_FROM_GQUEST_INVENTORY;
    }
    PacketSize_t getPacketSize() const {
        return szBYTE;
    }
    string getPacketName() const {
        return "CGUseItemFromGQuestInventory";
    }
    string toString() const;

public:
    BYTE getIndex() const {
        return m_Index;
    }
    void setIndex(BYTE Index) {
        m_Index = Index;
    }

private:
    BYTE m_Index; 
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromGQuestInventoryFactory;
//////////////////////////////////////////////////////////////////////////////

class CGUseItemFromGQuestInventoryFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGUseItemFromGQuestInventory();
    }
    string getPacketName() const throw() {
        return "CGUseItemFromGQuestInventory";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_USE_ITEM_FROM_GQUEST_INVENTORY;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromGQuestInventoryHandler;
//////////////////////////////////////////////////////////////////////////////

class GQuestInventory;
class Item;

// Server-only: CGUseItemFromGQuestInventoryHandler::execute has no
// client-side definition or use. Guarded (matching the client Cpackets
// copy's existing guard) since no CGHandlersStub.cpp-style client stub
// exists for this family.
#ifndef __GAME_CLIENT__
class CGUseItemFromGQuestInventoryHandler {
public:
    static void execute(CGUseItemFromGQuestInventory* pPacket, Player* pPlayer);
};
#endif

#endif
