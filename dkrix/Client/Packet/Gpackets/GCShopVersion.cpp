//--------------------------------------------------------------------------------
// 
// Filename    : GCShopVersion.cpp 


// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCShopVersion.h"
#include "Packet/Assert.h"


//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------

GCShopVersion::GCShopVersion() 
	throw ()
{
	__BEGIN_TRY 
	
	__END_CATCH;
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
GCShopVersion::~GCShopVersion() 
	throw ()
{
	__BEGIN_TRY 
	
	__END_CATCH;
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
void GCShopVersion::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	// read object id
	iStream.read( m_ObjectID );

	// read versions
	for (ShopRackType_t i=0; i<SHOP_RACK_TYPE_MAX; i++)
		iStream.read(m_Version[i]);

	iStream.read(m_MarketCondSell);
	__END_CATCH
}

		    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCShopVersion::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	// write object id
	oStream.write( m_ObjectID );

	// write versions
	for (ShopRackType_t i=0; i<SHOP_RACK_TYPE_MAX; i++)
		oStream.write(m_Version[i]);
	
	oStream.write( m_MarketCondSell );
	__END_CATCH
}

//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCShopVersion::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCShopVersionHandler::execute( this , pPlayer );

	__END_CATCH
}

//--------------------------------------------------------------------------------
// get packet's debug std::string
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
	std::string GCShopVersion::toString () const
		   throw ()
	{
		__BEGIN_TRY
			
		StringStream msg;
		
		msg << "GCShopVersion(" << "ObjectID:" << (int)m_ObjectID;
		msg << "Ratio:" << (int)m_MarketCondSell;
		
		for (ShopRackType_t i=0; i<SHOP_RACK_TYPE_MAX; i++)
			msg << " Version[" << (int)i << "] : " << (int)m_Version[i];

		//msg << ")" << endl;
		
		return msg.toString();
			
		__END_CATCH
	}
#endif

