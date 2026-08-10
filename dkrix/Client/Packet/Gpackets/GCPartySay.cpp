//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCPartySay.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCPartySay.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCPartySay::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	// Wire order is unchanged: nameLen, name, colour, messageLen, message.
	// The lengths were previously read into one reused variable with no cap,
	// and the handler strcpy's both into fixed 128-byte buffers -- a hostile or
	// MITM server could overflow them. Caps mirror the server's own write():
	// character names are varchar(10) and chat messages are capped at 128,
	// exactly as GCWhisper and GCGuildChat already do here.
	BYTE szName;
	iStream.read(szName);
	if ( szName > 10 )
		throw InvalidProtocolException("too large name length");
	iStream.read(m_Name,szName);

	iStream.read(m_Color);

	BYTE szMessage;
	iStream.read(szMessage);
	if ( szMessage > 128 )
		throw InvalidProtocolException("too large message length");
	iStream.read(m_Message,szMessage);
		
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCPartySay::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	BYTE szName = m_Name.size();
	oStream.write(szName);
	oStream.write(m_Name);
	szName = m_Message.size();
	oStream.write(m_Color);
	oStream.write(szName);
	oStream.write(m_Message);
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCPartySay::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCPartySayHandler::execute( this , pPlayer );
		
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string GCPartySay::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCPartySay("
		<< "Name : " << m_Name
		<< ", Message : " << m_Message
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif

