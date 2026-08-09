//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseMessageItemFromInventory.cpp 
// Written By  : excel96
// Description : 


//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGUseMessageItemFromInventory.h"
#include "Packet/Assert.h"

#ifdef __DEBUG_OUTPUT__
void CGUseMessageItemFromInventory::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	CGUseItemFromInventory::read(iStream);

	BYTE szMessage;

	// message
	iStream.read(szMessage);

	if (szMessage == 0)
		throw InvalidProtocolException("szMessage == 0");

	if (szMessage > 128)
		throw InvalidProtocolException("too large message length");

	iStream.read(m_Message , szMessage);


	__END_CATCH
}
#endif
		    
void CGUseMessageItemFromInventory::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY

	CGUseItemFromInventory::write(oStream);
	
	// message
	BYTE szMessage = m_Message.size();

	if (szMessage == 0)
		throw InvalidProtocolException("szMessage == 0");

	if (szMessage > 128)
		throw InvalidProtocolException("too large message length");

	oStream.write(szMessage);

	oStream.write(m_Message);


	__END_CATCH
}

void CGUseMessageItemFromInventory::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
#ifndef __GAME_CLIENT__
	CGUseMessageItemFromInventoryHandler::execute (this , pPlayer);
#endif
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
std::string CGUseMessageItemFromInventory::toString () 
	const throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGUseMessageItemFromInventory("
		<< "ObjectID:" << (int)getObjectID()
		<< ",InvenX:" << (int)getX()
		<< ",InvenY:" << (int)getY()
		<< ",msg:" << m_Message.c_str() 
		<< ")";
	return msg.toString();

	__END_CATCH
}
#endif