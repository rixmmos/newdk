//////////////////////////////////////////////////////////////////////
// 
// Filename    : ExtraInfo.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "ExtraInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "Assert.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
ExtraInfo::ExtraInfo () 
     throw ()
{
	__BEGIN_TRY
	m_ListNum = 0;
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
ExtraInfo::~ExtraInfo () 
    throw ()
{
	__BEGIN_TRY

	
	while ( !m_ExtraSlotInfoList.empty() ) {
		ExtraSlotInfo * pExtraSlotInfo = m_ExtraSlotInfoList.front();
		delete pExtraSlotInfo;
		m_ExtraSlotInfoList.pop_front();
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void ExtraInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	iStream.read( m_ListNum );

	for( int i = 0; i < m_ListNum; i++ ) {
		ExtraSlotInfo * pExtraSlotInfo = new ExtraSlotInfo();
		pExtraSlotInfo->read( iStream );
		m_ExtraSlotInfoList.push_back( pExtraSlotInfo );

	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void ExtraInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	oStream.write( m_ListNum );

    for ( std::list<ExtraSlotInfo*>:: const_iterator itr = m_ExtraSlotInfoList.begin(); itr!= m_ExtraSlotInfoList.end(); itr++) {
		Assert( *itr != NULL );
		(*itr)->write( oStream );
	}

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t ExtraInfo::getSize()
	throw()
{

	PacketSize_t PacketSize = szBYTE;

	for ( std::list< ExtraSlotInfo* >::const_iterator itr = m_ExtraSlotInfoList.begin() ; itr != m_ExtraSlotInfoList.end() ; itr ++ ) {

		PacketSize += (*itr)->getSize();

	}

	return PacketSize;

}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string ExtraInfo::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "ExtraInfo( ListNum:" << (int)m_ListNum 
		<< " ListSet( " ;

	for ( std::list<ExtraSlotInfo*>::const_iterator itr = m_ExtraSlotInfoList.begin(); itr!= m_ExtraSlotInfoList.end() ; itr++ ) {
		Assert( *itr != NULL );
		msg << (*itr)->toString() << ",";
	}

	msg << ")";

	return msg.toString();

	__END_CATCH
}
#endif