//--------------------------------------------------------------------------------
// 
// Filename    : GCShopSellFail.cpp 



// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCShopSellFail.h"
#include "Packet/Assert.h"


//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------

GCShopSellFail::GCShopSellFail() 
	throw ()
{
	__BEGIN_TRY 
	
	__END_CATCH;
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
GCShopSellFail::~GCShopSellFail() 
	throw ()
{
	__BEGIN_TRY 
	
	
	__END_CATCH;
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
void GCShopSellFail::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	//-----------------------------------------------------------------
	// read object id
	//-----------------------------------------------------------------
	iStream.read( m_ObjectID );

	__END_CATCH
}

		    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCShopSellFail::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	//-----------------------------------------------------------------
	// write object id
	//-----------------------------------------------------------------
	oStream.write( m_ObjectID );

	__END_CATCH
}

//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCShopSellFail::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCShopSellFailHandler::execute( this , pPlayer );

	__END_CATCH
}

//--------------------------------------------------------------------------------
// get packet's debug std::string
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
	std::string GCShopSellFail::toString () const
		   throw ()
	{
		__BEGIN_TRY
			
		StringStream msg;
		
		msg << "GCShopSellFail(" << "ObjectID:" << m_ObjectID;

		//msg << endl;

		return msg.toString();
			
		__END_CATCH
	}
#endif

