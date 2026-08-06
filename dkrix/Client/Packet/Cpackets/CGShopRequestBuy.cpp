////////////////////////////////////////////////////////////////////////////////
// Filename    : CGShopRequestBuy.cpp 

// Description : 



////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "CGShopRequestBuy.h"
#include "../SocketInputStream.h"
#include "../SocketOutputStream.h"


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void CGShopRequestBuy::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	iStream.read( m_ObjectID );
	iStream.read( m_RackType );
	iStream.read( m_RackIndex );
	iStream.read( m_Num );
	iStream.read( m_X );
	iStream.read( m_Y );

	__END_CATCH
}

		    
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void CGShopRequestBuy::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write( m_ObjectID );
	oStream.write( m_RackType );
	oStream.write( m_RackIndex );
	oStream.write( m_Num );
	oStream.write( m_X );
	oStream.write( m_Y );

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// execute packet's handler
////////////////////////////////////////////////////////////////////////////////
void CGShopRequestBuy::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifndef __GAME_CLIENT__
	CGShopRequestBuyHandler::execute ( this , pPlayer );
#endif
		
	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug std::string
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string CGShopRequestBuy::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "CGShopRequestBuy(" 
	 		<< "ObjectID:" << (int)m_ObjectID  << ","
			<< "RackType:" << (int)m_RackType << ","
			<< "RackIndex:" << (int)m_RackIndex << ","
			<< "NumberOfItems:" << (int)m_Num << ","
			<< "InvenX" << (int)m_X << ","
			<< "InvenY" << (int)m_Y << ","
			<< ")";
	return msg.toString();

	__END_CATCH
}
#endif