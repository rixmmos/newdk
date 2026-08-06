//----------------------------------------------------------------------
// 
// Filename    : GCAddOustersCorpse.cpp
// Written By  : Reiot
// 
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCAddOustersCorpse.h"


//----------------------------------------------------------------------

//----------------------------------------------------------------------
void GCAddOustersCorpse::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	m_OustersInfo.read( iStream );
	iStream.read( m_TreasureCount );

	__END_CATCH
}

		    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCAddOustersCorpse::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	m_OustersInfo.write( oStream );
	oStream.write( m_TreasureCount );

	__END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCAddOustersCorpse::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCAddOustersCorpseHandler::execute( this , pPlayer );
		
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
//--------------------------------------------------------------------------------
// get packet's debug string
//--------------------------------------------------------------------------------
std::string GCAddOustersCorpse::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "GCAddOustersCorpse("
		<< m_OustersInfo.toString()
		<< ", Count : " << (int)m_TreasureCount
		<< ")" ;

	return msg.toString();

	__END_CATCH
}
#endif