//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCPartyPosition.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCPartyPosition.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCPartyPosition::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	BYTE szName;
	iStream.read(szName);
	iStream.read(m_Name,szName);
	iStream.read(m_ZoneID);
	iStream.read(m_X);
	iStream.read(m_Y);
	iStream.read(m_MaxHP);
	iStream.read(m_HP);
		
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCPartyPosition::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	BYTE szName = m_Name.size();
	oStream.write(szName);
	oStream.write(m_Name);
	oStream.write(m_ZoneID);
	oStream.write(m_X);
	oStream.write(m_Y);
	oStream.write(m_MaxHP);
	oStream.write(m_HP);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCPartyPosition::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCPartyPositionHandler::execute( this , pPlayer );
		
	__END_CATCH
}


#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string GCPartyPosition::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCPartyPosition("
		<< "Name : " << m_Name
		<< ", X : " << m_X
		<< ", Y : " << m_Y
		<< ", HP : " << m_HP
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif

