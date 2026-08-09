//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCMoveError.cpp
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCMoveError.h"
#include "../SocketEncryptInputStream.h"
#include "../SocketEncryptOutputStream.h"
#include "Packet/Assert.h"


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCMoveError::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __USE_ENCRYPTER__
	SocketEncryptInputStream* pEIStream = dynamic_cast<SocketEncryptInputStream*>(&iStream);
    Assert(pEIStream!=NULL);

	if (pEIStream->getEncryptCode()!=0)
	{
		SHUFFLE_STATEMENT_2(pEIStream->getEncryptCode(),
							pEIStream->readEncrypt( m_X ),
							pEIStream->readEncrypt( m_Y ));
	}
	else
#endif
	{
		iStream.read( m_X );
		iStream.read( m_Y );
	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCMoveError::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream* pEOStream = dynamic_cast<SocketEncryptOutputStream*>(&oStream);
    Assert(pEOStream!=NULL);

	if (pEOStream->getEncryptCode()!=0)
	{
		SHUFFLE_STATEMENT_2(pEOStream->getEncryptCode(),
							pEOStream->writeEncrypt( m_X ),
							pEOStream->writeEncrypt( m_Y ));
	}
	else
#endif
	{
		oStream.write( m_X );
		oStream.write( m_Y );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCMoveError::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCMoveErrorHandler::execute( this , pPlayer );
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string GCMoveError::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCMoveError(X:" << (int)m_X << "Y:" << (int)m_Y << ")";
	return msg.toString();

	__END_CATCH
}
#endif