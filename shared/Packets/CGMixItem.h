//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMixItem.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_MIX_ITEM_H__
#define __CG_MIX_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"
class CGMixItem : public Packet {
public:
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_MIX_ITEM;
    }

    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szCoordInven + szCoordInven + (szObjectID * 2);
    }

    std::string getPacketName() const throw() {
        return "CGMixItem";
    }

    std::string toString() const throw();

    ObjectID_t getObjectID() const throw() {
        return m_ObjectID;
    }

    void setObjectID(ObjectID_t objectID) throw() {
        m_ObjectID = objectID;
    }

    CoordInven_t getX() const throw() {
        return m_InvenX;
    }

    void setX(CoordInven_t invenX) throw() {
        m_InvenX = invenX;
    }

    CoordInven_t getY() const throw() {
        return m_InvenY;
    }

    void setY(CoordInven_t invenY) throw() {
        m_InvenY = invenY;
    }

    ObjectID_t getTargetObjectID(uint index) const throw() {
        if (index >= 2) return 0;
        return m_TargetObjectID[index];
    }

    void setTargetObjectID(uint index, ObjectID_t oid) throw() {
        if (index >= 2) return;
        m_TargetObjectID[index] = oid;
    }

private:
    ObjectID_t m_ObjectID;
    CoordInven_t m_InvenX;
    CoordInven_t m_InvenY;
    ObjectID_t m_TargetObjectID[2];
};

class CGMixItemFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGMixItem();
    }

    std::string getPacketName() const throw() {
        return "CGMixItem";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_MIX_ITEM;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szCoordInven + szCoordInven + (szObjectID * 2);
    }
};

class Inventory;
class Item;

class CGMixItemHandler {
public:
    static void execute(CGMixItem* pPacket, Player* pPlayer);

#ifdef __GAME_SERVER__
    static void executeMix(CGMixItem* pPacket, Player* pPlayer, Item* pItem);
    static void executeDetach(CGMixItem* pPacket, Player* pPlayer, Item* pItem);
    static void executeClearOption(CGMixItem* pPacket, Player* pPlayer, Item* pItem);
#endif
};

#endif
