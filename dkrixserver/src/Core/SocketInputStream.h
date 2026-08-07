//////////////////////////////////////////////////////////////////////
//
// Filename    : SocketInputStream.h
// Written by  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////
//
// *Reiot's Notes*
//



//



//
//////////////////////////////////////////////////////////////////////

#ifndef __SOCKET_INPUT_STREAM_H__
#define __SOCKET_INPUT_STREAM_H__

// include files
#include <algorithm>

#include <type_traits>

#include "Exception.h"
#include "Socket.h"
#include "Types.h"

// constant definitions
const uint DefaultSocketInputBufferSize = 81920;

// forward declaration
class Packet;

//////////////////////////////////////////////////////////////////////
//
// class SocketInputStream
//
//////////////////////////////////////////////////////////////////////

class SocketInputStream {
    //////////////////////////////////////////////////
    // constructor/destructor
    //////////////////////////////////////////////////
public:
    // constructor
    SocketInputStream(Socket* sock, uint BufferSize = DefaultSocketInputBufferSize);

    // destructor
    virtual ~SocketInputStream() noexcept;


    //////////////////////////////////////////////////
    // methods
    //////////////////////////////////////////////////
public:
    // read data from stream (input buffer)
    uint read(char* buf, uint len);
    uint read(string& str, uint len);
    void readPacket(Packet* p);

    template <typename T> uint read(T& buf);

    // Phase 9 (2026-08-07): endian-safe read primitive. Opt-in only --
    // read<T>() above is untouched and still does a raw host-order
    // memory cast, which is what every existing packet on the wire
    // was written with (both server and client build for x86/x86-64,
    // i.e. little-endian). readLE<T>() decodes the wire bytes as
    // explicit little-endian regardless of host byte order, so it is
    // byte-for-byte identical to read<T>() on every host this project
    // ships on today, but would still decode correctly if a TU were
    // ever built for a big-endian target. Not wired into any packet
    // path -- callers opt in explicitly. Restricted to arithmetic
    // types: byte-order only makes sense per scalar field, not for a
    // struct reinterpreted as a blob.
    template <typename T> uint readLE(T& buf);

    /*	uint read (bool   & buf)  { return read((char*)&buf, szbool  ); }
        uint read (char   & buf)  { return read((char*)&buf, szchar  ); }
        uint read (uchar  & buf)  { return read((char*)&buf, szuchar ); }
        uint read (short  & buf)  { return read((char*)&buf, szshort ); }
        uint read (ushort & buf)  { return read((char*)&buf, szushort); }
        uint read (int    & buf)  { return read((char*)&buf, szint   ); }
        uint read (uint   & buf)  { return read((char*)&buf, szuint  ); }
        uint read (long   & buf)  { return read((char*)&buf, szlong  ); }
        uint read (ulong  & buf)  { return read((char*)&buf, szulong ); }
    */
    // peek data from stream (input buffer)
    bool peek(char* buf, uint len);

    // skip data from stream (input buffer)
    void skip(uint len);

    // fill stream (input buffer) from socket
    uint fill();
    uint fill_RAW();

    // resize buffer
    void resize(int size);

    // get buffer length
    uint capacity() const {
        return m_BufferLen;
    }

    // get data length in buffer
    uint length() const;
    uint size() const {
        return length();
    }

    // check if buffer is empty
    bool isEmpty() const {
        return m_Head == m_Tail;
    }

    // get debug string
    string toString() const;


    //////////////////////////////////////////////////
    // attributes
    //////////////////////////////////////////////////
private:
    // socket
    Socket* m_pSocket;

    // buffer
    char* m_Buffer;

    // buffer length
    uint m_BufferLen;

    // buffer head/tail
    uint m_Head;
    uint m_Tail;
    // add by viva 2008-12-31
public:
    WORD m_EncryptKey;
    BYTE* m_HashTable;
    void setKey(WORD EncryptKey, BYTE* HashTable) {
        m_EncryptKey = EncryptKey;
        m_HashTable = HashTable;
    }
    WORD EncryptData(WORD EncryptKey, char* buf, int len);
    // end
};


//////////////////////////////////////////////////////////////////////
//
// read data from input buffer
//
//////////////////////////////////////////////////////////////////////
template <typename T> uint SocketInputStream::read(T& buf) {
    uint len = (uint)sizeof(T);

    
    
    
    
    
    if (len > length())
        throw InsufficientDataException(len - length());

    if (m_Head < m_Tail) // normal order
    {
        //
        //    H   T
        // 0123456789
        // ...abcd...
        //
        buf = *(T*)(m_Buffer + m_Head);

    } else // reversed order ( m_Head > m_Tail )
    {
        //
        //     T  H
        // 0123456789
        // abcd...efg
        //
        uint rightLen = m_BufferLen - m_Head;
        if (len <= rightLen) {
            buf = *(T*)(m_Buffer + m_Head);
        } else {
            memcpy((char*)&buf, &m_Buffer[m_Head], rightLen);
            memcpy(((char*)(&buf) + rightLen), m_Buffer, len - rightLen);
        }
    }

    m_Head = (m_Head + len) % m_BufferLen;

    return len;

    //	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// read data from input buffer, explicit little-endian wire order
// (Phase 9, 2026-08-07 -- see declaration above)
//
//////////////////////////////////////////////////////////////////////
template <typename T> uint SocketInputStream::readLE(T& buf) {
    static_assert(std::is_arithmetic<T>::value, "readLE<T> requires an arithmetic type");

    unsigned char tmp[sizeof(T)];
    uint n = read((char*)tmp, (uint)sizeof(T));

    static const unsigned short kEndianProbe = 0x0001;
    bool hostIsLittleEndian = (*reinterpret_cast<const unsigned char*>(&kEndianProbe) == 0x01);
    if (!hostIsLittleEndian)
        std::reverse(tmp, tmp + sizeof(T));

    memcpy(&buf, tmp, sizeof(T));
    return n;
}

#endif
