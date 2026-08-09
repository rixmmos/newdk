//////////////////////////////////////////////////////////////////////
//
// Filename    : CGReloadFromInventory.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_RELOAD_FROM_INVENTORY_H__
#define __CG_RELOAD_FROM_INVENTORY_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGReloadFromInventory;
//
//////////////////////////////////////////////////////////////////////

class CGReloadFromInventory : public Packet {
public:
    // constructor
    CGReloadFromInventory();

    // destructor
    ~CGReloadFromInventory();

public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_RELOAD_FROM_INVENTORY;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szCoordInven + szCoordInven;
    }

    // get packet name
    string getPacketName() const {
        return "CGReloadFromInventory";
    }

    // get packet's debug string
    string toString() const;

public:
    // get / set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    // get / set Inventory X
    CoordInven_t getX() const {
        return m_InvenX;
    }
    void setX(CoordInven_t InvenX) {
        m_InvenX = InvenX;
    }

    // get / set Inventory Y
    CoordInven_t getY() const {
        return m_InvenY;
    }
    void setY(CoordInven_t InvenY) {
        m_InvenY = InvenY;
    }


private:
    // ObjectID
    ObjectID_t m_ObjectID;

    
    CoordInven_t m_InvenX;
    CoordInven_t m_InvenY;
};


//////////////////////////////////////////////////////////////////////
//
// class CGReloadFromInventoryFactory;
//
// Factory for CGReloadFromInventory
//
//////////////////////////////////////////////////////////////////////

class CGReloadFromInventoryFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGReloadFromInventory();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGReloadFromInventory";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_RELOAD_FROM_INVENTORY;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szCoordInven + szCoordInven;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGReloadFromInventoryHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGReloadFromInventoryHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGReloadFromInventoryHandler {
public:
    // execute packet's handler
    static void execute(CGReloadFromInventory* pPacket, Player* player);
};
#endif

#endif
