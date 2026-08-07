//////////////////////////////////////////////////////////////////////
//
// SocketOutputStream.h
//
// by Reiot
//
//////////////////////////////////////////////////////////////////////

#ifndef __SOCKET_OUTPUT_STREAM_H__
#define __SOCKET_OUTPUT_STREAM_H__

// include files
#include <algorithm>

#include <type_traits>

#include "Exception.h"
#include "Socket.h"
#include "Types.h"

// constant definitions
const unsigned int DefaultSocketOutputBufferSize = 81920;

// forward declaration
class Packet;

//////////////////////////////////////////////////////////////////////
//
// class SocketOutputStream
//
//////////////////////////////////////////////////////////////////////

class SocketOutputStream {
    //////////////////////////////////////////////////
    // constructor/destructor
    //////////////////////////////////////////////////
public:
    // constructor
    SocketOutputStream(Socket* sock, uint BufferSize = DefaultSocketOutputBufferSize);

    // destructor
    virtual ~SocketOutputStream() noexcept;


    //////////////////////////////////////////////////
    // methods
    //////////////////////////////////////////////////
public:
    // write data to stream (output buffer)
    // *CAUTION*


    uint write(const char* buf, uint len);
    uint write(const string& buf) {
        return write(buf.c_str(), buf.size());
    }
    void writePacket(const Packet* pPacket);

    template <typename T> uint write(T buf);

    // Phase 9 (2026-08-07): endian-safe write primitive. Opt-in only --
    // write<T>() above is untouched and still does a raw host-order
    // memory cast, matching what every existing packet on the wire was
    // written with (both server and client build for x86/x86-64, i.e.
    // little-endian). writeLE<T>() encodes the value as explicit
    // little-endian regardless of host byte order, so it is
    // byte-for-byte identical to write<T>() on every host this project
    // ships on today, but would still encode correctly if a TU were
    // ever built for a big-endian target. Not wired into any packet
    // path -- callers opt in explicitly. Restricted to arithmetic
    // types: byte-order only makes sense per scalar field, not for a
    // struct reinterpreted as a blob.
    template <typename T> uint writeLE(T buf);
    /*	uint write (bool   buf)  { return write((const char*)&buf, szbool  ); }
        uint write (char   buf)  { return write((const char*)&buf, szchar  ); }
        uint write (uchar  buf)  { return write((const char*)&buf, szuchar ); }
        uint write (short  buf)  { return write((const char*)&buf, szshort ); }
        uint write (ushort buf)  { return write((const char*)&buf, szushort); }
        uint write (int    buf)  { return write((const char*)&buf, szint   ); }
        uint write (uint   buf)  { return write((const char*)&buf, szuint  ); }
        uint write (long   buf)  { return write((const char*)&buf, szlong  ); }
        uint write (ulong  buf)  { return write((const char*)&buf, szulong ); }
    */
    // flush stream (output buffer) to socket
    uint flush();

    // resize buffer
    void resize(int size);

    // get buffer length
    int capacity() const {
        return m_BufferLen;
    }

    // get data length in buffer
    uint length() const;
    uint size() const {
        return length();
    }

    // get data in buffer
    char* getBuffer() const {
        return m_Buffer;
    }

    // check if buffer is empty
    bool isEmpty() const {
        return m_Head == m_Tail;
    }

    // get debug string
    string toString() const {
        StringStream msg;
        msg << "SocketOutputStream(m_BufferLen:" << m_BufferLen << ",m_Head:" << m_Head << ",m_Tail:" << m_Tail << ")";
        return msg.toString();
    }

    //////////////////////////////////////////////////
    // attributes
    //////////////////////////////////////////////////
private:
    // socket
    Socket* m_Socket;

    // output buffer
    char* m_Buffer;

    // buffer length
    uint m_BufferLen;

    // buffer head/tail
    uint m_Head;
    uint m_Tail;


    BYTE m_Sequence;
    // add by viva 2008-12-31
public:
    WORD m_EncryptKey;
    BYTE* m_HashTable;
    void setKey(WORD EncryptKey, BYTE* HashTable) {
        m_EncryptKey = EncryptKey;
        m_HashTable = HashTable;
    };
    WORD EncryptData(WORD EncryptKey, char* buf, int len);
    // end
};


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
template <typename T> uint SocketOutputStream::write(T buf) {
    __BEGIN_TRY

    uint len = sizeof(T);


    uint nFree = ((m_Head <= m_Tail) ? m_BufferLen - m_Tail + m_Head - 1 : m_Head - m_Tail - 1);
    // m_Tail - m_Head - 1 );


    if (len >= nFree)
        resize(len - nFree + 1);

    if (m_Head <= m_Tail) // normal order
    {
        //
        //    H   T
        // 0123456789
        // ...abcd...
        //

        if (m_Head == 0) {
            nFree = m_BufferLen - m_Tail - 1;
            *((T*)(m_Buffer + m_Tail)) = buf;
        } else {
            nFree = m_BufferLen - m_Tail;
            if (len <= nFree) {
                *((T*)(m_Buffer + m_Tail)) = buf;
            } else {
                memcpy(&m_Buffer[m_Tail], (char*)&buf, nFree);
                memcpy(m_Buffer, (((char*)&buf) + nFree), len - nFree);
            }
        }
    } else // reversed order
    {
        //
        //     T  H
        // 0123456789
        // abcd...efg
        //
        *((T*)(m_Buffer + m_Tail)) = buf;
    }

    // advance m_Tail
    m_Tail = (m_Tail + len) % m_BufferLen;

    return len;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// write data to output buffer, explicit little-endian wire order
// (Phase 9, 2026-08-07 -- see declaration above)
//
//////////////////////////////////////////////////////////////////////
template <typename T> uint SocketOutputStream::writeLE(T buf) {
    __BEGIN_TRY

    static_assert(std::is_arithmetic<T>::value, "writeLE<T> requires an arithmetic type");

    unsigned char tmp[sizeof(T)];
    memcpy(tmp, &buf, sizeof(T));

    static const unsigned short kEndianProbe = 0x0001;
    bool hostIsLittleEndian = (*reinterpret_cast<const unsigned char*>(&kEndianProbe) == 0x01);
    if (!hostIsLittleEndian)
        std::reverse(tmp, tmp + sizeof(T));

    return write((const char*)tmp, (uint)sizeof(T));

    __END_CATCH
}

#endif
