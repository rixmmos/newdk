//////////////////////////////////////////////////////////////////////////////
// Filename    : GCShopBought.cpp 

// Description : 





//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCShopBought.h"
#include "Assert.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
GCShopBought::GCShopBought() 
	throw ()
{
	__BEGIN_TRY 
	
	m_ObjectID     = 0;
	m_Version      = 0;
	m_ShopType     = 0;
	m_ShopIndex    = 0;
	m_ItemObjectID = 0;
	m_ItemClass    = 0;
	m_Durability   = 0;
	m_Silver       = 0;
	m_Grade        = 0;
	m_EnchantLevel = 0;
	
	__END_CATCH;
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCShopBought::~GCShopBought() 
	throw ()
{
	__BEGIN_TRY 
	
	__END_CATCH;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCShopBought::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	iStream.read( m_ObjectID );
	iStream.read( m_Version );
	iStream.read( m_ShopType );
	iStream.read( m_ShopIndex );
	iStream.read( m_ItemObjectID );
	iStream.read( m_ItemClass );
	iStream.read( m_ItemType );

	BYTE optionSize;
	iStream.read( optionSize );
	for (int i=0; i<optionSize; i++)
	{
		OptionType_t optionType;
		iStream.read( optionType );
		addOptionType( optionType );
	}

	iStream.read( m_Durability );
	iStream.read( m_Silver );
	iStream.read( m_Grade );
	iStream.read( m_EnchantLevel );

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCShopBought::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write( m_ObjectID );
	oStream.write( m_Version );
	oStream.write( m_ShopType );
	oStream.write( m_ShopIndex );
	oStream.write( m_ItemObjectID );
	oStream.write( m_ItemClass );
	oStream.write( m_ItemType );
	
	BYTE optionSize = m_OptionType.size();
	std::list<OptionType_t>::const_iterator itr = m_OptionType.begin();
	for (; itr!=m_OptionType.end(); itr++)
	{
		OptionType_t optionType = *itr;
		oStream.write( optionType );
	}

	oStream.write( m_Durability );
	oStream.write( m_Silver );
	oStream.write( m_Grade );
	oStream.write( m_EnchantLevel );
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCShopBought::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCShopBoughtHandler::execute( this , pPlayer );

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__	
//////////////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////////////
std::string GCShopBought::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCShopBought("     
		<< "ObjectID:"       << (int)m_ObjectID << ","
		<< "ShopVersion: "   << (int)m_Version << ","
		<< "ShopRackType: "  << (int)m_ShopType << ","
		<< "ShopRackIndex: " << (int)m_ShopIndex << ","
		<< "ItemObjectID: "  << (int)m_ItemObjectID << ","
		<< "ItemClass: "     << (int)m_ItemClass << ","
		<< "ITemType: "      << (int)m_ItemType << ","
		<< "OptionTypeSize: "    << (int)m_OptionType.size() << ","
		<< "Durability: "    << (int)m_Durability << ","
		<< "Silver: "        << (int)m_Silver << ","
		<< "Grade: "        << (int)m_Grade << ","
		<< "EnchantLEvel: "  << (int)m_EnchantLevel; 
	msg << ")";
	return msg.toString();
		
	__END_CATCH
}
#endif

