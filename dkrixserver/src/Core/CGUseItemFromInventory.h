//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseItemFromInventory.h
// Written By  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_ITEM_FROM_INVENTORY_H__
#define __CG_USE_ITEM_FROM_INVENTORY_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromInventory;
//////////////////////////////////////////////////////////////////////////////

class CGUseItemFromInventory : public Packet {
public:
    CGUseItemFromInventory() {};
    virtual ~CGUseItemFromInventory() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_USE_ITEM_FROM_INVENTORY;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szObjectID + szCoordInven + szCoordInven;
    }
    string getPacketName() const {
        return "CGUseItemFromInventory";
    }
    string toString() const;

public:
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    ObjectID_t getInventoryItemObjectID() {
        return m_InventoryItemObjectID;
    }
    void setInventoryItemObjectID(ObjectID_t InventoryItemObjectID) {
        m_InventoryItemObjectID = InventoryItemObjectID;
    }

    CoordInven_t getX() const {
        return m_InvenX;
    }
    void setX(CoordInven_t InvenX) {
        m_InvenX = InvenX;
    }

    CoordInven_t getY() const {
        return m_InvenY;
    }
    void setY(CoordInven_t InvenY) {
        m_InvenY = InvenY;
    }

private:
    ObjectID_t m_ObjectID; 
    
    ObjectID_t m_InventoryItemObjectID;
    CoordInven_t m_InvenX; 
    CoordInven_t m_InvenY; 
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromInventoryFactory;
//////////////////////////////////////////////////////////////////////////////

class CGUseItemFromInventoryFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new CGUseItemFromInventory();
    }
    string getPacketName() const {
        return "CGUseItemFromInventory";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_USE_ITEM_FROM_INVENTORY;
    }
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szObjectID + szCoordInven + szCoordInven;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromInventoryHandler;
//////////////////////////////////////////////////////////////////////////////

class Inventory;
class Item;

class CGUseItemFromInventoryHandler {
public:
    static void execute(CGUseItemFromInventory* pPacket, Player* pPlayer);

protected:
    static void executePotion(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeMagazine(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeETC(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeSerum(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeVampireETC(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeSlayerPortalItem(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeOustersSummonItem(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeKeyItem(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeFirecraker(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeDyePotion(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeResurrectItem(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeTranslator(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeEffectItem(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executePetItem(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executePetFood(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeEventGiftBox(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeTrapItem(CGUseItemFromInventory* pPacket, Player* pPlayer);
    static void executeForceScroll(CGUseItemFromInventory* pPacket, Player* pPlayer);
};

#endif
