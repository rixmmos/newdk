//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCUntransformOK.cc 
// Written By  : crazydog


// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCUntransformOK.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCUntransformOK::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read( m_X );
	iStream.read( m_Y );
	iStream.read( m_Dir );

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCUntransformOK::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write( m_X );
	oStream.write( m_Y );
	oStream.write( m_Dir );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCUntransformOK::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCUntransformOKHandler::execute( this , pPlayer );
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string GCUntransformOK::toString () const
		   throw ()
	{
		__BEGIN_TRY

		StringStream msg;
		msg << "GCUntransformOK("
			<< "X:" << (int)m_X 
			<< ",Y:" << (int) m_Y 
			<< ",Dir:" << Dir2String[m_Dir] 
			<< ")";
		return msg.toString();

		__END_CATCH
	}
#endif

