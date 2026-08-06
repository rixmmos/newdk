//----------------------------------------------------------------------
// 
// Filename    : CLReconnectLogin.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "CLReconnectLogin.h"

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void CLReconnectLogin::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	//--------------------------------------------------
	// read authentication key
	//--------------------------------------------------
	iStream.read( m_Key );
	iStream.read( m_LoginMode );

	__END_CATCH
}

		    
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void CLReconnectLogin::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	//--------------------------------------------------
	// write authentication key
	//--------------------------------------------------
	oStream.write( m_Key );
	oStream.write( m_LoginMode );

	__END_CATCH
}


//----------------------------------------------------------------------
// execute packet's handler
//----------------------------------------------------------------------
void CLReconnectLogin::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	#ifndef __GAME_CLIENT__
		CLReconnectLoginHandler::execute( this , pPlayer );
	#endif

	__END_CATCH
}


//----------------------------------------------------------------------
// get packet's debug std::string
//----------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
	std::string CLReconnectLogin::toString () const
		   throw ()
	{
		StringStream msg;
		
		msg << "CLReconnectLogin("
			<< "KEY:" << m_Key 
			<< ")" ;
		
		return msg.toString();
	}
#endif
