//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddHelicopter.cpp
// Written By  : excel96
// Description :

//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCAddHelicopter.h"

//////////////////////////////////////////////////////////////////////////////
// class GCAddHelicopter member methods
//////////////////////////////////////////////////////////////////////////////

void GCAddHelicopter::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	iStream.read(m_ObjectID);
	iStream.read(m_Code);

	__END_CATCH
}
		    
void GCAddHelicopter::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write(m_ObjectID);
	oStream.write(m_Code);
		
	__END_CATCH
}

void GCAddHelicopter::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCAddHelicopterHandler::execute( this , pPlayer );
		
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
	std::string GCAddHelicopter::toString () 
		const throw ()
	{
		__BEGIN_TRY

		StringStream msg;
		msg << "GCAddHelicopter( "
			<< ",ObjectID:" << (int)m_ObjectID
			<< ",Code:" << (int)m_Code
			<< ")";
		return msg.toString();

		__END_CATCH
	}

#endif