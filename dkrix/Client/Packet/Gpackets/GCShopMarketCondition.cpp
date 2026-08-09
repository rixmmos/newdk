//--------------------------------------------------------------------------------
// 
// Filename    : GCShopMarketCondition.cpp 


// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCShopMarketCondition.h"
#include "Packet/Assert.h"


//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------

GCShopMarketCondition::GCShopMarketCondition() 
	throw ()
{
	__BEGIN_TRY 
	
	__END_CATCH;
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
GCShopMarketCondition::~GCShopMarketCondition() 
	throw ()
{
	__BEGIN_TRY 
	
	__END_CATCH;
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
void GCShopMarketCondition::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	// read object id
	iStream.read( m_ObjectID );

	iStream.read( m_MarketCondBuy );

	iStream.read( m_MarketCondSell );

	__END_CATCH
}

		    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCShopMarketCondition::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	// write object id
	oStream.write( m_ObjectID );

	oStream.write( m_MarketCondBuy );

	oStream.write( m_MarketCondSell );
	
	__END_CATCH
}

//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCShopMarketCondition::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCShopMarketConditionHandler::execute( this , pPlayer );

	__END_CATCH
}

//--------------------------------------------------------------------------------
// get packet's debug std::string
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
	std::string GCShopMarketCondition::toString () const
		   throw ()
	{
		__BEGIN_TRY
			
		StringStream msg;
		
		msg << "GCShopMarketCondition(" 
			  << "ObjectID: "       << (int)m_ObjectID << ", "
				<< "MarketCondBuy: "  << (int)m_MarketCondBuy << ", "
				<< "MarketCondsell: " << (int)m_MarketCondSell;
		//msg << ")" << endl;
		
		return msg.toString();
			
		__END_CATCH
	}
#endif

