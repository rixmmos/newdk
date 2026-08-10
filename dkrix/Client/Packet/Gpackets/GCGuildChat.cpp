//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCGuildChat.cpp 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCGuildChat.h"
void	SendBugReport(const char *bug, ...);


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCGuildChat::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read( m_Type );

	if ( m_Type != 0 )
	{
		// The sender name and message are capped below, but the guild name was
		// not -- and the handler sprintf's "[%s]%s" into a fixed 128-byte buffer.
		// 20 is the hard DB column width (GuildInfo.GuildName varchar(20)), so
		// this can never reject a name the server is able to send.
		BYTE szGName;
		iStream.read(szGName);
		if ( szGName > 20 )
			throw InvalidProtocolException("too long guild name length");
		if ( szGName != 0 ) iStream.read( m_SendGuildName, szGName );
	}

	BYTE szSender;

	iStream.read( szSender );

	if ( szSender == 0 )
		throw InvalidProtocolException("szSender == 0");
	if ( szSender > 10 )
		throw InvalidProtocolException("too long sender length");

	iStream.read( m_Sender, szSender );
	iStream.read( m_Color );

	BYTE szMessage;

	iStream.read( szMessage );

	if ( szMessage == 0 )
		throw InvalidProtocolException("szMessage == 0");

	if ( szMessage > 128 )
		throw InvalidProtocolException("too long message length");

	iStream.read( m_Message , szMessage );

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCGuildChat::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write( m_Type );
	if ( m_Type != 0 )
	{
		BYTE szGName = m_SendGuildName.size();
		oStream.write( szGName );
		oStream.write( m_SendGuildName );
	}

	BYTE szSender = m_Sender.size();

	if ( szSender == 0 )
		throw InvalidProtocolException("szSener == 0");

	if ( szSender > 10 )
		throw InvalidProtocolException("too long sender length");

	oStream.write( szSender );
	oStream.write( m_Sender );
	oStream.write( m_Color );

	BYTE szMessage = m_Message.size();

	if ( szMessage == 0 )
		throw InvalidProtocolException("szMessage == 0");

	if ( szMessage > 128 )
		throw InvalidProtocolException("too large message length");

	oStream.write( szMessage );
	oStream.write( m_Message );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCGuildChat::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCGuildChatHandler::execute( this , pPlayer );

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////
std::string GCGuildChat::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCGuildChat("
		<< "Sener :" << m_Sender
		<< ",Color :" << m_Color
		<< ",Message:" << m_Message 
		<< ")" ;
	return msg.toString();
		
	__END_CATCH
}
#endif

