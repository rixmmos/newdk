//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseItemFromGQuestInventory.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_ITEM_FROM_GQUEST_INVENTORY_H__
#define __CG_USE_ITEM_FROM_GQUEST_INVENTORY_H__

#include "Packet.h"
#include "PacketFactory.h"

class GQuestInventory;
class Item;

class CGUseItemFromGQuestInventory : public Packet {
public:
    CGUseItemFromGQuestInventory() {}
    virtual ~CGUseItemFromGQuestInventory() {}

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_USE_ITEM_FROM_GQUEST_INVENTORY;
    }
    PacketSize_t getPacketSize() const throw() {
        return szBYTE;
    }
    string getPacketName() const throw() {
        return "CGUseItemFromGQuestInventory";
    }
    string toString() const throw();

    BYTE getIndex() const throw() {
        return m_Index;
    }
    void setIndex(BYTE index) throw() {
        m_Index = index;
    }

private:
    BYTE m_Index;
};

class CGUseItemFromGQuestInventoryFactory : public PacketFactory {
public:
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

class CGUseItemFromGQuestInventoryHandler {
public:
    static void execute(CGUseItemFromGQuestInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
