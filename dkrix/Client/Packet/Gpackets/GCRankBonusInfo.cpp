//-------------------------------------------------------------------------------- // 
// Filename    : GCRankBonusInfo.cpp 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCRankBonusInfo.h"
#include "Packet/Assert.h"

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
GCRankBonusInfo::GCRankBonusInfo ()
	throw ()
{
}

//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
GCRankBonusInfo::~GCRankBonusInfo ()
	throw ()
{
	m_RankBonusInfoList.clear();
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCRankBonusInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	BYTE ListNum;
	iStream.read( ListNum );

	for ( WORD i = 0; i < ListNum; i++ )
	{
		DWORD rankBonusType;
		iStream.read( rankBonusType );
		m_RankBonusInfoList.push_back( rankBonusType );
	}

	__END_CATCH
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCRankBonusInfo::write ( SocketOutputStream & oStream ) const 
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	//--------------------------------------------------
	// write pc type
	//--------------------------------------------------
	oStream.write( static_cast<DWORD>(m_RankBonusInfoList.size()) );

	for (std::list<DWORD>::const_iterator itr = m_RankBonusInfoList.begin(); itr!= m_RankBonusInfoList.end(); itr++) 
	{
		oStream.write( (*itr) );
	}

	__END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCRankBonusInfo::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GCRankBonusInfoHandler::execute( this , pPlayer );

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
//--------------------------------------------------------------------------------
// get packet's debug std::string
//--------------------------------------------------------------------------------
std::string GCRankBonusInfo::toString () const
       throw ()
{
	__BEGIN_TRY
		
	StringStream msg;
	
	msg << "GCRankBonusInfo(";
	for (std::list<DWORD>::const_iterator itr = m_RankBonusInfoList.begin(); itr!= m_RankBonusInfoList.end(); itr++) 
	{
		msg << (*itr) << ",";
	}
	msg << ")";

	return msg.toString();

	__END_CATCH
}
#endif
