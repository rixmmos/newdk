//////////////////////////////////////////////////////////////////////
//
// Filename    : CGReloadFromInventory.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_RELOAD_FROM_INVENTORY_H__
#define __CG_RELOAD_FROM_INVENTORY_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGReloadFromInventory : public Packet {
public:
    CGReloadFromInventory() throw();
    ~CGReloadFromInventory() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_RELOAD_FROM_INVENTORY;
    }
    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szCoordInven + szCoordInven;
    }
    string getPacketName() const throw() {
        return "CGReloadFromInventory";
    }
    string toString() const throw();

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

private:
    ObjectID_t m_ObjectID;
    CoordInven_t m_InvenX;
    CoordInven_t m_InvenY;
};

class CGReloadFromInventoryFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGReloadFromInventory();
    }
    string getPacketName() const throw() {
        return "CGReloadFromInventory";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_RELOAD_FROM_INVENTORY;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szCoordInven + szCoordInven;
    }
};

class CGReloadFromInventoryHandler {
public:
    static void execute(CGReloadFromInventory* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
