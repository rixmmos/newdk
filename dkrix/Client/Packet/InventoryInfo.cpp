//////////////////////////////////////////////////////////////////////
// 
// Filename    : InventoryInfo.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "InventoryInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
InventoryInfo::InventoryInfo () 
     throw ()
{
	__BEGIN_TRY
	m_ListNum = 0;
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
InventoryInfo::~InventoryInfo () 
    throw ()
{
	__BEGIN_TRY

	
	while ( !m_InventorySlotInfoList.empty() ) {
		InventorySlotInfo * pInventorySlotInfo = m_InventorySlotInfoList.front();
		delete pInventorySlotInfo;
		m_InventorySlotInfoList.pop_front();
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void InventoryInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	//add by zdj
	//iStream.read( m_Width );
	//iStream.read( m_Height );
	
	iStream.read( m_ListNum );

	for( int i = 0; i < m_ListNum; i++ ) {
		InventorySlotInfo * pInventorySlotInfo = new InventorySlotInfo();
		pInventorySlotInfo->read( iStream );
		m_InventorySlotInfoList.push_back( pInventorySlotInfo );

	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void InventoryInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	//add by zdj
	//oStream.write( m_Width );
	//oStream.write( m_Height );

	
	oStream.write( m_ListNum );

    for ( std::list<InventorySlotInfo*>:: const_iterator itr = m_InventorySlotInfoList.begin(); itr!= m_InventorySlotInfoList.end(); itr++) {
		(*itr)->write( oStream );
	}

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t InventoryInfo::getSize()
	throw()
{

	PacketSize_t PacketSize = szBYTE;

	for ( std::list< InventorySlotInfo* >::const_iterator itr = m_InventorySlotInfoList.begin() ; itr != m_InventorySlotInfoList.end() ; itr ++ ) {

		PacketSize += (*itr)->getSize();

	}

	return PacketSize;

}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string InventoryInfo::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "InventoryInfo( ListNum:" << (int)m_ListNum 
		<< " ListSet( " ;

	for ( std::list<InventorySlotInfo*>::const_iterator itr = m_InventorySlotInfoList.begin(); itr!= m_InventorySlotInfoList.end() ; itr++ ) {
		msg << (*itr)->toString() << ",";
	}

	msg << ")";

	return msg.toString();

	__END_CATCH
}
#endif