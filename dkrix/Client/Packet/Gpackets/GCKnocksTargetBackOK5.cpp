//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCKnocksTargetBackOK5.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCKnocksTargetBackOK5.h"
#include "../SocketInputStream.h"
#include "../SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCKnocksTargetBackOK5::GCKnocksTargetBackOK5 ()
     throw ()
{
	__BEGIN_TRY

	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GCKnocksTargetBackOK5::~GCKnocksTargetBackOK5 ()
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCKnocksTargetBackOK5::read ( SocketInputStream & iStream )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	iStream.read( m_ObjectID );
	iStream.read( m_TargetObjectID );
//	iStream.read( m_X);
//	iStream.read( m_Y );
	iStream.read( m_bSuccess);

	iStream.read( m_SkillType);
	iStream.read( m_dir);
	iStream.read( m_X);
	iStream.read( m_Y);
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCKnocksTargetBackOK5::write ( SocketOutputStream & oStream )
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	oStream.write( m_ObjectID );
	oStream.write( m_TargetObjectID );
//	oStream.write( m_X);
//	oStream.write( m_Y);
	oStream.write( m_bSuccess);
	
	oStream.write( m_SkillType);
	oStream.write( m_dir);
	oStream.write( m_X);
	oStream.write( m_Y);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// execute packet's handler
//
//////////////////////////////////////////////////////////////////////
void GCKnocksTargetBackOK5::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCKnocksTargetBackOK5Handler::execute( this , pPlayer );
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string GCKnocksTargetBackOK5::toString () 
		const throw ()
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "GCKnocksTargetBackOK5(ObjectID: " << (int)m_ObjectID 
			<< ",TargetObjectID: " << (int)m_TargetObjectID << ",Success: "<< m_bSuccess
			<< ",(Dir,X,Y) : " << (int)m_dir << (int)m_X << (int)m_Y
	//		<< ",TargetXY: " << (int)m_X<<","<<(int)m_Y
			<< ")";

		return msg.toString();

		__END_CATCH
	}
#endif