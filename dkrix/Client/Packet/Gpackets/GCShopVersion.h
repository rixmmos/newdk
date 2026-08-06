//--------------------------------------------------------------------------------
// 
// Filename    : GCShopVersion.h 


// 
//--------------------------------------------------------------------------------

#ifndef __GC_SHOP_VERSION_H__
#define __GC_SHOP_VERSION_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//--------------------------------------------------------------------------------
//
// class GCShopVersion;
//
//--------------------------------------------------------------------------------

class GCShopVersion : public Packet 
{

public :

	GCShopVersion() throw ();
	virtual ~GCShopVersion() throw ();
	
	
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
	
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_SHOP_VERSION; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return szObjectID + szShopVersion*3+ szMarketCond; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCShopVersion"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif


public :

	// get/set NPC's object id
	ObjectID_t getObjectID () const throw () { return m_ObjectID; }
	void setObjectID ( ObjectID_t creatureID ) throw () { m_ObjectID = creatureID; }

	// get/set shop version
	ShopVersion_t getVersion(ShopRackType_t type) const throw()
	{
		if (type >= SHOP_RACK_TYPE_MAX) throw ("GCShopVersion::getVersion() : Out of Bound!");
		return m_Version[type];
	}
	
	void setVersion(ShopRackType_t type, ShopVersion_t ver) throw()
	{
		if (type >= SHOP_RACK_TYPE_MAX) throw ("GCShopVersion::setVersion() : Out of Bound!");
		m_Version[type] = ver;
	}

	// get/set market condition sell
	MarketCond_t getMarketCondSell(void) const throw() { return m_MarketCondSell;}
	void setMarketCondSell(MarketCond_t cond) throw() { m_MarketCondSell = cond;}
	
private :
	
	// NPC's object id
	ObjectID_t m_ObjectID;

	// shop version
	ShopVersion_t m_Version[SHOP_RACK_TYPE_MAX];
	
	MarketCond_t m_MarketCondSell;

};


//////////////////////////////////////////////////////////////////////
//
// class GCShopVersionFactory;
//
// Factory for GCShopVersion
//
//////////////////////////////////////////////////////////////////////

class GCShopVersionFactory : public PacketFactory 
{

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCShopVersion(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCShopVersion"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_SHOP_VERSION; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szObjectID + szShopVersion*3+ szMarketCond; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCShopVersionHandler;
//
//////////////////////////////////////////////////////////////////////

class GCShopVersionHandler 
{
	
public :
	
	// execute packet's handler
	static void execute ( GCShopVersion * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
