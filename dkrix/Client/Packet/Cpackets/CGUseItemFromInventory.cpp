//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseItemFromInventory.cpp 
// Written By  : excel96
// Description : 


//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGUseItemFromInventory.h"
#include "../SocketEncryptInputStream.h"
#include "../SocketEncryptOutputStream.h"
#include "Assert.h"

CGUseItemFromInventory::CGUseItemFromInventory () 
     throw ()
{
	__BEGIN_TRY

	m_InventoryItemObjectID = 0;
	
	__END_CATCH
}
	
CGUseItemFromInventory::~CGUseItemFromInventory () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

void CGUseItemFromInventory::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
#ifdef __USE_ENCRYPTER__
	SocketEncryptInputStream* pEIStream = dynamic_cast<SocketEncryptInputStream*>(&iStream);
    Assert(pEIStream!=NULL);

	if (pEIStream->getEncryptCode()!=0)
	{
		SHUFFLE_STATEMENT_3(pEIStream->getEncryptCode(),						//modify by viva for 3 parameter
							pEIStream->readEncrypt(m_ObjectID),
	//						pEIStream->readEncrypt(m_InventoryItemObjectID),	//modify by viva for notice
							pEIStream->readEncrypt(m_InvenX),
							pEIStream->readEncrypt(m_InvenY));
	}
	else
#endif
	{
		iStream.read(m_ObjectID);
	//	iStream.read(m_InventoryItemObjectID);	//modify by viva for notice
		iStream.read(m_InvenX);
		iStream.read(m_InvenY);
	}

	__END_CATCH
}
		    
void CGUseItemFromInventory::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream* pEOStream = dynamic_cast<SocketEncryptOutputStream*>(&oStream);
    Assert(pEOStream!=NULL);

	if (pEOStream->getEncryptCode()!=0)
	{
		SHUFFLE_STATEMENT_3(pEOStream->getEncryptCode(),						//modify by viva for 3 parameter
							pEOStream->writeEncrypt(m_ObjectID),
		//					pEOStream->writeEncrypt(m_InventoryItemObjectID),	//modify by viva for notice
							pEOStream->writeEncrypt(m_InvenX),
							pEOStream->writeEncrypt(m_InvenY));
	}
	else
#endif
	{
		oStream.write(m_ObjectID);
	//	oStream.write(m_InventoryItemObjectID);
		oStream.write(m_InvenX);
		oStream.write(m_InvenY);
	}

	__END_CATCH
}

void CGUseItemFromInventory::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

#ifndef __GAME_CLIENT__
	CGUseItemFromInventoryHandler::execute (this , pPlayer);
#endif
	
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
std::string CGUseItemFromInventory::toString () 
	const throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGUseItemFromInventory("
		<< "ObjectID:" << (int)m_ObjectID 
		<< ",InvenX:" << (int)m_InvenX
		<< ",InvenY:" << (int)m_InvenY
		<< ")";
	return msg.toString();

	__END_CATCH
}
#endif