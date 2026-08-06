//////////////////////////////////////////////////////////////////////////////
// Filename    : GCGQuestInventory.h
// Written By  : elca@ewestsoft.com
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_GQUEST_INVENTORY_H__
#define __GC_GQUEST_INVENTORY_H__

#include <list>

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

#define MAX_GQUEST_INVENTORY_ITEM_NUM 100

//////////////////////////////////////////////////////////////////////////////
// class GCGQuestInventory;

//////////////////////////////////////////////////////////////////////////////

class GCGQuestInventory : public Packet {
public:
    GCGQuestInventory();
    ~GCGQuestInventory();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_GQUEST_INVENTORY;
    }
    PacketSize_t getPacketSize() const {
        return szBYTE + szItemType * m_ItemList.size();
    }
    string getPacketName() const {
        return "GCGQuestInventory";
    }
    string toString() const;

public:
    list<ItemType_t>& getItemList() {
        return m_ItemList;
    }
    const list<ItemType_t>& getItemList() const {
        return m_ItemList;
    }

private:
    list<ItemType_t> m_ItemList;
};


//////////////////////////////////////////////////////////////////////////////
// class GCGQuestInventoryFactory;
//////////////////////////////////////////////////////////////////////////////

class GCGQuestInventoryFactory : public PacketFactory {
public:
    GCGQuestInventoryFactory() {}
    virtual ~GCGQuestInventoryFactory() {}

public:
    Packet* createPacket() {
        return new GCGQuestInventory();
    }
    string getPacketName() const {
        return "GCGQuestInventory";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_GQUEST_INVENTORY;
    }
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + szItemType * MAX_GQUEST_INVENTORY_ITEM_NUM;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class GCGQuestInventoryHandler;
//////////////////////////////////////////////////////////////////////////////

class GCGQuestInventoryHandler {
public:
    static void execute(GCGQuestInventory* pGCGQuestInventory, Player* pPlayer);
};

#endif
