//////////////////////////////////////////////////////////////////////
//
// Filename    : CGDisplayItem.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_DISPLAY_ITEM_H__
#define __CG_DISPLAY_ITEM_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGDisplayItem;
//
//////////////////////////////////////////////////////////////////////

class CGDisplayItem : public Packet {
public:
    CGDisplayItem() {};
    ~CGDisplayItem() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_DISPLAY_ITEM;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szCoordInven + szCoordInven + szObjectID + szGold + szBYTE;
    }

    // get packet name
    string getPacketName() const {
        return "CGDisplayItem";
    }

    // get packet's debug string
    string toString() const;

    CoordInven_t getX() const {
        return m_X;
    }
    CoordInven_t getY() const {
        return m_Y;
    }
    void setXY(CoordInven_t x, CoordInven_t y) {
        m_X = x;
        m_Y = y;
    }

    ObjectID_t getItemObjectID() const {
        return m_ItemObjectID;
    }
    void setItemObjectID(ObjectID_t oid) {
        m_ItemObjectID = oid;
    }

    Gold_t getPrice() const {
        return m_Price;
    }
    void setPrice(Gold_t price) {
        m_Price = price;
    }

    BYTE getIndex() const {
        return m_Index;
    }
    void setIndex(BYTE index) {
        m_Index = index;
    }

private:
    CoordInven_t m_X, m_Y;
    ObjectID_t m_ItemObjectID;
    Gold_t m_Price;
    BYTE m_Index;
};


//////////////////////////////////////////////////////////////////////
//
// class CGDisplayItemFactory;
//
// Factory for CGDisplayItem
//
//////////////////////////////////////////////////////////////////////

class CGDisplayItemFactory : public PacketFactory {
public:
    // constructor
    CGDisplayItemFactory() {}

    // destructor
    virtual ~CGDisplayItemFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGDisplayItem();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGDisplayItem";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_DISPLAY_ITEM;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szCoordInven + szCoordInven + szObjectID + szGold + szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGDisplayItemHandler;
//
//////////////////////////////////////////////////////////////////////

// Unlike most Phase 12 pairs, this Handler class is NOT guarded by
// #ifndef __GAME_CLIENT__: dkrix/Client/CGHandlersStub.cpp already
// provides an empty-body client-side definition of
// CGDisplayItemHandler::execute (the "CGStoreOpen precedent"). Keeping
// it unconditional here means CGHandlersStub.cpp needs no structural
// change — only its #include and exception spec were updated.
class CGDisplayItemHandler {
public:
    // execute packet's handler
    static void execute(CGDisplayItem* pCGDisplayItem, Player* pPlayer);
};

#endif
