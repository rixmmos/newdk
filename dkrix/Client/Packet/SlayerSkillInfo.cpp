//////////////////////////////////////////////////////////////////////
// 
// Filename    : SlayerSkillInfo.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "SlayerSkillInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "Assert.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
SlayerSkillInfo::SlayerSkillInfo () 
     throw ()
{
	__BEGIN_TRY
	m_bLearnNewSkill = false;
	m_DomainType = 0;
	m_ListNum = 0;
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
SlayerSkillInfo::~SlayerSkillInfo () 
    throw ()
{
	__BEGIN_TRY

	
	while ( !m_SubSlayerSkillInfoList.empty() ) {
		SubSlayerSkillInfo * pSubSlayerSkillInfo = m_SubSlayerSkillInfoList.front();
		delete pSubSlayerSkillInfo;
		m_SubSlayerSkillInfoList.pop_front();
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void SlayerSkillInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	iStream.read( m_bLearnNewSkill);
	iStream.read( m_DomainType );
	iStream.read( m_ListNum );

	for( int i = 0; i < m_ListNum; i++ ) {
		SubSlayerSkillInfo * pSubSlayerSkillInfo = new SubSlayerSkillInfo();
		pSubSlayerSkillInfo->read( iStream );
		m_SubSlayerSkillInfoList.push_back( pSubSlayerSkillInfo );

	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void SlayerSkillInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	oStream.write( m_bLearnNewSkill );
	oStream.write( m_DomainType );
	oStream.write( m_ListNum );

    for ( std::list<SubSlayerSkillInfo*>:: const_iterator itr = m_SubSlayerSkillInfoList.begin(); itr!= m_SubSlayerSkillInfoList.end(); itr++) {
		Assert( *itr != NULL );
		(*itr)->write( oStream );
	}

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t SlayerSkillInfo::getSize()
	throw()
{

	PacketSize_t PacketSize = szBYTE + szSkillDomainType + szBYTE;

	for ( std::list< SubSlayerSkillInfo* >::const_iterator itr = m_SubSlayerSkillInfoList.begin() ; itr != m_SubSlayerSkillInfoList.end() ; itr ++ ) {

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
	std::string SlayerSkillInfo::toString () 
		const throw ()
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "SlayerSkillInfo( ListNum:" << (int)m_ListNum 
			<< " ListSet( " ;

		for ( std::list<SubSlayerSkillInfo*>::const_iterator itr = m_SubSlayerSkillInfoList.begin(); itr!= m_SubSlayerSkillInfoList.end() ; itr++ ) {
			Assert( *itr != NULL );
			msg << (*itr)->toString() << ",";
		}

		msg << ")";

		return msg.toString();

		__END_CATCH
	}
#endif