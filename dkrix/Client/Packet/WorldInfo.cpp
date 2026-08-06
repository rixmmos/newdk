//////////////////////////////////////////////////////////////////////
// 
// Filename    : WorldInfo.cpp 
// Written By  : elca@ewestsoft.com


// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "WorldInfo.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
WorldInfo::WorldInfo () 
     throw ()
{
	__BEGIN_TRY
	m_Stat = 0;
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
WorldInfo::~WorldInfo () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void WorldInfo::read ( SocketInputStream & iStream ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	BYTE szName;
	
	iStream.read( m_ID );
	iStream.read( szName );
	iStream.read( m_Name, szName );
	iStream.read( m_Stat );

	__END_CATCH
}
		    
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void WorldInfo::write ( SocketOutputStream & oStream ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	BYTE szName = m_Name.size();
	
	oStream.write( m_ID );
	oStream.write( szName );
	oStream.write( m_Name );
	oStream.write( m_Stat );

	__END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t WorldInfo::getSize()
	throw()
{
	__BEGIN_TRY

	BYTE szName = m_Name.size();

	PacketSize_t PacketSize = szWorldID + szBYTE + szName + szBYTE;

	return PacketSize;

	__END_CATCH
}

 /////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
	std::string WorldInfo::toString () 
		const throw ()
	{
		__BEGIN_TRY

		StringStream msg;

		msg << "WorldInfo( "
			<< "ID : "<< (int)m_ID
			<< "Name : " << m_Name
			<< "Stat : " << (int)m_Stat
			<< ")";

		return msg.toString();

		__END_CATCH
	}
#endif
