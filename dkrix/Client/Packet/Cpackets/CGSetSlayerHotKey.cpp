//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGSetSlayerHotKey.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "CGSetSlayerHotKey.h"

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void CGSetSlayerHotKey::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	for( int i = 0; i < 4; i++ ) {
		iStream.read( m_HotKey[i] );
	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void CGSetSlayerHotKey::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	for ( int i = 0; i < 4; i++ ) {
		oStream.write( m_HotKey[i] );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void CGSetSlayerHotKey::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	#ifndef __GAME_CLIENT__
		CGSetSlayerHotKeyHandler::execute ( this , pPlayer );
	#endif
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string CGSetSlayerHotKey::toString () const
    throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGSetSlayerHotKey("
		<< "F9:" << (int)m_HotKey[0]
		<< ",F10:" << (int)m_HotKey[1]
		<< ",F11:" << (int)m_HotKey[2]
		<< ",F12:" << (int)m_HotKey[3]
		<< ")";

	return msg.toString();

	__END_CATCH
}
#endif