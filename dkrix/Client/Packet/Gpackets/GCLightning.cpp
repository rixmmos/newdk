//--------------------------------------------------------------------------------
// 
// Filename    : GCLightning.cpp 
// Written By  : Reiot
// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCLightning.h"


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCLightning::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read( m_Delay );

	__END_CATCH
}

		    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCLightning::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write( m_Delay );

	__END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCLightning::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCLightningHandler::execute( this , pPlayer );
		
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get packet's debug std::string
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
	std::string GCLightning::toString () const
		   throw ()
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "GCLightning("
			<< "Delay:" << (int)m_Delay
			<< ")";

		return msg.toString();

		__END_CATCH
	}

#endif
