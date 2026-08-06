//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCMPRecoveryEnd.cpp 
// Written By  : Reiot
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCMPRecoveryEnd.h"

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
GCMPRecoveryEnd::GCMPRecoveryEnd()
{
	__BEGIN_TRY
	m_CurrentMP = 0;
	__END_CATCH
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
GCMPRecoveryEnd::~GCMPRecoveryEnd()
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCMPRecoveryEnd::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	iStream.read( m_CurrentMP );

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCMPRecoveryEnd::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	oStream.write( m_CurrentMP );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCMPRecoveryEnd::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCMPRecoveryEndHandler::execute( this , pPlayer );
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string GCMPRecoveryEnd::toString () const
		   throw ()
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "GCMPRecoveryEnd("
			<< ",CurrentMP:" << (int)m_CurrentMP
			<< ")" ;

		return msg.toString();

		__END_CATCH
	}
#endif