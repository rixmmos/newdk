////////////////////////////////////////////////////////////////////////////////
// Filename    : CGBuyStoreItem.cpp 

// Description : 



////////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGBuyStoreItem.h"

void CGBuyStoreItem::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		 
	iStream.read(m_OwnerObjectID);
	iStream.read(m_ItemObjectID);
	iStream.read(m_Index);

	__END_CATCH
}

void CGBuyStoreItem::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY

	oStream.write(m_OwnerObjectID);
	oStream.write(m_ItemObjectID);
	oStream.write(m_Index);

	__END_CATCH
}

void CGBuyStoreItem::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	CGBuyStoreItemHandler::execute (this , pPlayer);
		
	__END_CATCH
}

string CGBuyStoreItem::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "CGBuyStoreItem(" 
			<< ")";
	return msg.toString();

	__END_CATCH
}
