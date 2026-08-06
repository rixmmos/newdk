//////////////////////////////////////////////////////////////////////////////
// Filename    : GCTeachSkillInfo.cpp 

// Description : 



//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCTeachSkillInfo.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCTeachSkillInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	iStream.read( m_DomainType );
	iStream.read( m_TargetLevel );

	__END_CATCH
}
		    
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCTeachSkillInfo::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	oStream.write( m_DomainType );
	oStream.write( m_TargetLevel );
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCTeachSkillInfo::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCTeachSkillInfoHandler::execute( this , pPlayer );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string GCTeachSkillInfo::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	std::string domain;

	switch (m_DomainType)
	{
		case SKILL_DOMAIN_BLADE:   domain = "BLADE";   break;
		case SKILL_DOMAIN_SWORD:   domain = "SWORD";   break;
		case SKILL_DOMAIN_GUN:     domain = "GUN";     break;
		//case SKILL_DOMAIN_RIFLE:   domain = "RIFLE";   break;
		case SKILL_DOMAIN_ENCHANT: domain = "ENCHANT"; break;
		case SKILL_DOMAIN_HEAL:    domain = "HEAL";    break;
		case SKILL_DOMAIN_ETC:     domain = "ETC";     break;
		case SKILL_DOMAIN_VAMPIRE: domain = "VAMPIRE"; break;
		default:                   domain = "UNKNOWN"; break;
	}

	msg << "GCTeachSkillInfo("
	    << "DomainType:"  << domain << ","
	    << "TargetLevel:" << (int)m_TargetLevel 
	    << ")";

	return msg.toString();
		
	__END_CATCH
}


#endif