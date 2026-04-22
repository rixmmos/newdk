//////////////////////////////////////////////////////////////////////
//
// Filename    : SocketInputStream.h
// Description : Shared canonical socket input stream
//
//////////////////////////////////////////////////////////////////////

#ifndef __SOCKET_INPUT_STREAM_H__
#define __SOCKET_INPUT_STREAM_H__

#include <cstring>
#include <string>
#include <type_traits>

#include "Endian.h"
#include "Exception.h"
#include "Socket.h"
#include "Types.h"

#ifdef __GAME_CLIENT__
const uint DefaultSocketInputBufferSize = 8192;
#else
const uint DefaultSocketInputBufferSize = 81920;
#endif

class Packet;

class SocketInputStream {
public:
    SocketInputStream(Socket* sock, uint BufferSize = DefaultSocketInputBufferSize);
    virtual ~SocketInputStream() noexcept;

    uint read(char* buf, uint len);
    uint read(std::string& str, uint len);
    void readPacket(Packet* pPacket);
    void read(Packet* pPacket) { readPacket(pPacket); }

    template <typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value, uint>::type read(T& buf);

    template <typename T>
    typename std::enable_if<!std::is_integral<T>::value && !std::is_enum<T>::value, uint>::type read(T& buf);

    uint read(bool& buf) { return readScalar(buf); }
    uint read(char& buf) { return readScalar(buf); }
    uint read(uchar& buf) { return readScalar(buf); }
    uint read(short& buf) { return readScalar(buf); }
    uint read(ushort& buf) { return readScalar(buf); }
    uint read(int& buf) { return readScalar(buf); }
    uint read(uint& buf) { return readScalar(buf); }
    uint read(long& buf) {
        int32_t tmp = 0;
        uint ret = readScalar(tmp);
        buf = static_cast<long>(tmp);
        return ret;
    }
    uint read(ulong& buf) {
        uint32_t tmp = 0;
        uint ret = readScalar(tmp);
        buf = static_cast<ulong>(tmp);
        return ret;
    }

    bool peek(char* buf, uint len);
    void skip(uint len);
    uint fill();
    uint fill_RAW();
    void resize(int size);

    uint capacity() const { return m_BufferLen; }
    uint length() const;
    uint size() const { return length(); }
    bool isEmpty() const { return m_Head == m_Tail; }
    std::string toString() const;

private:
    template <typename T> uint readScalar(T& buf);

private:
    Socket* m_pSocket;
    char* m_Buffer;
    uint m_BufferLen;
    uint m_Head;
    uint m_Tail;

public:
    WORD m_EncryptKey;
    BYTE* m_HashTable;
    void setKey(WORD EncryptKey, BYTE* HashTable) {
        m_EncryptKey = EncryptKey;
        m_HashTable = HashTable;
    }
    WORD EncryptData(WORD EncryptKey, char* buf, int len);
};

template <typename T> uint SocketInputStream::readScalar(T& buf) {
    static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "readScalar requires an integral-like type");

    char raw[sizeof(T)];
    uint ret = read(raw, sizeof(T));
    buf = endian::decode_little<T>(raw);
    return ret;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value, uint>::type SocketInputStream::read(
    T& buf) {
    return readScalar(buf);
}

template <typename T>
typename std::enable_if<!std::is_integral<T>::value && !std::is_enum<T>::value, uint>::type SocketInputStream::read(
    T& buf) {
    return read(reinterpret_cast<char*>(&buf), static_cast<uint>(sizeof(T)));
}

#endif
