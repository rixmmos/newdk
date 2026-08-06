//////////////////////////////////////////////////////////////////////
//
// Filename    : CGMakeItem.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_MAKE_ITEM_H__
#define __CG_MAKE_ITEM_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGMakeItem;
//
//////////////////////////////////////////////////////////////////////

class CGMakeItem : public Packet {
public:
    // constructor
    CGMakeItem();

    // destructor
    ~CGMakeItem();

public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_MAKE_ITEM;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szItemClass + szItemType;
    }

    // get packet name
    string getPacketName() const {
        return "CGMakeItem";
    }

    // get packet's debug string
    string toString() const;

public:
    ItemType_t getItemType() const {
        return m_ItemType;
    }
    void setItemType(ItemType_t c) {
        m_ItemType = c;
    }

    ItemClass_t getItemClass() const {
        return m_ItemClass;
    }
    void setItemClass(ItemClass_t c) {
        m_ItemClass = c;
    }

private:
    ItemClass_t m_ItemClass;
    ItemType_t m_ItemType;
};


//////////////////////////////////////////////////////////////////////
//
// class CGMakeItemFactory;
//
// Factory for CGMakeItem
//
//////////////////////////////////////////////////////////////////////

class CGMakeItemFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGMakeItem();
    }

    // get packet name
    string getPacketName() const {
        return "CGMakeItem";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_MAKE_ITEM;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szItemClass + szItemType;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGMakeItemHandler;
//
//////////////////////////////////////////////////////////////////////

class CGMakeItemHandler {
public:
    // execute packet's handler
    static void execute(CGMakeItem* pPacket, Player* player);
};

#endif
