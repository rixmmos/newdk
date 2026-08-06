//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCGQuestStatusInfo.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCGQuestStatusInfo.h"
#include "Client.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCGQuestStatusInfo::GCGQuestStatusInfo () 
     throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GCGQuestStatusInfo::~GCGQuestStatusInfo () 
    throw ()
{
	__BEGIN_TRY

	std::list<QuestStatusInfo*>::iterator itr = m_Infos.begin();

	for ( ; itr != m_Infos.end() ; ++itr )
	{
		SAFE_DELETE( (*itr) );
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCGQuestStatusInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	m_Infos.clear();
	BYTE size;
	iStream.read(size);

	for ( int i=0; i<size; i++ )
	{
		QuestStatusInfo* pInfo = new QuestStatusInfo;
		pInfo->read(iStream);
		m_Infos.push_back(pInfo);
	}

	
	__END_CATCH
}

		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCGQuestStatusInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	BYTE size = m_Infos.size();
	oStream.write(size);

	std::list<QuestStatusInfo*>::const_iterator itr = m_Infos.begin();

	for ( ; itr != m_Infos.end(); ++itr )
	{
		(*itr)->write(oStream);
	}
		
	__END_CATCH
}

PacketSize_t	addSize( PacketSize_t tot, const QuestStatusInfo* pInfo )
{
	return tot + pInfo->getSize();
}

PacketSize_t GCGQuestStatusInfo::getPacketSize() 
const throw() 
{ 
	return std::accumulate( m_Infos.begin(), m_Infos.end(), szBYTE, addSize ); 
}
//////////////////////////////////////////////////////////////////////
//
// execute packet's handler
//
//////////////////////////////////////////////////////////////////////
void GCGQuestStatusInfo::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCGQuestStatusInfoHandler::execute( this , pPlayer );
		
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
string GCGQuestStatusInfo::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCGQuestStatusInfo("
		<< ")";
	return msg.toString();

	__END_CATCH
}
#endif

