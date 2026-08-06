//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMixItem.h 
// Written By  : excel96
// Description : 


//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_MIX_ITEM_H__
#define __CG_MIX_ITEM_H__

#include "../Packet.h"
#include "../PacketFactory.h"
#include "../Assert.h"

//////////////////////////////////////////////////////////////////////////////
// class CGMixItem;
//////////////////////////////////////////////////////////////////////////////

class CGMixItem : public Packet 
{
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_CG_MIX_ITEM; }
	PacketSize_t getPacketSize() const throw() { return szObjectID + szCoordInven + szCoordInven + ( szObjectID * 2 ); }

#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const throw() { return "CGMixItem"; }
	std::string toString() const throw();
#endif
	
public:
	ObjectID_t getObjectID() const throw() { return m_ObjectID; }
	void setObjectID(ObjectID_t ObjectID) throw() { m_ObjectID = ObjectID; }

	CoordInven_t getX() const throw() { return m_InvenX; }
	void setX(CoordInven_t InvenX) throw() { m_InvenX = InvenX; }

	CoordInven_t getY() const throw() { return m_InvenY; }
	void setY(CoordInven_t InvenY) throw() { m_InvenY = InvenY; }

	ObjectID_t getTargetObjectID( uint index ) const throw() { assert(index<2); return m_TargetObjectID[index]; }
	void setTargetObjectID( uint index, ObjectID_t oid ) throw() { assert(index<2); m_TargetObjectID[index] = oid; }

private:
	ObjectID_t   m_ObjectID; 
	CoordInven_t m_InvenX;   
	CoordInven_t m_InvenY;   

	ObjectID_t	m_TargetObjectID[2];	
};


//////////////////////////////////////////////////////////////////////////////
// class CGMixItemFactory;
//////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGMixItemFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new CGMixItem(); }
	std::string getPacketName() const throw() { return "CGMixItem"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_MIX_ITEM; }
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szCoordInven + szCoordInven + ( szObjectID * 2 ); }
};
#endif

//////////////////////////////////////////////////////////////////////////////
// class CGMixItemHandler;
//////////////////////////////////////////////////////////////////////////////

class Inventory;
class Item;

class CGMixItemHandler 
{
public:
	static void execute(CGMixItem* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
