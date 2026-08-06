//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseMessageItemFromInventory.h
// Written By  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_MESSAGE_ITEM_FROM_INVENTORY_H__
#define __CG_USE_MESSAGE_ITEM_FROM_INVENTORY_H__

#include "CGUseItemFromInventory.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGUseMessageItemFromInventory;
//////////////////////////////////////////////////////////////////////////////

class CGUseMessageItemFromInventory : public CGUseItemFromInventory {
public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_USE_MESSAGE_ITEM_FROM_INVENTORY;
    }
    PacketSize_t getPacketSize() const {
        return CGUseItemFromInventory::getPacketSize() + szBYTE + m_Message.size();
    }
    string getPacketName() const {
        return "CGUseMessageItemFromInventory";
    }
    string toString() const;

public:
    const string& getMessage() const {
        return m_Message;
    }
    void setMessage(const string& msg) {
        m_Message = msg;
    }

private:
    string m_Message;
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseMessageItemFromInventoryFactory;
//////////////////////////////////////////////////////////////////////////////

class CGUseMessageItemFromInventoryFactory : public CGUseItemFromInventoryFactory {
public:
    Packet* createPacket() {
        return new CGUseMessageItemFromInventory();
    }
    string getPacketName() const {
        return "CGUseMessageItemFromInventory";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_USE_MESSAGE_ITEM_FROM_INVENTORY;
    }
    PacketSize_t getPacketMaxSize() const {
        return CGUseItemFromInventoryFactory::getPacketMaxSize() + szBYTE + 128;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseMessageItemFromInventoryHandler;
//////////////////////////////////////////////////////////////////////////////

class CGUseMessageItemFromInventoryHandler {
public:
    static void execute(CGUseMessageItemFromInventory* pPacket, Player* player);

protected:
    static void executeEventTree(CGUseMessageItemFromInventory* pPacket, Player* player);
    // add by Coffee
    static void executeEventFromMessage(CGUseMessageItemFromInventory* pPacket, Player* player);
};

#endif
