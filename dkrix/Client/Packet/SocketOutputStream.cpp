//////////////////////////////////////////////////////////////////////
// 
// SocketOutputStream.cpp
// 
// by Reiot
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////
#include "Client_PCH.h"
#include "SocketOutputStream.h"
#include "Assert.h"
#include "Packet.h"
#include "DebugInfo.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
SocketOutputStream::SocketOutputStream ( Socket * sock , uint BufferLen )
	throw ( ProtocolException , Error )
: m_Socket(sock), m_Buffer(NULL), m_BufferLen(BufferLen), m_Head(0), m_Tail(0) ,m_Sequence(0)
{
	__BEGIN_TRY

	Assert( m_Socket != NULL );
	Assert( m_BufferLen > 0 );
	
	m_Buffer = new char[ m_BufferLen ];
	//add by viva
	m_EncryptKey = 0;
	m_HashTable = NULL;
	//end
	__END_CATCH
}
	

//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
SocketOutputStream::~SocketOutputStream ()
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	if ( m_Buffer != NULL ) {
		
		
		// flush();
		delete [] m_Buffer;
		m_Buffer = NULL;
	}
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// write data to stream (output buffer)
//
// *Notes*
//
// ( ( m_Head = m_Tail + 1 ) ||  
//   ( ( m_Head == 0 ) && ( m_Tail == m_BufferLen - 1 ) )
//


//
//////////////////////////////////////////////////////////////////////
uint SocketOutputStream::write ( const char * buf , uint len )
     throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	uint nFree = ( ( m_Head <= m_Tail ) ?  m_BufferLen - m_Tail + m_Head - 1 : m_Head - m_Tail - 1 );
	//m_Tail - m_Head - 1 );

	
	if ( len >= nFree )
		resize( len - nFree + 1 );
		
	if ( m_Head <= m_Tail ) {		// normal order

		//
		//    H   T
		// 0123456789
		// ...abcd...
		//
		
		if ( m_Head == 0 ) {
			
			nFree = m_BufferLen - m_Tail - 1;
			memcpy( &m_Buffer[m_Tail] , buf , len );

		} else {

			nFree = m_BufferLen - m_Tail;
			if ( len <= nFree )
				memcpy( &m_Buffer[m_Tail] , buf , len );
			else {
				memcpy( &m_Buffer[m_Tail] , buf , nFree );
				memcpy( m_Buffer , &buf[nFree] , len - nFree );
			}

		}

	} else {						// reversed order

		//
		//     T  H
		// 0123456789
		// abcd...efg
		//
		
		memcpy( &m_Buffer[m_Tail] , buf , len );

	}
	
	// advance m_Tail
	m_Tail = ( m_Tail + len ) % m_BufferLen;
		
	return len;
	
	__END_CATCH
}
	

//////////////////////////////////////////////////////////////////////
// write packet to stream (output buffer)
//////////////////////////////////////////////////////////////////////
void SocketOutputStream::write ( const Packet * pPacket )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	PacketID_t packetID = pPacket->getPacketID();
	write( (char*)&packetID , szPacketID );
	
	PacketSize_t packetSize = pPacket->getPacketSize();
	write( (char*)&packetSize , szPacketSize );
	
	
	write( (char*)&m_Sequence, szSequenceSize);
	m_Sequence++;

    printf("%s:%d SocketOutputStream::write packetID: %d, packetSZ: %d sequence %d\n",
        __FILE__, __LINE__,
        packetID, packetSize, m_Sequence-1);

	
		pPacket->write( *this );
	
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// flush stream (output buffer) to socket
//////////////////////////////////////////////////////////////////////
uint SocketOutputStream::flush () 
     throw ( IOException , Error )
{
	__BEGIN_TRY

	uint nFlushed = 0;
	uint nSent;
	uint nLeft;
	
	try {

		if ( m_Head < m_Tail ) {
		
			//
			//    H   T
			// 0123456789
			// ...abcd...
			//
	
			nLeft = m_Tail - m_Head;
	//add by viva 2008-12-31
			if(nLeft > 0)
				m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Head], nLeft);
			//end
			while ( nLeft > 0 ) {
				nSent = m_Socket->send( &m_Buffer[m_Head] , nLeft , 0 );
				nFlushed += nSent;
				nLeft -= nSent;
				m_Head += nSent;
			}
	
		} else if ( m_Head > m_Tail ) {
	
			//
			//     T  H
			// 0123456789
			// abcd...efg
			//
			
			nLeft = m_BufferLen - m_Head;
	//add by viva 2008-12-31
			if(nLeft > 0)
				m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Head], nLeft);
			//end
			while ( nLeft > 0 ) {
				nSent = m_Socket->send( &m_Buffer[m_Head] , nLeft , 0 );
				nFlushed += nSent;
				nLeft -= nSent;
				m_Head += nSent;
			}
			
			Assert( m_Head == m_BufferLen );
			
			m_Head = 0;
			
			nLeft = m_Tail;
	//add by viva 2008-12-31
			if(nLeft > 0)
				m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Head], nLeft);
			//end
			while ( nLeft > 0 ) {
				nSent = m_Socket->send( &m_Buffer[m_Head] , nLeft , 0 );
				nFlushed += nSent;
				nLeft -= nSent;
				m_Head += nSent;
			}
	
		}
	
		Assert( m_Head == m_Tail );
//		if ( m_Head != m_Tail ) 
//			g_pLogManager->log1( "m_Head = %d , m_Tail = %d\n", m_Head, m_Tail );
		//m_Head = m_Tail = 0;
		
	} catch ( NonBlockingIOException ) {
	}
	
	m_Head = m_Tail = 0;

	return nFlushed;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// resize buffer
//////////////////////////////////////////////////////////////////////
void SocketOutputStream::resize ( int size )
	 throw ( IOException , Error )
{
	__BEGIN_TRY
		
	Assert( size != 0 );

	uint newBufferLen = m_BufferLen + size;
	uint len = length();
	
	if ( size < 0 ) {
		
		
		
		if ( newBufferLen < 0 || newBufferLen < len )
			throw IOException("new buffer is too small!");
		
	} 
	
	
	char * newBuffer = new char[ newBufferLen ];
		
	
	if ( m_Head < m_Tail ) {

		//
		//    H   T
		// 0123456789
		// ...abcd...
		//

		memcpy( newBuffer , &m_Buffer[m_Head] , m_Tail - m_Head );

	} else if ( m_Head > m_Tail ) {

		//
		//     T  H
		// 0123456789
		// abcd...efg
		//
		 
		memcpy( newBuffer , &m_Buffer[m_Head] , m_BufferLen - m_Head );
		memcpy( &newBuffer[ m_BufferLen - m_Head ] , m_Buffer , m_Tail );

	}
		
	
	delete [] m_Buffer;
		
	
	m_Buffer = newBuffer;
	m_BufferLen = newBufferLen;
	m_Head = 0;
	m_Tail = len;	

	#ifdef __DEBUG_OUTPUT__
		ofstream ofile("buffer_resized.log",ios::app);
		ofile << "SocketOutputStream resized " << size << " bytes!" << endl;
		ofile.close();
	#endif

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get data's size in buffer
//////////////////////////////////////////////////////////////////////
uint SocketOutputStream::length () const
     throw ()
{
    if ( m_Head < m_Tail )
        return m_Tail - m_Head;
	 
    else if ( m_Head > m_Tail )
        return m_BufferLen - m_Head + m_Tail;
			 
    return 0;
}
//add by viva 2008-12-31
WORD SocketOutputStream::EncryptData(WORD EncryptKey, char* buf, int len)
	throw()
{
	return EncryptKey;

	for(int i = 0; i<len; i++)
		*(buf + i) ^= 0xCC;

	if(m_HashTable == NULL)	return EncryptKey;

	for(int i = 0; i<len; i++)
	{
		*(buf + i) ^= m_HashTable[EncryptKey];
		if(++EncryptKey == 512)	EncryptKey = 0;
	}
	
	return EncryptKey;
	
}
//end
