//////////////////////////////////////////////////////////////////////////////
// Filename    : GCGQuestInventory.h 
// Written By  : elca@ewestsoft.com
// Description : 

//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_GQUEST_INVENTORY_H__
#define __GC_GQUEST_INVENTORY_H__

#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

#include <vector>

#define MAX_GQUEST_INVENTORY_ITEM_NUM	100

//////////////////////////////////////////////////////////////////////////////
// class GCGQuestInventory;

//////////////////////////////////////////////////////////////////////////////

class GCGQuestInventory : public Packet 
{
public:
	GCGQuestInventory() throw();
	~GCGQuestInventory() throw();
	
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_GQUEST_INVENTORY; }
	PacketSize_t getPacketSize() const throw() { return szBYTE + szItemType * m_ItemList.size(); }
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCGQuestInventory"; }
	string toString() const throw();
#endif
public:
	std::vector<ItemType_t>&	getItemList() { return m_ItemList; }
	const std::vector<ItemType_t>&	getItemList() const { return m_ItemList; }

private:
	std::vector<ItemType_t>	m_ItemList;
};


//////////////////////////////////////////////////////////////////////////////
// class GCGQuestInventoryFactory;
//////////////////////////////////////////////////////////////////////////////

class GCGQuestInventoryFactory : public PacketFactory 
{
public :
	GCGQuestInventoryFactory() throw() {}
	virtual ~GCGQuestInventoryFactory() throw() {}
	
public:
	Packet* createPacket() throw() { return new GCGQuestInventory(); }
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCGQuestInventory"; }
#endif
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_GQUEST_INVENTORY; }
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + szItemType * MAX_GQUEST_INVENTORY_ITEM_NUM; }
};

//////////////////////////////////////////////////////////////////////////////
// class GCGQuestInventoryHandler;
//////////////////////////////////////////////////////////////////////////////

class GCGQuestInventoryHandler 
{
public:
	static void execute(GCGQuestInventory* pGCGQuestInventory, Player* pPlayer) throw ( ProtocolException , Error );

};

#endif
