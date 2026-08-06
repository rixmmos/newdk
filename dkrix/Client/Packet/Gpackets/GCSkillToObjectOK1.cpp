//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCSkillToObjectOK1.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCSkillToObjectOK1.h"



//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCSkillToObjectOK1::GCSkillToObjectOK1 () 
     throw ()
{
	__BEGIN_TRY


	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GCSkillToObjectOK1::~GCSkillToObjectOK1 () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCSkillToObjectOK1::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	iStream.read( m_SkillType );
	iStream.read( m_CEffectID );
	iStream.read( m_TargetObjectID );
	iStream.read( m_Duration );
	iStream.read( m_Grade );

	ModifyInfo::read( iStream);
	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCSkillToObjectOK1::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	oStream.write( m_SkillType );
	oStream.write( m_CEffectID );
	oStream.write( m_TargetObjectID );
	oStream.write( m_Duration );
	oStream.write( m_Grade );
	ModifyInfo::write( oStream);
	
	__END_CATCH
}
//////////////////////////////////////////////////////////////////////
//
// execute packet's handler
//
//////////////////////////////////////////////////////////////////////
void GCSkillToObjectOK1::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCSkillToObjectOK1Handler::execute( this , pPlayer );
		
	__END_CATCH
}


#ifdef __DEBUG_OUTPUT__
//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
std::string GCSkillToObjectOK1::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCSkillToObjectOK1("
		<< "SkillType:"         << (int)m_SkillType 
		<< ",CEffectID:"        << (int)m_CEffectID 
		<< ",TargetObjectID : " << (int)m_TargetObjectID 
		<< ",Duration: "        << (int)m_Duration
		<< ",Grade: "        << (int)m_Grade;
	msg << ModifyInfo::toString();
    msg << ")";
	return msg.toString();

	__END_CATCH
}



#endif