//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAddZoneToInventory.h
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_ZONE_TO_INVENTORY_H__
#define __CG_ADD_ZONE_TO_INVENTORY_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGAddZoneToInventory;
//////////////////////////////////////////////////////////////////////////////

class CGAddZoneToInventory : public Packet {
public:
    CGAddZoneToInventory();
    ~CGAddZoneToInventory();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_ADD_ZONE_TO_INVENTORY;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szCoord + szCoord + szCoordInven + szCoordInven;
    }
    string getPacketName() const {
        return "CGAddZoneToInventory";
    }
    string toString() const;

public:
    ObjectID_t getObjectID() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    Coord_t getZoneX() const {
        return m_ZoneX;
    }
    void setZoneX(Coord_t ZoneX) {
        m_ZoneX = ZoneX;
    }

    Coord_t getZoneY() const {
        return m_ZoneY;
    }
    void setZoneY(Coord_t ZoneY) {
        m_ZoneY = ZoneY;
    }

    CoordInven_t getInvenX() const {
        return m_InvenX;
    }
    void setInvenX(CoordInven_t InvenX) {
        m_InvenX = InvenX;
    }

    CoordInven_t getInvenY() const {
        return m_InvenY;
    }
    void setInvenY(CoordInven_t InvenY) {
        m_InvenY = InvenY;
    }

private:
    ObjectID_t m_ObjectID;
    Coord_t m_ZoneX;
    Coord_t m_ZoneY;
    CoordInven_t m_InvenX;
    CoordInven_t m_InvenY;
};

//////////////////////////////////////////////////////////////////////////////
// class CGAddZoneToInventoryFactory;
//////////////////////////////////////////////////////////////////////////////

class CGAddZoneToInventoryFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGAddZoneToInventory();
    }
    string getPacketName() const throw() {
        return "CGAddZoneToInventory";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_ADD_ZONE_TO_INVENTORY;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szCoord + szCoord + szCoordInven + szCoordInven;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGAddZoneToInventoryHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGAddZoneToInventoryHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGAddZoneToInventoryHandler {
public:
    static void execute(CGAddZoneToInventory* pPacket, Player* player);
};
#endif

#endif
