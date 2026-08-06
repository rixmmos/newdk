//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCSkillToTileOK3.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCSkillToTileOK3.h"



//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCSkillToTileOK3::GCSkillToTileOK3 () 
     throw ()
{
	__BEGIN_TRY

//	m_CListNum = 0;

	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GCSkillToTileOK3::~GCSkillToTileOK3 () 
    throw ()

{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCSkillToTileOK3::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	iStream.read( m_ObjectID );
	iStream.read( m_SkillType );
	iStream.read( m_X );
	iStream.read( m_Y );
	iStream.read( m_Grade );
/*	
	iStream.read( m_Duration );
	iStream.read( m_CListNum );

	ObjectID_t m_Value;

	for( int i = 0; i < m_CListNum; i++ ) {
		iStream.read( m_Value );
		m_CList.push_back(m_Value);
	}
*/
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCSkillToTileOK3::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	oStream.write( m_ObjectID );
	oStream.write( m_SkillType );
	oStream.write( m_X );
	oStream.write( m_Y );
	oStream.write( m_Grade );
/*	
	oStream.write( m_Duration );
	oStream.write( m_CListNum );

	for ( list<ObjectID_t>::const_iterator itr = m_CList.begin(); itr!= m_CList.end() ; itr++ ) {
		oStream.write( *itr );
	}
*/
	__END_CATCH
}
 
 

//////////////////////////////////////////////////////////////////////
//
// GCSkillToTileOK3::execute()
// 
//////////////////////////////////////////////////////////////////////
void GCSkillToTileOK3::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCSkillToTileOK3Handler::execute( this , pPlayer );
		
	__END_CATCH
}


#ifdef __DEBUG_OUTPUT__

//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
std::string GCSkillToTileOK3::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCSkillToTileOK3("
		<< "SkillType:" << (int)m_SkillType 
		<< ",ObjectID:" << (int)m_ObjectID
		<< ",Grade:" << (int)m_Grade;
	msg << ")";
	return msg.toString();

	__END_CATCH
}


#endif