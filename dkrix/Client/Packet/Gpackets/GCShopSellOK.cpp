//--------------------------------------------------------------------------------
// 
// Filename    : GCShopSellOK.cpp 






// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCShopSellOK.h"
#include "Assert.h"


//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------

GCShopSellOK::GCShopSellOK() 
	throw ()
{
	__BEGIN_TRY 
	
	__END_CATCH;
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
GCShopSellOK::~GCShopSellOK() 
	throw ()
{
	__BEGIN_TRY 
	
	
	__END_CATCH;
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
void GCShopSellOK::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	iStream.read(m_ObjectID);
	iStream.read(m_Version);
	iStream.read(m_ItemObjectID);
	iStream.read(m_Price);

	__END_CATCH
}

		    
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCShopSellOK::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write(m_ObjectID);
	oStream.write(m_Version);
	oStream.write(m_ItemObjectID);
	oStream.write(m_Price);
	
	__END_CATCH
}

//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCShopSellOK::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCShopSellOKHandler::execute( this , pPlayer );

	__END_CATCH
}

//--------------------------------------------------------------------------------
// get packet's debug std::string
//--------------------------------------------------------------------------------
#ifdef __DEBUG_OUTPUT__
	std::string GCShopSellOK::toString () const
		   throw ()
	{
		__BEGIN_TRY
			
		StringStream msg;
		
		msg << "GCShopSellOK(";
		msg << "ObjectID:"     << m_ObjectID << ","
			<< "Version:"      << m_Version << ","
				<< "ItemObejctID:" << m_ItemObjectID << ","
				<< "Price:"        << m_Price;
	//	msg << ")" << endl;

		return msg.toString();
			
		__END_CATCH
	}
#endif

