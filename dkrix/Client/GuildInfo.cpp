//////////////////////////////////////////////////////////////////////
// 
// Filename    : GuildInfo.cpp 
// Written By  :
// Description :
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GuildInfo.h"
#include "Packet/SocketInputStream.h"
#include "Packet/SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GuildInfo::GuildInfo () 
     throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GuildInfo::~GuildInfo () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GuildInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	BYTE szGuildName, szGuildMaster, szGuildExpireDate;

	
	iStream.read( m_GuildID );
	iStream.read( szGuildName );

	if ( szGuildName == 0 )
		throw InvalidProtocolException( "szGuildName == 0" );
	if ( szGuildName > 30 )
		throw InvalidProtocolException( "too long szGuildName size" );

	iStream.read( m_GuildName, szGuildName );
	iStream.read( szGuildMaster );

	if ( szGuildMaster == 0 )
		throw InvalidProtocolException( "szGuildMaster == 0" );
	if ( szGuildMaster > 20 )
		throw InvalidProtocolException( "too long szGuildMaster size" );

	iStream.read( m_GuildMaster, szGuildMaster );
	iStream.read( m_GuildMemberCount );
	iStream.read( szGuildExpireDate );

	if ( szGuildExpireDate > 11 )
		throw InvalidProtocolException( "too long szGuildExpireDate size" );

	if ( szGuildExpireDate != 0 )
		iStream.read( m_GuildExpireDate, szGuildExpireDate );
	else
		m_GuildExpireDate == "";

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GuildInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	BYTE szGuildName = m_GuildName.size();
	BYTE szGuildMaster = m_GuildMaster.size();
	BYTE szGuildExpireDate = m_GuildExpireDate.size();
	
	if ( szGuildName == 0 )
		throw InvalidProtocolException( "szGuildName == 0" );
	if ( szGuildName > 30 )
		throw InvalidProtocolException( "too long szGuildName size" );

	if ( szGuildMaster == 0 )
		throw InvalidProtocolException( "szGuildMaster == 0" );
	if ( szGuildMaster > 20 )
		throw InvalidProtocolException( "too long szGuildMaster size" );

	
	oStream.write( m_GuildID );
	oStream.write( szGuildName );
	oStream.write( m_GuildName );
	oStream.write( szGuildMaster );
	oStream.write( m_GuildMaster );
	oStream.write( m_GuildMemberCount );

	oStream.write( szGuildExpireDate );

	if ( szGuildExpireDate != 0 )
		oStream.write( m_GuildExpireDate );

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t GuildInfo::getSize()
	throw()
{
	__BEGIN_TRY

	BYTE szGuildName = m_GuildName.size();
	BYTE szGuildMaster = m_GuildMaster.size();

	PacketSize_t PacketSize = szGuildID + szBYTE + szGuildName + szBYTE + szGuildMaster + szBYTE;

	return PacketSize;

	__END_CATCH
}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
std::string GuildInfo::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "GuildInfo( "
		<< "GuildID:" << m_GuildID
		<< "GuildName:" << m_GuildName
		<< "GuildMaster:" << m_GuildMaster
		<< "GuildMemberCount:" << m_GuildMemberCount
		<< ")";

	return msg.toString();

	__END_CATCH
}

