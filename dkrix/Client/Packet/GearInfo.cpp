//////////////////////////////////////////////////////////////////////
// 
// Filename    : GearInfo.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GearInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "Assert.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GearInfo::GearInfo () 
     throw ()
{
	__BEGIN_TRY
	m_ListNum = 0;
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GearInfo::~GearInfo () 
    throw ()
{
	__BEGIN_TRY

	
	while ( !m_GearSlotInfoList.empty() ) {
		GearSlotInfo * pGearSlotInfo = m_GearSlotInfoList.front();
		delete pGearSlotInfo;
		m_GearSlotInfoList.pop_front();
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GearInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	iStream.read( m_ListNum );

	for( int i = 0; i < m_ListNum; i++ ) {
		GearSlotInfo * pGearSlotInfo = new GearSlotInfo();
		pGearSlotInfo->read( iStream );
		m_GearSlotInfoList.push_back( pGearSlotInfo );

	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GearInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	oStream.write( m_ListNum );

    for ( std::list<GearSlotInfo*>:: const_iterator itr = m_GearSlotInfoList.begin(); itr!= m_GearSlotInfoList.end(); itr++) {
		Assert( *itr != NULL );
		(*itr)->write( oStream );
	}

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t GearInfo::getSize()
	throw()
{

	PacketSize_t PacketSize = szBYTE;

	for ( std::list< GearSlotInfo* >::const_iterator itr = m_GearSlotInfoList.begin() ; itr != m_GearSlotInfoList.end() ; itr ++ ) {

		PacketSize += (*itr)->getSize();

	}

	return PacketSize;

}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string GearInfo::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "GearInfo( ListNum:" << (int)m_ListNum 
		<< " ListSet( " ;

	for ( std::list<GearSlotInfo*>::const_iterator itr = m_GearSlotInfoList.begin(); itr!= m_GearSlotInfoList.end() ; itr++ ) {
		Assert( *itr != NULL );
		msg << (*itr)->toString() << ",";
	}

	msg << ")";

	return msg.toString();

	__END_CATCH
}
#endif