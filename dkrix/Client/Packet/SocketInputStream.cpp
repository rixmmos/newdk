//#define __TEST_PACKET_RECEIVED_SIZE_PER_SECOND__

//////////////////////////////////////////////////////////////////////
// 
// SocketInputStream.cpp
// 
// by Reiot
// 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////
#include "Client_PCH.h"
#include "SocketInputStream.h"
#include <errno.h>
#include "Assert.h"
#include "Packet.h"
#include "MinTr.h"
#include <cstdio>

#if __LINUX__
	#include <sys/ioctl.h>
#elif __WINDOWS__
	#include <winsock.h>
#endif

#ifdef __TEST_PACKET_RECEIVED_SIZE_PER_SECOND__

DWORD	g_dwReceivedSizeCheckTime=0;
DWORD	g_dwReceiveSize=0;

#endif

extern uint receiveWithDebug (Socket *pSock, void * buf , uint len);

static void TraceSocketFlow(const char* step)
{
	(void)step;
}

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
SocketInputStream::SocketInputStream ( Socket * sock , uint BufferLen )
	throw ( ProtocolException , Error )
: m_pSocket(sock), m_Buffer(NULL), m_BufferLen(BufferLen), m_Head(0), m_Tail(0)
{
	__BEGIN_TRY
		
	Assert( m_pSocket != NULL );
	Assert( m_BufferLen > 0 );
	
	m_Buffer = new char[ m_BufferLen ];

	//add by viva 2008-12-31
	m_EncryptKey = 0;
	m_HashTable = NULL;
	//end
		
	__END_CATCH
}
	

//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
SocketInputStream::~SocketInputStream ()
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	if ( m_Buffer != NULL ) {
		delete [] m_Buffer;
		m_Buffer = NULL;
	}
		
	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
//
// read data from input buffer
//
//////////////////////////////////////////////////////////////////////
uint SocketInputStream::read ( char * buf , uint len ) 
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	Assert( buf != NULL );	
	
	if ( len == 0 )
		throw InvalidProtocolException("len==0");
	
	
	
	
	
	
	if ( len > length() )
		throw InsufficientDataException( len - length() );
	
	if ( m_Head < m_Tail ) {	// normal order

		//
        //    H   T
        // 0123456789
        // ...abcd...
        //

		memcpy( buf , &m_Buffer[m_Head] , len );

	} else {					// reversed order ( m_Head > m_Tail )
		
        //
        //     T  H
        // 0123456789
        // abcd...efg
        //
	 
		uint rightLen = m_BufferLen - m_Head;
		if ( len <= rightLen ) {
			memcpy( buf , &m_Buffer[m_Head] , len );
		} else {
			memcpy( buf , &m_Buffer[m_Head] , rightLen );
			memcpy( &buf[rightLen] , m_Buffer , len - rightLen );
		}

	}

	m_Head = ( m_Head + len ) % m_BufferLen;
/*
	#ifdef __DEBUG_OUTPUT__
		if (len > 0) {
			FILE* fp = fopen("read.log", "a");
			for (int i=0; i< len ; i++) {
				fprintf(fp, " %02x", (unsigned char)buf[i]);
			}
			fclose(fp);
		}
	#endif
*/ 	
	return len;
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// read data from input buffer
//////////////////////////////////////////////////////////////////////
uint SocketInputStream::read ( std::string & str , uint len ) 
throw ( ProtocolException , Error ) {
	__BEGIN_TRY
		
	if ( len == 0 )
		throw InvalidProtocolException("len==0");
	
	
	
	
	
	
	if ( len > length() )
		throw InsufficientDataException( len - length() );
	
	
	str.reserve( len );

	if ( m_Head < m_Tail ) {	// normal order

		//
        //    H   T
        // 0123456789
        // ...abcd...
        //

		str.assign( &m_Buffer[m_Head] , len );

	} else { 					// reversed order ( m_Head > m_Tail )

        //
        //     T  H
        // 0123456789
        // abcd...efg
        //

		uint rightLen = m_BufferLen - m_Head;
		if ( len <= rightLen ) {
			str.assign( &m_Buffer[m_Head] , len );
		} else {
			str.assign( &m_Buffer[m_Head] , rightLen );
			str.append( m_Buffer , len - rightLen );
		}
	}

	// Fix: Ensure string is properly null-terminated by resizing to actual content length
	// This prevents strlen() from reading past the end when c_str() is used
	// Find first null terminator or use entire length
	size_t nullPos = str.find('\0');
	if (nullPos != std::string::npos) {
		str.resize(nullPos);
	}

	m_Head = ( m_Head + len ) % m_BufferLen;

	/*
	#ifdef __DEBUG_OUTPUT__
		if (len > 0) {
			FILE* fp = fopen("read.log", "a");
			const char *buf = str.c_str();
			for (int i=0; i< len ; i++) {
				fprintf(fp, " %02x", (unsigned char)buf[i]);
			}
			fclose(fp);
		}
	#endif
		*/

	return len;
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// read packet from input buffer
//////////////////////////////////////////////////////////////////////
void SocketInputStream::read ( Packet * pPacket ) 
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	
	
	
	
	skip( szPacketHeader );

	
	
	
	
	
	pPacket->read( *this );

//    printf("%s:%d:%s read packet: %d size: %d\n", pPacket->getPacketID(), pPacket->getPacketSize());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// peek data from buffer
//////////////////////////////////////////////////////////////////////
bool SocketInputStream::peek ( char * buf , uint len ) throw ( ProtocolException , Error )
{
//	__BEGIN_TRY
			
	Assert( buf != NULL );	

	if ( len == 0 )
		throw InvalidProtocolException("len==0");
	
	
	if ( len > length() ) {
		// throw InsufficientDataException( len - length() );
		return false;
	}

	
	if ( m_Head < m_Tail ) {	// normal order

		//
        //    H   T
        // 0123456789
        // ...abcd...
        //

		memcpy( buf , &m_Buffer[m_Head] , len );

	} else { 					// reversed order ( m_Head > m_Tail )
		
        //
        //     T  H
        // 0123456789
        // abcd...efg
        //
	 
		uint rightLen = m_BufferLen - m_Head;
		if ( len <= rightLen ) {
			memcpy( &buf[0]        , &m_Buffer[m_Head] , len );
		} else {
			memcpy( &buf[0]        , &m_Buffer[m_Head] , rightLen );
			memcpy( &buf[rightLen] , &m_Buffer[0]      , len - rightLen );
		}
	}

/*
#ifdef __DEBUG_OUTPUT__
	if (len == szPacketHeader) {
				FILE* fp = fopen("peek.log", "a");
				fprintf(fp, "(pos=%d) ", m_Head);
				for (int i=0; i<len; i++) {
					fprintf(fp, " %02x", (unsigned char)(buf[i]));
				}
				fprintf(fp, "\r\n");
				fclose(fp);
	}
#endif
*/
	
	return true;
//	__END_CATCH
}

	
//////////////////////////////////////////////////////////////////////
//
// skip data from buffer
//
// read(N) == peek(N) + skip(N)
//
//////////////////////////////////////////////////////////////////////
void SocketInputStream::skip ( uint len ) 
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	if ( len == 0 )
		throw InvalidProtocolException("len==0");
	
	if ( len > length() )
		throw InsufficientDataException( len - length() );
	
	

	uint pos = m_Head;
	m_Head = ( m_Head + len ) % m_BufferLen;

/*
	#ifdef __DEBUG_OUTPUT__
		if (len > 0) {
			FILE* fp = fopen("read.log", "a");
			fprintf(fp, "\r\n pos (%d) ", pos);
			for (uint i=0; i< len; i++) {
				pos = (pos + i) % m_BufferLen;
				fprintf(fp, " %02x", (unsigned char)(m_Buffer[pos]));
			}
			fclose(fp);
		}
	#endif
*/

	__END_CATCH
}
	

//////////////////////////////////////////////////////////////////////
//
// fill buffer from socket
//


//
// *NOTES*
//



//



//




//



//
//////////////////////////////////////////////////////////////////////
uint SocketInputStream::fill () throw ( IOException , Error )
{
	__BEGIN_TRY
		
	uint nFilled = 0;		
	uint nReceived;			
	uint nFree;				

#ifdef __TEST_PACKET_RECEIVED_SIZE_PER_SECOND__
	if( g_dwReceivedSizeCheckTime == 0 )
	{
		g_dwReceivedSizeCheckTime = timeGetTime();
		g_dwReceiveSize = 0;
	}

	if( timeGetTime() - g_dwReceivedSizeCheckTime >= 1000 )
	{
//		_MinTrace("Packet Received Size Per Second : %d %d\n", g_dwReceiveSize,timeGetTime() - g_dwReceivedSizeCheckTime );
		g_dwReceivedSizeCheckTime = timeGetTime();
		g_dwReceiveSize = 0;
	}
#endif
	

	if ( m_Head <= m_Tail ) {	// normal order
								

		
		if ( m_Head == 0 ) {

			//
			// H   T
			// 0123456789
			// abcd......
			//
			
			
			
			

			nFree = m_BufferLen - m_Tail - 1;
			nReceived = receiveWithDebug(m_pSocket, &m_Buffer[m_Tail] , nFree );

			// by tiancaiamao
			if (nReceived==0) return 0;
			//add by viva
			if(nReceived>0)
				m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
			//end

			m_Tail += nReceived;
			nFilled += nReceived;

#ifdef __TEST_PACKET_RECEIVED_SIZE_PER_SECOND__
			g_dwReceiveSize += nReceived;
#endif
			
			if ( nReceived == nFree ) {

				
				
				
				uint available = m_pSocket->available();
				if ( available > 0 ) {
					resize( available + 1 );
					
					nReceived = receiveWithDebug(m_pSocket, &m_Buffer[m_Tail] , available );
					
					//add by viva
					if(nReceived>0)
						m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
					//end
					m_Tail += nReceived;
					nFilled += nReceived;
#ifdef __TEST_PACKET_RECEIVED_SIZE_PER_SECOND__
					g_dwReceiveSize += nReceived;
#endif
				}
			}

		} else {	// m_Head != 0
			
			//
			//    H   T
			// 0123456789
			// ...abcd...
			//

			
			nFree = m_BufferLen - m_Tail;
			nReceived = receiveWithDebug(m_pSocket, &m_Buffer[m_Tail] , nFree );

			// by tiancaiamao Nonblock exception
			if (nReceived==0) return 0;
			//add by viva
			if(nReceived>0)
				m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
			//end
			m_Tail = ( m_Tail + nReceived ) % m_BufferLen;
			nFilled += nReceived;
#ifdef __TEST_PACKET_RECEIVED_SIZE_PER_SECOND__
			g_dwReceiveSize += nReceived;
#endif

			if ( nReceived == nFree ) {
				
				Assert( m_Tail == 0 );

				
				
				
				
				nFree = m_Head - 1;
				nReceived = receiveWithDebug(m_pSocket, &m_Buffer[0] , nFree );
				//add by viva
				if(nReceived>0)
					m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
				//end
				m_Tail += nReceived;
				nFilled += nReceived;
				
				if ( nReceived == nFree ) {	// buffer is full

					
					
					uint available = m_pSocket->available();
					if ( available > 0 ) {
						resize( available + 1 );
						
						nReceived = receiveWithDebug(m_pSocket, &m_Buffer[m_Tail] , available );

						// by tiancaiamao
						if (nReceived==0) return 0;
						//add by viva
						if(nReceived>0)
							m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
						//end
						m_Tail += nReceived;
						nFilled += nReceived;
#ifdef __TEST_PACKET_RECEIVED_SIZE_PER_SECOND__
						g_dwReceiveSize += nReceived;
#endif
					}
				}
			}
		}

	} else {	// reversed order ( m_Head > m_Tail )
		
		//
        //     T  H
        // 0123456789
        // abcd...efg
        //
		
		nFree = m_Head - m_Tail - 1;
		nReceived = receiveWithDebug(m_pSocket, &m_Buffer[m_Tail] , nFree );
		// by tiancaiamao
		if(nReceived==0) return 0;
		//add by viva
		if(nReceived>0)
			m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
		//end
		m_Tail += nReceived;
		nFilled += nReceived;
#ifdef __TEST_PACKET_RECEIVED_SIZE_PER_SECOND__
		g_dwReceiveSize += nReceived;
#endif
		
		if ( nReceived == nFree ) {		// buffer is full

			
			
			uint available = m_pSocket->available();
			if ( available > 0 ) {
				resize( available + 1 );
				
				nReceived = receiveWithDebug(m_pSocket, &m_Buffer[m_Tail] , available );
				if(nReceived==0) return 0;
				//add by viva
				if(nReceived>0)
					m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
				//end
				m_Tail += nReceived;
				nFilled += nReceived;
#ifdef __TEST_PACKET_RECEIVED_SIZE_PER_SECOND__
				g_dwReceiveSize += nReceived;
#endif
			}
		}

	}

	if (nFilled > 0)
	{
		char trace[160];
		sprintf(trace, "SocketInputStream::fill received=%u available=%u head=%u tail=%u", nFilled, length(), m_Head, m_Tail);
		TraceSocketFlow(trace);
	}
	return nFilled;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// fill buffer from socket
//

//
//////////////////////////////////////////////////////////////////////
uint SocketInputStream::fill_RAW ()
	 throw ( IOException , Error )
{
	__BEGIN_TRY
		
#if __LINUX__
	uint nfree = m_BufferLen - m_Tail - 1;	

	int nread = recv( m_pSocket->getSOCKET() , &m_Buffer[m_Tail] , nfree , 0 );

	if ( nread < 0 ) {
		if ( errno == EWOULDBLOCK ) {
			// NonBlockingIOException
			nread = 0;
		} else if ( errno == ECONNRESET ) {
			// ConnectException
			throw ConnectException();
		} else {
			// Error
			throw UnknownError(strerror(errno),errno);
		}
	} else if ( nread == 0 ) {
		// EOFException
		throw EOFException();
	}
	
	m_Tail += nread;
	
	if ( nread == (int)nfree ) {
		
		uint more = 0;
		int result = ioctl( m_pSocket->getSOCKET() , FIONREAD , &more );
		if ( result < 0 )
			throw UnknownError(strerror(errno),errno);
		
		if ( more > 0 ) {

			
			resize( more + 1 );

			
			nread = recv( m_pSocket->getSOCKET() , &m_Buffer[m_Tail] , more , 0 );
			
			
			Assert( (int)more == nread );
			
			nread += more;
		}		
	}
	
	return nread;

#elif __WINDOWS__
	throw UnsupportedError();
#endif

	return 0;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// resize buffer
//////////////////////////////////////////////////////////////////////
void SocketInputStream::resize ( int size )
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
		ofile << "SocketInputStream resized " << size << " bytes!" << endl;
		ofile.close();
	#endif

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// get data's size in buffer
//
// NOTES
//
//       H   T           T  H
//    0123456789     0123456789
//    ...abcd...     abcd...efg
//
//    7 - 3 = 4      10 - ( 7 - 4 ) = 7
//
// CAUTION
//



//
//////////////////////////////////////////////////////////////////////
uint SocketInputStream::length () const
     throw ()
{
	__BEGIN_TRY

	if ( m_Head < m_Tail )
		return m_Tail - m_Head;
	
	else if ( m_Head > m_Tail ) 
		return m_BufferLen - m_Head + m_Tail;
	
	return 0;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////
std::string SocketInputStream::toString () const
	throw ()
{
	StringStream msg;

	msg << "SocketInputStream("
		<< "BufferLen:" << m_BufferLen
		<< ",Head:" << m_Head
		<< ",Tail:" << m_Tail
		<< ")";

	return msg.toString();
}
//add by viva 2008-12-31
WORD SocketInputStream::EncryptData(WORD EncryptKey, char* buf, int len)
	throw()
{
	return EncryptKey;

	for(int i = 0; i<len; i++)
		*(buf + i) ^= 0xCC;
	
	if(m_HashTable == NULL) return EncryptKey;

	for(int i = 0; i<len; i++)
	{
		*(buf + i) ^= m_HashTable[EncryptKey];
		if(++EncryptKey == 512)	EncryptKey = 0;
	}
	return EncryptKey;
}
//end

// add by tiancaiamao
uint receiveWithDebug (Socket *pSock, void * buf , uint len) {
	uint ret = pSock->receive(buf,len);
	if (ret > 0) {
		char trace[160];
		sprintf(trace, "Socket recv ret=%u req=%u", ret, len);
		TraceSocketFlow(trace);
	}
/*
	#ifdef __DEBUG_OUTPUT__
		if (ret > 0) {
			FILE* fp = fopen("fill.log", "a");
			for (int i=0; i<ret; i++) {
				fprintf(fp, " %02x", (unsigned char)(((char*)buf)[i]));
			}
			fprintf(fp, "\r\n");
			fclose(fp);
		}
	#endif
*/
	return ret;
}
