////////////////////////////////////////////////////////////////////////////////
// Filename    : CGBuyStoreItem.h 

// Description : 



////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_BUY_STORE_ITEM_H__
#define __CG_BUY_STORE_ITEM_H__

#include "../Packet.h"
#include "../PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGBuyStoreItem;
//
////////////////////////////////////////////////////////////////////////////////

class CGBuyStoreItem : public Packet 
{
public:
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_BUY_STORE_ITEM; }
	PacketSize_t getPacketSize() const throw() { return szObjectID+szObjectID+szBYTE; }
	string getPacketName() const throw() { return "CGBuyStoreItem"; }
	string toString() const throw();
	
public:
	ObjectID_t getOwnerObjectID() throw() { return m_OwnerObjectID; }
	void setOwnerObjectID(ObjectID_t ObjectID) throw() { m_OwnerObjectID = ObjectID; }

	ObjectID_t getItemObjectID() throw() { return m_ItemObjectID; }
	void setItemObjectID(ObjectID_t ObjectID) throw() { m_ItemObjectID = ObjectID; }

	BYTE getIndex(void) const throw() { return m_Index; }
	void setIndex(BYTE index) throw() { m_Index = index;}

private:
	ObjectID_t		m_OwnerObjectID;
	ObjectID_t		m_ItemObjectID;
	BYTE			m_Index;
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGBuyStoreItemFactory;
//
////////////////////////////////////////////////////////////////////////////////

class CGBuyStoreItemFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGBuyStoreItem(); }
	string getPacketName() const throw() { return "CGBuyStoreItem"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_BUY_STORE_ITEM; }
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID+szObjectID+szBYTE; }
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGBuyStoreItemHandler;
//
////////////////////////////////////////////////////////////////////////////////

class CGBuyStoreItemHandler 
{
public:
	static void execute(CGBuyStoreItem* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
