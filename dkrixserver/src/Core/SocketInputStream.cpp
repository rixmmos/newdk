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
#include "SocketInputStream.h"

#include <errno.h>

#include "Assert.h"
#include "Packet.h"

#if __LINUX__
#include <sys/ioctl.h>
#elif __WINDOWS__
#include <winsock.h>
#endif


//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
SocketInputStream::SocketInputStream(Socket* sock, uint BufferLen)
    : m_pSocket(sock), m_Buffer(NULL), m_BufferLen(BufferLen), m_Head(0), m_Tail(0) {
    __BEGIN_TRY

    Assert(m_pSocket != NULL);
    Assert(m_BufferLen > 0);

    m_Buffer = new char[m_BufferLen];
    // add by viva 2008-12-31
    m_EncryptKey = 0;
    m_HashTable = NULL;
    // end

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
SocketInputStream::~SocketInputStream() noexcept {
    SAFE_DELETE_ARRAY(m_Buffer);
}


//////////////////////////////////////////////////////////////////////
//
// read data from input buffer
//
//////////////////////////////////////////////////////////////////////
uint SocketInputStream::read(char* buf, uint len) {
    //	__BEGIN_TRY

    Assert(buf != NULL);

    if (len == 0)
        throw InvalidProtocolException("len==0");

    
    
    
    
    
    if (len > length())
        throw InsufficientDataException(len - length());

    if (m_Head < m_Tail) { // normal order

        //
        //    H   T
        // 0123456789
        // ...abcd...
        //

        memcpy(buf, &m_Buffer[m_Head], len);

    } else { // reversed order ( m_Head > m_Tail )

        //
        //     T  H
        // 0123456789
        // abcd...efg
        //

        uint rightLen = m_BufferLen - m_Head;
        if (len <= rightLen) {
            memcpy(buf, &m_Buffer[m_Head], len);
        } else {
            memcpy(buf, &m_Buffer[m_Head], rightLen);
            memcpy(&buf[rightLen], m_Buffer, len - rightLen);
        }
    }

    m_Head = (m_Head + len) % m_BufferLen;

    return len;

    //	__END_CATCH
}


//////////////////////////////////////////////////////////////////////
// read data from input buffer
//////////////////////////////////////////////////////////////////////
uint SocketInputStream::read(string& str, uint len) {
    __BEGIN_TRY

    if (len == 0)
        throw InvalidProtocolException("len==0");

    
    
    
    
    
    if (len > length())
        throw InsufficientDataException(len - length());

    
    str.reserve(len);

    if (m_Head < m_Tail) { // normal order

        //
        //    H   T
        // 0123456789
        // ...abcd...
        //

        str.assign(&m_Buffer[m_Head], len);

    } else { // reversed order ( m_Head > m_Tail )

        //
        //     T  H
        // 0123456789
        // abcd...efg
        //

        uint rightLen = m_BufferLen - m_Head;
        if (len <= rightLen) {
            str.assign(&m_Buffer[m_Head], len);
        } else {
            str.assign(&m_Buffer[m_Head], rightLen);
            str.append(m_Buffer, len - rightLen);
        }
    }

    m_Head = (m_Head + len) % m_BufferLen;

    return len;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// read packet from input buffer
//////////////////////////////////////////////////////////////////////
void SocketInputStream::readPacket(Packet* pPacket) {
    __BEGIN_TRY

    
    
    
    skip(szPacketHeader);

    
    
    
    
    // cout<<"Receive_before:"<<pPacket->toString()<<endl;
    pPacket->read(*this);

    // This trace ran unconditionally on every packet the server received, and
    // the cost is not the write alone: Packet::toString() builds a StringStream,
    // which is a list<string> that heap-allocates one node per field, then
    // concatenates. So every CGMove -- 10 bytes on the wire -- cost a handful of
    // allocations, a string concatenation, and a ~33 byte line on stdout, which
    // docker/start-servers.sh redirects into gameserver.out. That is an
    // unbounded log file driven directly by client packet rate: a connected
    // client that spams movement fills the disk, and every player pays the
    // allocation cost during normal play.
    //
    // The diagnostic is not lost. GamePlayer already carries a better one --
    // m_bPacketLog writes pPacket->toString() to a per-player, time-bounded file
    // (GamePlayer::processInput and ::sendPacket) -- which is targeted rather
    // than global. Define __TRACE_PACKETS__ to get the firehose back.
#ifdef __TRACE_PACKETS__
    cout << "Receive:" << pPacket->toString() << endl;
#endif

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// peek data from buffer
//////////////////////////////////////////////////////////////////////
bool SocketInputStream::peek(char* buf, uint len) {
    //	__BEGIN_TRY

    Assert(buf != NULL);

    if (len == 0)
        throw InvalidProtocolException("len==0");

    
    if (len > length())
        // throw InsufficientDataException( len - length() );
        
        return false;

    
    if (m_Head < m_Tail) { // normal order

        //
        //    H   T
        // 0123456789
        // ...abcd...
        //

        memcpy(buf, &m_Buffer[m_Head], len);

    } else { // reversed order ( m_Head > m_Tail )

        //
        //     T  H
        // 0123456789
        // abcd...efg
        //

        uint rightLen = m_BufferLen - m_Head;
        if (len <= rightLen) {
            memcpy(&buf[0], &m_Buffer[m_Head], len);
        } else {
            memcpy(&buf[0], &m_Buffer[m_Head], rightLen);
            memcpy(&buf[rightLen], &m_Buffer[0], len - rightLen);
        }
    }

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
void SocketInputStream::skip(uint len) {
    __BEGIN_TRY

    if (len == 0)
        throw InvalidProtocolException("len==0");

    if (len > length())
        throw InsufficientDataException(len - length());

    
    m_Head = (m_Head + len) % m_BufferLen;

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





// )
//



//
//////////////////////////////////////////////////////////////////////
uint SocketInputStream::fill() {
    __BEGIN_TRY

    uint nFilled = 0; 
    uint nReceived;   
    uint nFree;       

    if (m_Head <= m_Tail) { // normal order
                            

        
        if (m_Head == 0) {
            //
            // H   T
            // 0123456789
            // abcd......
            //

            
            
            

            nFree = m_BufferLen - m_Tail - 1;
            nReceived = m_pSocket->receive(&m_Buffer[m_Tail], nFree);

            
            if (nReceived == 0)
                return 0;
            // add by viva
            if (nReceived > 0)
                m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
            // end
            m_Tail += nReceived;
            nFilled += nReceived;

            if (nReceived == nFree) {
                
                
                
                uint available = m_pSocket->available();
                if (available > 0) {
                    resize(available + 1);
                    
                    nReceived = m_pSocket->receive(&m_Buffer[m_Tail], available);

                    
                    if (nReceived == 0)
                        return 0;
                    // add by viva
                    if (nReceived > 0)
                        m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
                    // end
                    m_Tail += nReceived;
                    nFilled += nReceived;
                }
            }

        } else { // m_Head != 0

            //
            //    H   T
            // 0123456789
            // ...abcd...
            //

            
            
            nFree = m_BufferLen - m_Tail;
            nReceived = m_pSocket->receive(&m_Buffer[m_Tail], nFree);

            
            if (nReceived == 0)
                return 0;
            // add by viva
            if (nReceived > 0)
                m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
            // end
            m_Tail = (m_Tail + nReceived) % m_BufferLen;
            nFilled += nReceived;

            if (nReceived == nFree) {
                Assert(m_Tail == 0);

                
                
                
                
                nFree = m_Head - 1;
                nReceived = m_pSocket->receive(&m_Buffer[0], nFree);

                
                if (nReceived == 0)
                    return 0;
                // add by viva
                if (nReceived > 0)
                    m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
                // end
                m_Tail += nReceived;
                nFilled += nReceived;

                if (nReceived == nFree) { // buffer is full

                    
                    
                    
                    uint available = m_pSocket->available();
                    if (available > 0) {
                        resize(available + 1);
                        
                        nReceived = m_pSocket->receive(&m_Buffer[m_Tail], available);

                        
                        if (nReceived == 0)
                            return 0;
                        // add by viva
                        if (nReceived > 0)
                            m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
                        // end
                        m_Tail += nReceived;
                        nFilled += nReceived;
                    }
                }
            }
        }

    } else { // reversed order ( m_Head > m_Tail )

        //
        //     T  H
        // 0123456789
        // abcd...efg
        //

        nFree = m_Head - m_Tail - 1;
        nReceived = m_pSocket->receive(&m_Buffer[m_Tail], nFree);

        
        if (nReceived == 0)
            return 0;
        // add by viva
        if (nReceived > 0)
            m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
        // end
        m_Tail += nReceived;
        nFilled += nReceived;

        if (nReceived == nFree) { // buffer is full

            
            
            
            uint available = m_pSocket->available();
            if (available > 0) {
                resize(available + 1);
                
                nReceived = m_pSocket->receive(&m_Buffer[m_Tail], available);

                
                if (nReceived == 0)
                    return 0;
                // add by viva
                if (nReceived > 0)
                    m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail], nReceived);
                // end
                m_Tail += nReceived;
                nFilled += nReceived;
            }
        }
    }
    // add by viva 2008-12-31
    //	if(nFilled>0)
    //	{
    //		if(m_Tail>=nFilled)
    //			m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_Tail-nFilled], nFilled);
    //		else
    //		{
    //			m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[m_BufferLen - m_Tail], nFilled - m_Tail);
    //			m_EncryptKey = EncryptData(m_EncryptKey, &m_Buffer[0], m_Tail);
    //		}
    //	}
    // end
    return nFilled;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// fill buffer from socket
//

//
//////////////////////////////////////////////////////////////////////
uint SocketInputStream::fill_RAW() {
    __BEGIN_TRY

#if __LINUX__
    uint nfree = m_BufferLen - m_Tail - 1;

    int nread = recv(m_pSocket->getSOCKET(), &m_Buffer[m_Tail], nfree, 0);

    if (nread < 0) {
        if (errno == EWOULDBLOCK) {
            // NonBlockingIOException
            nread = 0;
        } else if (errno == ECONNRESET) {
            // ConnectException
            throw ConnectException();
        } else {
            // Error
            throw UnknownError(strerror(errno), errno);
        }
    } else if (nread == 0) {
        // EOFException
        throw EOFException();
    }

    m_Tail += nread;

    if (nread == (int)nfree) {
        
        uint more = 0;
        int result = ioctl(m_pSocket->getSOCKET(), FIONREAD, &more);
        if (result < 0)
            throw UnknownError(strerror(errno), errno);

        if (more > 0) {
            
            resize(more + 1);

            
            nread = recv(m_pSocket->getSOCKET(), &m_Buffer[m_Tail], more, 0);

            
            
            Assert((int)more == nread);

            nread += more;
        }
    }

    return nread;

#elif __WINDOWS__
    throw UnsupportedError();
#endif

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// resize buffer
//////////////////////////////////////////////////////////////////////
void SocketInputStream::resize(int size) {
    __BEGIN_TRY

    Assert(size != 0);

    
    size = max(size, (int)(m_BufferLen >> 1));
    uint newBufferLen = m_BufferLen + size;
    uint len = length();

    if (size < 0) {
        
        
        if (newBufferLen < 0 || newBufferLen < len)
            throw IOException("new buffer is too small!");
    }

    
    char* newBuffer = new char[newBufferLen];

    
    if (m_Head < m_Tail) {
        //
        //    H   T
        // 0123456789
        // ...abcd...
        //

        memcpy(newBuffer, &m_Buffer[m_Head], m_Tail - m_Head);

    } else if (m_Head > m_Tail) {
        //
        //     T  H
        // 0123456789
        // abcd...efg
        //

        memcpy(newBuffer, &m_Buffer[m_Head], m_BufferLen - m_Head);
        memcpy(&newBuffer[m_BufferLen - m_Head], m_Buffer, m_Tail);
    }

    
    delete[] m_Buffer;

    
    m_Buffer = newBuffer;
    m_BufferLen = newBufferLen;
    m_Head = 0;
    m_Tail = len; 

    ofstream ofile("buffer_resized.log", ios::app);
    ofile << "SocketInputStream resized " << size << " bytes!" << endl;
    ofile.close();

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
uint SocketInputStream::length() const {
    __BEGIN_TRY

    if (m_Head < m_Tail)
        return m_Tail - m_Head;

    else if (m_Head > m_Tail)
        return m_BufferLen - m_Head + m_Tail;

    return 0;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////
string SocketInputStream::toString() const {
    StringStream msg;

    msg << "SocketInputStream("
        << "BufferLen:" << m_BufferLen << ",Head:" << m_Head << ",Tail:" << m_Tail << ")";

    return msg.toString();
}
// add by viva 2008-12-31
WORD SocketInputStream::EncryptData(WORD EncryptKey, char* buf, int len) {
    return EncryptKey;

    for (int i = 0; i < len; i++)
        *(buf + i) ^= 0xCC;

    if (m_HashTable == NULL)
        return EncryptKey;

    for (int i = 0; i < len; i++) {
        *(buf + i) ^= m_HashTable[EncryptKey];
        if (++EncryptKey == 512)
            EncryptKey = 0;
    }
    return EncryptKey;
}
// end
