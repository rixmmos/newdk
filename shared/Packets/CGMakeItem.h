//////////////////////////////////////////////////////////////////////
//
// Filename    : CGMakeItem.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_MAKE_ITEM_H__
#define __CG_MAKE_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGMakeItem : public Packet {
public:
    CGMakeItem() throw();
    ~CGMakeItem() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_MAKE_ITEM;
    }

    PacketSize_t getPacketSize() const throw() {
        return szItemClass + szItemType;
    }

    std::string getPacketName() const throw() {
        return "CGMakeItem";
    }

    std::string toString() const throw();

    ItemType_t getItemType() const throw() {
        return m_ItemType;
    }

    void setItemType(ItemType_t c) throw() {
        m_ItemType = c;
    }

    ItemClass_t getItemClass() const throw() {
        return m_ItemClass;
    }

    void setItemClass(ItemClass_t c) throw() {
        m_ItemClass = c;
    }

private:
    ItemClass_t m_ItemClass;
    ItemType_t m_ItemType;
};

class CGMakeItemFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGMakeItem();
    }

    std::string getPacketName() const throw() {
        return "CGMakeItem";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_MAKE_ITEM;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szItemClass + szItemType;
    }
};

#ifndef __GAME_CLIENT__
class CGMakeItemHandler {
public:
    static void execute(CGMakeItem* pPacket, Player* player);
};
#endif

#endif
