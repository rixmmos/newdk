//////////////////////////////////////////////////////////////////////
// 
// Filename    : VampireSkillInfo.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "VampireSkillInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "Assert.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
VampireSkillInfo::VampireSkillInfo () 
     throw ()
{
	__BEGIN_TRY
	m_bLearnNewSkill = false;
	m_ListNum = 0;
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
VampireSkillInfo::~VampireSkillInfo () 
    throw ()
{
	__BEGIN_TRY

	
	while ( !m_SubVampireSkillInfoList.empty() ) {
		SubVampireSkillInfo * pSubVampireSkillInfo = m_SubVampireSkillInfoList.front();
		delete pSubVampireSkillInfo;
		m_SubVampireSkillInfoList.pop_front();
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void VampireSkillInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	iStream.read( m_bLearnNewSkill);
	iStream.read( m_ListNum );

	for( int i = 0; i < m_ListNum; i++ ) {
		SubVampireSkillInfo * pSubVampireSkillInfo = new SubVampireSkillInfo();
		pSubVampireSkillInfo->read( iStream );
		m_SubVampireSkillInfoList.push_back( pSubVampireSkillInfo );

	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void VampireSkillInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	oStream.write( m_bLearnNewSkill );
	oStream.write( m_ListNum );

    for ( std::list<SubVampireSkillInfo*>:: const_iterator itr = m_SubVampireSkillInfoList.begin(); itr!= m_SubVampireSkillInfoList.end(); itr++) {
		Assert( *itr != NULL );
		(*itr)->write( oStream );
	}

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t VampireSkillInfo::getSize()
	throw()
{

	PacketSize_t PacketSize = szBYTE + szBYTE;

	for ( std::list< SubVampireSkillInfo* >::const_iterator itr = m_SubVampireSkillInfoList.begin() ; itr != m_SubVampireSkillInfoList.end() ; itr ++ ) {

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
	std::string VampireSkillInfo::toString () 
		const throw ()
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "VampireSkillInfo( ListNum:" << (int)m_ListNum 
			<< " ListSet( " ;

		for ( std::list<SubVampireSkillInfo*>::const_iterator itr = m_SubVampireSkillInfoList.begin(); itr!= m_SubVampireSkillInfoList.end() ; itr++ ) {
			Assert( *itr != NULL );
			msg << (*itr)->toString() << ",";
		}

		msg << ")";

		return msg.toString();

		__END_CATCH
	}
#endif