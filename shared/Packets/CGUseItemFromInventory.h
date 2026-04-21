//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseItemFromInventory.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_ITEM_FROM_INVENTORY_H__
#define __CG_USE_ITEM_FROM_INVENTORY_H__

#include "Packet.h"
#include "PacketFactory.h"

class Inventory;
class Item;

class CGUseItemFromInventory : public Packet {
public:
    CGUseItemFromInventory() throw();
    virtual ~CGUseItemFromInventory() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_USE_ITEM_FROM_INVENTORY;
    }
    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szCoordInven + szCoordInven;
    }
    string getPacketName() const throw() {
        return "CGUseItemFromInventory";
    }
    string toString() const throw();

    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t objectID) {
        m_ObjectID = objectID;
    }

    ObjectID_t getInventoryItemObjectID() throw() {
        return m_InventoryItemObjectID;
    }
    void setInventoryItemObjectID(ObjectID_t inventoryItemObjectID) {
        m_InventoryItemObjectID = inventoryItemObjectID;
    }

    CoordInven_t getX() const {
        return m_InvenX;
    }
    void setX(CoordInven_t invenX) {
        m_InvenX = invenX;
    }

    CoordInven_t getY() const {
        return m_InvenY;
    }
    void setY(CoordInven_t invenY) {
        m_InvenY = invenY;
    }

private:
    ObjectID_t m_ObjectID;
    ObjectID_t m_InventoryItemObjectID;
    CoordInven_t m_InvenX;
    CoordInven_t m_InvenY;
};

class CGUseItemFromInventoryFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGUseItemFromInventory();
    }
    string getPacketName() const throw() {
        return "CGUseItemFromInventory";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_USE_ITEM_FROM_INVENTORY;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szCoordInven + szCoordInven;
    }
};

class CGUseItemFromInventoryHandler {
public:
    static void execute(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);

protected:
    static void executePotion(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeMagazine(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeETC(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeSerum(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeVampireETC(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeSlayerPortalItem(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeOustersSummonItem(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeKeyItem(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeFirecraker(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeDyePotion(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeResurrectItem(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeTranslator(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeEffectItem(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executePetItem(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executePetFood(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeEventGiftBox(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeTrapItem(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void executeForceScroll(CGUseItemFromInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
