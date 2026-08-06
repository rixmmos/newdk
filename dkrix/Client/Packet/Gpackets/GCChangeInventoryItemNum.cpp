//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCChangeInventoryItemNum.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCChangeInventoryItemNum.h"
#include "../SocketInputStream.h"
#include "../SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCChangeInventoryItemNum::GCChangeInventoryItemNum () 
     throw ()
{
	__BEGIN_TRY
	m_ChangedItemListNum = 0;
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GCChangeInventoryItemNum::~GCChangeInventoryItemNum () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCChangeInventoryItemNum::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	iStream.read( m_ChangedItemListNum );

	int i;
	ObjectID_t item;
	ItemNum_t num;
	
	for( i = 0; i < m_ChangedItemListNum; i++ ) {
		iStream.read( item);
		m_ChangedItemList.push_back( item);
	}
	for( i = 0; i < m_ChangedItemListNum; i++ ) {
		iStream.read( num);
		m_ChangedItemNumList.push_back( num);
	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCChangeInventoryItemNum::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	oStream.write( m_ChangedItemListNum );

    for ( std::list<ObjectID_t>:: const_iterator itr = m_ChangedItemList.begin(); itr!= m_ChangedItemList.end(); itr++) {
		oStream.write(*itr);
	}
    for ( std::list<ItemNum_t>:: const_iterator itr2 = m_ChangedItemNumList.begin(); itr2!= m_ChangedItemNumList.end(); itr2++) {
		oStream.write(*itr2);
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// GCChangeInventoryItemNum::addListElement()
// 

//
//////////////////////////////////////////////////////////////////////
void GCChangeInventoryItemNum::addChangedItemListElement( ObjectID_t id, ItemNum_t num)
	throw()
{
	__BEGIN_TRY

	
	m_ChangedItemList.push_back( id);
	m_ChangedItemNumList.push_back( num);

	
	m_ChangedItemListNum++;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string GCChangeInventoryItemNum::toString () 
		const throw ()
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "Changed ListNum: " << (int)m_ChangedItemListNum 
			<< " ChangedListSet( " ;

		int i;
		std::list<ObjectID_t>::const_iterator itrItem = m_ChangedItemList.begin();
		std::list<ItemNum_t>::const_iterator itrItemNum = m_ChangedItemNumList.begin();
		for( i = 0; i < m_ChangedItemListNum; i++)
		{
			msg << "(" << (int)(*itrItem) << ","<<(int)(*itrItemNum)<<"),";
			itrItem++;
			itrItemNum++;
		}

		msg << ")";

		return msg.toString();

		__END_CATCH
	}

#endif