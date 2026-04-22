//////////////////////////////////////////////////////////////////////////////
// Filename    : CGExchangeList.h
// Written By  : Exchange System
// Description : Client requests listing list from server
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_EXCHANGE_LIST_H__
#define __CG_EXCHANGE_LIST_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGExchangeList : public Packet {
public:
    CGExchangeList()
        : m_Page(1), m_PageSize(20), m_ItemClass(0xFF), m_ItemType(0xFFFF), m_MinPrice(0), m_MaxPrice(0) {}
    virtual ~CGExchangeList() {}

    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);

    PacketSize_t getPacketSize() const {
        return szint * 4 + sizeof(BYTE) + sizeof(ushort) + szBYTE + m_SellerFilter.size();
    }
    PacketID_t getPacketID() const { return PACKET_CG_EXCHANGE_LIST; }
    string getPacketName() const { return "CGExchangeList"; }
    string toString() const;

    int getPage() const { return m_Page; }
    int getPageSize() const { return m_PageSize; }
    BYTE getItemClass() const { return m_ItemClass; }
    ushort getItemType() const { return m_ItemType; }
    int getMinPrice() const { return m_MinPrice; }
    int getMaxPrice() const { return m_MaxPrice; }
    const string& getSellerFilter() const { return m_SellerFilter; }

    void setPage(int page) { m_Page = page; }
    void setPageSize(int size) { m_PageSize = size; }
    void setItemClass(BYTE itemClass) { m_ItemClass = itemClass; }
    void setItemType(ushort itemType) { m_ItemType = itemType; }
    void setMinPrice(int minPrice) { m_MinPrice = minPrice; }
    void setMaxPrice(int maxPrice) { m_MaxPrice = maxPrice; }
    void setSellerFilter(const string& filter) { m_SellerFilter = filter; }

private:
    int m_Page;
    int m_PageSize;
    BYTE m_ItemClass;
    ushort m_ItemType;
    int m_MinPrice;
    int m_MaxPrice;
    string m_SellerFilter;
};

class CGExchangeListFactory : public PacketFactory {
public:
    Packet* createPacket() throw() { return new CGExchangeList(); }
    string getPacketName() const throw() { return "CGExchangeList"; }
    PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_EXCHANGE_LIST; }
    PacketSize_t getPacketMaxSize() const throw() { return szint * 4 + sizeof(BYTE) + sizeof(ushort) + szBYTE + 64; }
};

class CGExchangeListHandler {
public:
    static void execute(CGExchangeList* pPacket, Player* pPlayer);
};

#endif
