//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCActiveGuildList.cpp 
// Written By  : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCActiveGuildList.h"


//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCActiveGuildList::GCActiveGuildList()
	throw()
{
}

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCActiveGuildList::~GCActiveGuildList()
	throw()
{
	__BEGIN_TRY
	
	
	clearGuildInfoList();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCActiveGuildList::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	WORD ListNum;

	iStream.read( ListNum );
	for ( int i = 0; i < ListNum; i++ )
	{
		GuildInfo* pGuildInfo = new GuildInfo();
		pGuildInfo->read( iStream );
		m_GuildInfoList.push_front( pGuildInfo );
	}

	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCActiveGuildList::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	WORD ListNum = m_GuildInfoList.size();
	oStream.write( ListNum );

	GuildInfoListConstItor itr = m_GuildInfoList.begin();
	for ( ; itr != m_GuildInfoList.end(); itr++ )
	{
		(*itr)->write( oStream );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCActiveGuildList::clearGuildInfoList()
	throw()
{
	__BEGIN_TRY

	
	while( !m_GuildInfoList.empty() )
	{
		GuildInfo* pGuildInfo = m_GuildInfoList.front();
		m_GuildInfoList.pop_front();
		delete( pGuildInfo );
//		SAFE_DELETE( pGuildInfo );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCActiveGuildList::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCActiveGuildListHandler::execute( this , pPlayer );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet size
//////////////////////////////////////////////////////////////////////
PacketSize_t GCActiveGuildList::getPacketSize() const
	throw()
{
	__BEGIN_TRY

	PacketSize_t PacketSize = szWORD;

	GuildInfoListConstItor itr = m_GuildInfoList.begin();

	for ( ; itr != m_GuildInfoList.end(); itr++ )
	{
		PacketSize += (*itr)->getSize();
	}

	return PacketSize;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////
std::string GCActiveGuildList::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "GCActiveGuildList(";

	std::list<GuildInfo*>::const_iterator itr = m_GuildInfoList.begin();
	for ( ; itr != m_GuildInfoList.end(); itr++ )
	{
		msg << (*itr)->toString();
	}

	msg << ")";
	
	return msg.toString();
		
	__END_CATCH
}

