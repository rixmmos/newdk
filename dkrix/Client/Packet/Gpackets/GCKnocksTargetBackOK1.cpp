//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCKnocksTargetBackOK1.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCKnocksTargetBackOK1.h"
#include "../SocketInputStream.h"
#include "../SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCKnocksTargetBackOK1::GCKnocksTargetBackOK1 ()
     throw ()
{
	__BEGIN_TRY
	__BEGIN_DEBUG
	m_ObjectID = 0;
	m_BulletNum = 0;
	__END_DEBUG
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GCKnocksTargetBackOK1::~GCKnocksTargetBackOK1 ()
    throw ()
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	__END_DEBUG
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCKnocksTargetBackOK1::read ( SocketInputStream & iStream )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	
	
	iStream.read( m_SkillType );
	iStream.read( m_dir );
	iStream.read( m_X );
	iStream.read( m_Y );
	iStream.read( m_ObjectID );
	iStream.read( m_BulletNum );
	iStream.read( m_bSuccess);

	ModifyInfo::read( iStream);

	__END_DEBUG
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCKnocksTargetBackOK1::write ( SocketOutputStream & oStream )
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	
	oStream.write( m_SkillType);
	oStream.write( m_dir );
	oStream.write( m_X );
	oStream.write( m_Y );
	oStream.write( m_ObjectID );
	oStream.write( m_BulletNum );
	oStream.write( m_bSuccess);

	ModifyInfo::write( oStream);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// execute packet's handler
//
//////////////////////////////////////////////////////////////////////
void GCKnocksTargetBackOK1::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__BEGIN_DEBUG
		
	GCKnocksTargetBackOK1Handler::execute( this , pPlayer );
		
	__END_DEBUG
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string GCKnocksTargetBackOK1::toString () 
		const throw ()
	{
		__BEGIN_TRY
		__BEGIN_DEBUG

		StringStream msg;

		msg << "GCKnocksTargetBackOK1(ObjectID: " << (int)m_ObjectID
			<< ", BulletNum: " << (int)m_BulletNum
			<< ", (DIR,X,Y): " << (int)m_dir <<(int)m_X<<(int)m_Y
			<< ", Success: " << (int)m_bSuccess
			<< ModifyInfo::toString()
			<< ")";

		return msg.toString();

		__END_DEBUG
		__END_CATCH
	}
#endif

