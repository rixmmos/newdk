//----------------------------------------------------------------------
// 
// Filename    : LCWorldList.cpp 
// Written By  : Reiot
// Description :
// 
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCWorldList.h"
#include "../PCSlayerInfo.h"
#include "../PCVampireInfo.h"
#include "../PCOustersInfo.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------
LCWorldList::LCWorldList ()
	throw ()
{
	m_CurrentWorldID = 1;
}


//----------------------------------------------------------------------
// destructor
//----------------------------------------------------------------------
LCWorldList::~LCWorldList ()
	throw ()
{
	__BEGIN_TRY

	
	while ( !m_WorldInfoList.empty() ) 
	{
		WorldInfo * pWorldInfo = m_WorldInfoList.front();
		if (pWorldInfo!=NULL)
		{
			delete pWorldInfo;
			pWorldInfo = NULL;
		}
		m_WorldInfoList.pop_front();
	}

	__END_CATCH
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
void LCWorldList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	iStream.read( m_CurrentWorldID );

	BYTE ListNum;

    
	iStream.read( ListNum );
	for( int i = 0; i < ListNum; i++ ) {
		WorldInfo * pWorldInfo = new WorldInfo();
		pWorldInfo->read( iStream );
		m_WorldInfoList.push_back( pWorldInfo );
	}
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void LCWorldList::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oStream.write( m_CurrentWorldID );

	BYTE ListNum = m_WorldInfoList.size();
	
	oStream.write( ListNum );

	for ( std::list<WorldInfo*>:: const_iterator itr = m_WorldInfoList.begin(); itr!= m_WorldInfoList.end(); itr++) {
		(*itr)->write( oStream );
	}
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void LCWorldList::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	LCWorldListHandler::execute( this , pPlayer );
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
PacketSize_t LCWorldList::getPacketSize () const 
	throw ()
{
	__BEGIN_TRY

	
	PacketSize_t PacketSize = szWorldID + szBYTE;

	for ( std::list< WorldInfo* >::const_iterator itr = m_WorldInfoList.begin() ; itr != m_WorldInfoList.end() ; itr ++ ) {
		PacketSize += (*itr)->getSize();
	}

	return PacketSize;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string LCWorldList::toString () const
		   throw ()
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "LCWorldList( CurrentWorldID : " << (int)m_CurrentWorldID;
		for ( std::list< WorldInfo* >::const_iterator itr = m_WorldInfoList.begin() ; itr != m_WorldInfoList.end() ; itr ++ ) {
			msg << (*itr)->toString();
		}
		msg << ")";

		return msg.toString();

		__END_CATCH
	}
#endif
