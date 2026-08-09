//////////////////////////////////////////////////////////////////////////////
// Filename    : GCShopBuyFail.cpp 
// Written By  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCShopBuyFail.h"
#include "Packet/Assert.h"

void GCShopBuyFail::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	iStream.read(m_ObjectID);
	iStream.read(m_Code);
	iStream.read(m_Amount);

	__END_CATCH
}
		    
void GCShopBuyFail::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	oStream.write(m_ObjectID);
	oStream.write(m_Code);
	oStream.write(m_Amount);

	__END_CATCH
}

void GCShopBuyFail::execute (Player * pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
		
	GCShopBuyFailHandler::execute(this , pPlayer);

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
	std::string GCShopBuyFail::toString () const
		   throw ()
	{
		__BEGIN_TRY
			
		StringStream msg;
		msg << "GCShopBuyFail(" 
			<< "ObjectID:" << m_ObjectID 
			<< ",CODE:" << GCShopBuyFailCode2String[m_Code]
			<< ",Amount:" << m_Amount
			<< ")";
		return msg.toString();
			
		__END_CATCH
	}
#endif