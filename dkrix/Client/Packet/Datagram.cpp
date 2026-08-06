//////////////////////////////////////////////////////////////////////
//
// Filename    : Datagram.cpp
// Written By  : reiot@ewestsoft.com
// Description : 
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "Datagram.h"
#include "Assert.h"
#include "PacketFactoryManager.h"
#include "DatagramPacket.h"
#include "Packet.h"

void	SendBugReport(const char *bug, ...);

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
Datagram::Datagram () 
	throw () 
: m_Length(0), m_InputOffset(0), m_OutputOffset(0), m_Data(NULL) 
{
	__BEGIN_TRY

	memset( &m_SockAddr , 0 , sizeof(m_SockAddr) );
	m_SockAddr.sin_family = AF_INET;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
Datagram::~Datagram () 
	throw () 
{ 
	__BEGIN_TRY

	if ( m_Data != NULL ) {
		delete [] m_Data; 
		m_Data = NULL;
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void Datagram::read ( char * buf , uint len )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	// boundary check
	//Assert( m_InputOffset + len <= m_Length );
	if (m_InputOffset + len > m_Length)
		throw InsufficientDataException("Datagram read");

	memcpy( buf , &m_Data[m_InputOffset] , len );

	m_InputOffset += len;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void Datagram::read ( std::string & str , uint len )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	// boundary check
	//Assert( m_InputOffset + len <= m_Length );
	if (m_InputOffset + len > m_Length )
		throw InsufficientDataException("Datagram read");

	str.reserve(len);
	str.assign( &m_Data[m_InputOffset] , len );

	m_InputOffset += len;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// 



// 


// 
// *CAUTION*
// 



// 
//////////////////////////////////////////////////////////////////////
void Datagram::read ( DatagramPacket * & pPacket )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	Assert( pPacket == NULL );

	PacketID_t packetID;
	PacketSize_t packetSize;

	// initialize packet header
	read( (char*)&packetID , szPacketID );
	read( (char*)&packetSize , szPacketSize );

	cout << "DatagramPacket I  D : " << packetID;

	
	if ( packetID >= Packet::PACKET_MAX )
	{
//		SendBugReport("DataGram Invalid packetID : %d/%d", packetID, Packet::PACKET_MAX );
		throw InvalidProtocolException("invalid packet id(datagram)");
	}

	#ifdef __DEBUG_OUTPUT__
		cout << " ( " << g_pPacketFactoryManager->getPacketName(packetID).c_str() << " ) " << endl;
	#endif

	cout << "DatagramPacket Size : " << packetSize << endl;

	
	if ( packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID) )
	{
		SendBugReport("too large PacketSize ID)%d %d/%d", packetID, packetSize, g_pPacketFactoryManager->getPacketMaxSize( packetID ) );
		throw InvalidProtocolException("too large packet size(DataGram)");
	}

	
	if ( m_Length < szPacketHeader + packetSize )
		throw Error("    .");

	
	if ( m_Length > szPacketHeader + packetSize )
		throw Error("     .");

	
	pPacket = (DatagramPacket*)g_pPacketFactoryManager->createPacket( packetID );

	Assert( pPacket != NULL );

	
	pPacket->read( *this );

	
	pPacket->setHost( getHost() );
	pPacket->setPort( getPort() );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void Datagram::write ( const char * buf , uint len )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	// boundary check
	Assert( m_OutputOffset + len <= m_Length );

	memcpy( &m_Data[m_OutputOffset] , buf , len );

	m_OutputOffset += len;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////

//
// *CAUTION*
//


//
//////////////////////////////////////////////////////////////////////
void Datagram::write ( const std::string & str )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	// boundary check
	Assert( m_OutputOffset + str.size() <= m_Length );

	// write std::string body
	write( str.c_str() , str.size() );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// write packet
//




//
//////////////////////////////////////////////////////////////////////
void Datagram::write ( const DatagramPacket * pPacket )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	Assert( pPacket != NULL );

	PacketID_t packetID = pPacket->getPacketID();
	PacketSize_t packetSize = pPacket->getPacketSize();

	
	setData( szPacketHeader + packetSize );

	
	write( (char*)&packetID , szPacketID );
	write( (char*)&packetSize , szPacketSize );

	
	pPacket->write( *this );

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// set data
//

//
//////////////////////////////////////////////////////////////////////
void Datagram::setData ( char * data , uint len )
	throw ( ProtocolException , Error ) 
{ 
	__BEGIN_TRY

	Assert( data != NULL && m_Data == NULL );

	m_Length = len; 
	m_Data = new char[m_Length]; 
	memcpy( m_Data , data , m_Length ); 

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void Datagram::setData ( uint len )
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	Assert( m_Data == NULL );

	m_Length = len;
	m_Data = new char[ m_Length ];

	__END_CATCH
}
	

//////////////////////////////////////////////////////////////////////
// set address
//////////////////////////////////////////////////////////////////////
void Datagram::setAddress ( SOCKADDR_IN * pSockAddr )
	throw ( ProtocolException , Error ) 
{ 
	__BEGIN_TRY

	Assert( pSockAddr != NULL );

	memcpy( &m_SockAddr , pSockAddr , szSOCKADDR_IN ); 

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// get debug std::string
//////////////////////////////////////////////////////////////////////
std::string Datagram::toString () const
	throw ()
{
	StringStream msg;
	msg << "Datagram("
		<< "Length:" << m_Length
		<< ",InputOffset:" << m_InputOffset
		<< ",OutputOffset:" << m_OutputOffset
		<< ")";
	return msg.toString();
}
