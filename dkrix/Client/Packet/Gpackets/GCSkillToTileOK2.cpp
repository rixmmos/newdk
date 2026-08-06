//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCSkillToTileOK2.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCSkillToTileOK2.h"



//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCSkillToTileOK2::GCSkillToTileOK2 () 
     throw ()
{
	__BEGIN_TRY

	m_CListNum = 0;

	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GCSkillToTileOK2::~GCSkillToTileOK2 () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCSkillToTileOK2::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	iStream.read( m_ObjectID );
	iStream.read( m_SkillType);
	iStream.read( m_X );
	iStream.read( m_Y );
	iStream.read( m_Range );
	iStream.read( m_Duration );
	iStream.read( m_Grade );
    iStream.read( m_CListNum );

    ObjectID_t m_Value;
	int i;

	for( i = 0; i < m_CListNum; i++ ) {
		iStream.read( m_Value );
		m_CList.push_back( m_Value );
	}

	ModifyInfo::read( iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCSkillToTileOK2::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	oStream.write( m_ObjectID );
	oStream.write( m_SkillType );
	oStream.write( m_X );
	oStream.write( m_Y );
	oStream.write( m_Range );
	oStream.write( m_Duration );
	oStream.write( m_Grade );
    oStream.write( m_CListNum );

    for ( std::list<ObjectID_t>::const_iterator itr = m_CList.begin(); itr!= m_CList.end() ; itr++ ) {
		oStream.write( *itr );
	}

	ModifyInfo::write( oStream);
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// GCSkillToTileOK2::addListElement()
// 

//
//////////////////////////////////////////////////////////////////////
void GCSkillToTileOK2::addCListElement( ObjectID_t ObjectID )
    throw()
{
    __BEGIN_TRY

    
    m_CList.push_back( ObjectID );

    
    m_CListNum++;

    __END_CATCH

}

 


//////////////////////////////////////////////////////////////////////
//
// execute packet's handler
//
//////////////////////////////////////////////////////////////////////
void GCSkillToTileOK2::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCSkillToTileOK2Handler::execute( this , pPlayer );
		
	__END_CATCH
}


#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
std::string GCSkillToTileOK2::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCSkillToTileOK2("
		<< "ObjectID:" << (int)m_ObjectID 
		<< ",SkillType:" << (int)m_SkillType 
		<< ",X:" << (int)m_X
		<< ",Y:" << (int)m_Y
		<< ",Range:" << (int)m_Range
		<< ",Duration:" << (int)m_Duration 
		<< ",Grade:" << (int)m_Grade 
		<< ",CListNum:" << (int)m_CListNum 
		<< " CListSet(";

	for ( std::list<ObjectID_t>::const_iterator itr = m_CList.begin(); itr!= m_CList.end() ; itr++ ) {
		msg << (int)(*itr) << ",";
	}

	msg << ")";
	msg << ModifyInfo::toString();

	msg << ")";

	return msg.toString();

	__END_CATCH
}


#endif