//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseItemFromGQuestInventory.h 
// Written By  : excel96
// Description : 


//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_ITEM_FROM_GQUEST_INVENTORY_H__
#define __CG_USE_ITEM_FROM_GQUEST_INVENTORY_H__

#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromGQuestInventory;
//////////////////////////////////////////////////////////////////////////////

class CGUseItemFromGQuestInventory : public Packet 
{
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_USE_ITEM_FROM_GQUEST_INVENTORY; }
	PacketSize_t getPacketSize() const throw() { return szBYTE; }
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "CGUseItemFromGQuestInventory"; }
	string toString() const throw();
#endif	
public:
	BYTE getIndex() const throw() { return m_Index; }
	void setIndex(BYTE Index) throw() { m_Index = Index; }

private:
	BYTE   m_Index; 
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromGQuestInventoryFactory;
//////////////////////////////////////////////////////////////////////////////

class CGUseItemFromGQuestInventoryFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGUseItemFromGQuestInventory(); }
//#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "CGUseItemFromGQuestInventory"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_USE_ITEM_FROM_GQUEST_INVENTORY; }
//#endif
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE; }
};


//////////////////////////////////////////////////////////////////////////////
// class CGUseItemFromGQuestInventoryHandler;
//////////////////////////////////////////////////////////////////////////////

class GQuestInventory;
class Item;
#ifndef __GAME_CLIENT__
class CGUseItemFromGQuestInventoryHandler 
{
public:
	static void execute(CGUseItemFromGQuestInventory* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};
#endif
#endif
