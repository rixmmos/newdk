//--------------------------------------------------------------------------------
// 
// Filename    : CGShopRequestList.cpp 




// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "CGShopRequestList.h"
#include "../SocketInputStream.h"
#include "../SocketOutputStream.h"


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CGShopRequestList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	iStream.read( m_ObjectID );
	iStream.read( m_RackType );

	__END_CATCH
}

		    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CGShopRequestList::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write( m_ObjectID );
	oStream.write( m_RackType );

	__END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void CGShopRequestList::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	#ifndef __GAME_CLIENT__
		CGShopRequestListHandler::execute ( this , pPlayer );
	#endif
		
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug std::string
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
	std::string CGShopRequestList::toString () 
		const throw ()
	{
		StringStream msg;
		
		msg << "CGShopRequestList(" 
			<< "ObjectID:" << (int)m_ObjectID 
			<< "RackType:" << (int)m_RackType
			<< ")";
			
		return msg.toString();
	}
#endif
