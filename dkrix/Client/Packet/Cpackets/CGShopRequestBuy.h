////////////////////////////////////////////////////////////////////////////////
// Filename    : CGShopRequestBuy.h 

// Description : 



////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_SHOP_REQUEST_BUY_H__
#define __CG_SHOP_REQUEST_BUY_H__

#include "../Packet.h"
#include "../PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGShopRequestBuy;
//
////////////////////////////////////////////////////////////////////////////////

class CGShopRequestBuy : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );
	PacketID_t getPacketID () const throw () { return PACKET_CG_SHOP_REQUEST_BUY; }
	PacketSize_t getPacketSize () const throw () { return szObjectID+szShopRackType+szBYTE+szItemNum+szCoord*2; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "CGShopRequestBuy"; }
		std::string toString () const throw ();
	#endif
	
public:
	ObjectID_t getObjectID() throw () { return m_ObjectID; }
	void setObjectID( ObjectID_t ObjectID ) throw() { m_ObjectID = ObjectID; }

	ShopRackType_t getShopType(void) const throw () { return m_RackType; }
	void setShopType(ShopRackType_t type) throw() { m_RackType = type; }
	
	BYTE getShopIndex(void) const throw() { return m_RackIndex; }
	void setShopIndex(BYTE index) throw() { m_RackIndex = index;}

	ItemNum_t getItemNum(void) const throw() { return m_Num;}
	void setItemNum(ItemNum_t num) throw() { m_Num = num;}

	Coord_t getX(void) const throw() { return m_X; }
	void setX(Coord_t x) throw() { m_X = x;}

	Coord_t getY(void) const throw() { return m_Y; }
	void setY(Coord_t y) throw() { m_Y = y;}

private:
	ObjectID_t     m_ObjectID;  // NPC ID
	ShopRackType_t m_RackType;  // Rack type
	BYTE           m_RackIndex; // Rack index
	ItemNum_t      m_Num;       // Number of item
	Coord_t        m_X;         
	Coord_t        m_Y; 

};


////////////////////////////////////////////////////////////////////////////////
//
// class CGShopRequestBuyFactory;
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGShopRequestBuyFactory : public PacketFactory {
public:
	Packet * createPacket () throw () { return new CGShopRequestBuy(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "CGShopRequestBuy"; }
	#endif
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_SHOP_REQUEST_BUY; }
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID+szShopRackType+szBYTE+szItemNum+szCoord*2; }
};
#endif

////////////////////////////////////////////////////////////////////////////////
//
// class CGShopRequestBuyHandler;
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
class CGShopRequestBuyHandler 
{
public:
	static void execute ( CGShopRequestBuy * pPacket , Player * player ) throw ( ProtocolException , Error );
	static void executeNormal ( CGShopRequestBuy * pPacket , Player * player ) throw ( ProtocolException , Error );
	static void executeMotorcycle ( CGShopRequestBuy * pPacket , Player * player ) throw ( ProtocolException , Error );
	static void sendFailPacket ( CGShopRequestBuy * pPacket , Player * player ) throw ( ProtocolException , Error );
};
#endif

#endif
