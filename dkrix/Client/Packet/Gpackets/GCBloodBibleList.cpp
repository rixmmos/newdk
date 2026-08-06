//////////////////////////////////////////////////////////////////////////////
// Filename    : GCBloodBibleList.cpp 
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCBloodBibleList.h"
#include "Assert.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
GCBloodBibleList::~GCBloodBibleList() 
	throw ()
{
	__BEGIN_TRY 

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCBloodBibleList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	BYTE num;

	iStream.read( num );

	for ( int i=0; i<num; ++i )
	{
		ItemType_t iType;
		iStream.read( iType );

		m_BloodBibleList.push_back( iType );
	}

	__END_CATCH
}
		    
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCBloodBibleList::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	Assert( m_BloodBibleList.size() <= 12 );

	BYTE num = m_BloodBibleList.size();

	oStream.write(num);

	std::vector<ItemType_t>::const_iterator itr = m_BloodBibleList.begin();

	for ( int i=0; i<num; i++ )
	{
		oStream.write( *itr++ );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCBloodBibleList::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCBloodBibleListHandler::execute( this , pPlayer );

	__END_CATCH
}

PacketSize_t GCBloodBibleList::getPacketSize() const
	throw()
{
	__BEGIN_TRY

	PacketSize_t result = 0;

	result += szBYTE + szItemType * m_BloodBibleList.size();

	return result;

	__END_CATCH
}
#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////////////
string GCBloodBibleList::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "GCBloodBibleList(";
	msg << ")";

	return msg.toString();
		
	__END_CATCH
}

#endif