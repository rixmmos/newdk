////////////////////////////////////////////////////////////////////////////////
// Filename    : CGBuyStoreItem.h

// Description :



////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_BUY_STORE_ITEM_H__
#define __CG_BUY_STORE_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGBuyStoreItem;
//
////////////////////////////////////////////////////////////////////////////////

class CGBuyStoreItem : public Packet {
public:
    CGBuyStoreItem() {};
    ~CGBuyStoreItem() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_BUY_STORE_ITEM;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szObjectID + szBYTE;
    }
    string getPacketName() const {
        return "CGBuyStoreItem";
    }
    string toString() const;

public:
    ObjectID_t getOwnerObjectID() {
        return m_OwnerObjectID;
    }
    void setOwnerObjectID(ObjectID_t ObjectID) {
        m_OwnerObjectID = ObjectID;
    }

    ObjectID_t getItemObjectID() {
        return m_ItemObjectID;
    }
    void setItemObjectID(ObjectID_t ObjectID) {
        m_ItemObjectID = ObjectID;
    }

    BYTE getIndex(void) const {
        return m_Index;
    }
    void setIndex(BYTE index) {
        m_Index = index;
    }

private:
    ObjectID_t m_OwnerObjectID;
    ObjectID_t m_ItemObjectID;
    BYTE m_Index;
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGBuyStoreItemFactory;
//
////////////////////////////////////////////////////////////////////////////////

class CGBuyStoreItemFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGBuyStoreItem();
    }
    string getPacketName() const throw() {
        return "CGBuyStoreItem";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_BUY_STORE_ITEM;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szObjectID + szBYTE;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGBuyStoreItemHandler;
//
////////////////////////////////////////////////////////////////////////////////

// Unlike most Phase 12 pairs, this Handler class is NOT guarded by
// #ifndef __GAME_CLIENT__: dkrix/Client/CGHandlersStub.cpp already
// provides an empty-body client-side definition of
// CGBuyStoreItemHandler::execute (the "CGStoreOpen precedent" from the
// parked line, rather than the CLGetWorldList pilot's guard-and-omit
// approach). Keeping it unconditional here, matching the server's
// pre-migration style exactly, means CGHandlersStub.cpp needs no
// structural change — only its exception spec was updated to match this
// declaration (see CGHandlersStub.cpp).
class CGBuyStoreItemHandler {
public:
    static void execute(CGBuyStoreItem* pPacket, Player* player);
};

#endif
