//--------------------------------------------------------------------------------
// 
// Filename    : GCChangeDarkLight.cpp 
// Written By  : Reiot
// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCChangeDarkLight.h"


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCChangeDarkLight::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read( m_DarkLevel );
	iStream.read( m_LightLevel );

	__END_CATCH
}

		    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCChangeDarkLight::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write( m_DarkLevel );
	oStream.write( m_LightLevel );

	__END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCChangeDarkLight::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCChangeDarkLightHandler::execute( this , pPlayer );
		
	__END_CATCH
}


//--------------------------------------------------------------------------------
// get packet's debug std::string
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
	std::string GCChangeDarkLight::toString () const
		   throw ()
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "GCChangeDarkLight("
			<< "DarkLevel:" << (int)m_DarkLevel
			<< ",LightLevel:" << (int)m_LightLevel 
			<< ")";

		return msg.toString();

		__END_CATCH
	}
#endif
