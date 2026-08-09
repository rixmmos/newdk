//////////////////////////////////////////////////////////////////////
//
// Filename    : CGReloadFromQuickSlot.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_RELOAD_FROM_QUICKSLOT_H__
#define __CG_RELOAD_FROM_QUICKSLOT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGReloadFromQuickSlot;
//
//////////////////////////////////////////////////////////////////////

class CGReloadFromQuickSlot : public Packet {
public:
    // constructor
    CGReloadFromQuickSlot();

    // destructor
    ~CGReloadFromQuickSlot();

public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_RELOAD_FROM_QUICKSLOT;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szSlotID;
    }

    // get packet name
    string getPacketName() const {
        return "CGReloadFromQuickSlot";
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

    // get / set QuickSlotID
    SlotID_t getSlotID() const {
        return m_SlotID;
    }
    void setSlotID(SlotID_t SlotID) {
        m_SlotID = SlotID;
    }


private:
    // ObjectID
    ObjectID_t m_ObjectID;

    
    SlotID_t m_SlotID;
};


//////////////////////////////////////////////////////////////////////
//
// class CGReloadFromQuickSlotFactory;
//
// Factory for CGReloadFromQuickSlot
//
//////////////////////////////////////////////////////////////////////

class CGReloadFromQuickSlotFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGReloadFromQuickSlot();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGReloadFromQuickSlot";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_RELOAD_FROM_QUICKSLOT;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szSlotID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGReloadFromQuickSlotHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGReloadFromQuickSlotHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGReloadFromQuickSlotHandler {
public:
    // execute packet's handler
    static void execute(CGReloadFromQuickSlot* pPacket, Player* player);
};
#endif

#endif
