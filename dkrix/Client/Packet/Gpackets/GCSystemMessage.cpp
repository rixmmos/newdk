//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCSystemMessage.cpp 
// Written By  : reiot@ewestsoft.com
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSystemMessage.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCSystemMessage::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	BYTE szMessage;

	iStream.read( szMessage );

	if ( szMessage == 0 )
		throw InvalidProtocolException("szMessage == 0");

    // No length cap here on purpose. szMessage is a BYTE, so `> 256` was
    // tautologically false and never fired; but the server's GCSystemMessage
    // ::write() applies no cap either, so a real cap would drop legitimate long
    // GM notices. The overflow it enabled is fixed in the handler instead, by
    // sizing the destination buffers and using bounded copies.

    iStream.read(m_Message, szMessage);

    iStream.read(m_Color);

    BYTE t;
    iStream.read(t);
    m_Type = (SystemMessageType)t;

    __END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCSystemMessage::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	BYTE szMessage = m_Message.size();

	oStream.write( szMessage );

	if ( szMessage == 0 )
		throw InvalidProtocolException("szMessage == 0");

	if ( szMessage > 256 )
		throw InvalidProtocolException("too large message length");

	oStream.write( m_Message );

	oStream.write( m_Color );

	BYTE t = (BYTE)m_Type;
	oStream.write( t );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCSystemMessage::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCSystemMessageHandler::execute( this , pPlayer );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////
std::string GCSystemMessage::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "GCSystemMessage("
		<< "Type:" << (int)m_Type 
		<< ",Color:" << m_Color 
		<< ",Message:" << m_Message 
		<< ")" ;
	
	return msg.toString();
		
	__END_CATCH
}


