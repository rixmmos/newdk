//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCMoveOK.cc 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCMoveOK.h"
#include "../SocketEncryptInputStream.h"
#include "../SocketEncryptOutputStream.h"
#include "Assert.h"

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCMoveOK::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	/*
	iStream.read( m_X );
	iStream.read( m_Y );
	iStream.read( m_Dir );
	*/

#ifdef __USE_ENCRYPTER__
	SocketEncryptInputStream* pEIStream = dynamic_cast<SocketEncryptInputStream*>(&iStream);
    Assert(pEIStream!=NULL);

	if (pEIStream->getEncryptCode()!=0)
	{
		SHUFFLE_STATEMENT_3(pEIStream->getEncryptCode(),
							pEIStream->readEncrypt( m_X ),
							pEIStream->readEncrypt( m_Y ),
							pEIStream->readEncrypt( m_Dir ));
	}
	else
#endif
	{
		iStream.read( m_X );
		iStream.read( m_Y );
		iStream.read( m_Dir );
	}


	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCMoveOK::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	/*
	oStream.write( m_X );
	oStream.write( m_Y );
	oStream.write( m_Dir );
	*/

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream* pEOStream = dynamic_cast<SocketEncryptOutputStream*>(&oStream);
    Assert(pEOStream!=NULL);

	if (pEOStream->getEncryptCode()!=0)
	{
		SHUFFLE_STATEMENT_3(pEOStream->getEncryptCode(),
							pEOStream->writeEncrypt( m_X ),
							pEOStream->writeEncrypt( m_Y ),
							pEOStream->writeEncrypt( m_Dir ));
	}
	else
#endif
	{
		oStream.write( m_X );
		oStream.write( m_Y );
		oStream.write( m_Dir );
	}


	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCMoveOK::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCMoveOKHandler::execute( this , pPlayer );
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string GCMoveOK::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCMoveOK("
		<< "X:" << (int)m_X 
		<< ",Y:" << (int) m_Y 
		<< ",Dir:" << Dir2String[m_Dir] 
		<< ")";
	return msg.toString();

	__END_CATCH
}
#endif

