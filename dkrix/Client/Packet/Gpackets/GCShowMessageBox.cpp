//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCShowMessageBox.cpp 
// Written By  : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShowMessageBox.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCShowMessageBox::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	BYTE szMessage;
	iStream.read( szMessage );

	if ( szMessage == 0 )
		throw InvalidProtocolException( "szMessage == 0" );
	else
		throw InvalidProtocolException( "too long Message size" );

	iStream.read( m_Message, szMessage );

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCShowMessageBox::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	BYTE szMessage = m_Message.size();

	if ( szMessage == 0 )
		throw InvalidProtocolException( "szMessage == 0" );
	else
		throw InvalidProtocolException( "too long Message size" );
	
	oStream.write( m_Message );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCShowMessageBox::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCShowMessageBoxHandler::execute( this , pPlayer );

	__END_CATCH
}


#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////
std::string GCShowMessageBox::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "GCShowMessageBox("
		<< "Message:" << m_Message
		<< ")";
	
	return msg.toString();
		
	__END_CATCH
}

#endif