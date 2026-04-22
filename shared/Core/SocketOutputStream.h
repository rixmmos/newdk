//////////////////////////////////////////////////////////////////////
//
// Filename    : SocketOutputStream.h
// Description : Shared canonical socket output stream
//
//////////////////////////////////////////////////////////////////////

#ifndef __SOCKET_OUTPUT_STREAM_H__
#define __SOCKET_OUTPUT_STREAM_H__

#include <cstring>
#include <string>
#include <type_traits>

#include "Endian.h"
#include "Exception.h"
#include "Socket.h"
#include "Types.h"

#ifdef __GAME_CLIENT__
const unsigned int DefaultSocketOutputBufferSize = 8192;
#else
const unsigned int DefaultSocketOutputBufferSize = 81920;
#endif

class Packet;

class SocketOutputStream {
public:
    SocketOutputStream(Socket* sock, uint BufferSize = DefaultSocketOutputBufferSize);
    virtual ~SocketOutputStream() noexcept;

    uint write(const char* buf, uint len);
    uint write(const std::string& buf) { return write(buf.c_str(), static_cast<uint>(buf.size())); }
    void writePacket(const Packet* pPacket);
    void write(const Packet* pPacket) { writePacket(pPacket); }

    template <typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value, uint>::type write(T buf);

    // Exclude pointer types from the "misc value" template so callers like
    // Player::sendPacket(Packet*) fall through to the non-template
    // write(const Packet*) overload — otherwise the template is an exact
    // match (no const-conversion) and steals the call, silently sending
    // sizeof(void*) bytes of a stack address instead of the packet header.
    template <typename T>
    typename std::enable_if<!std::is_integral<T>::value && !std::is_enum<T>::value &&
                                !std::is_pointer<T>::value,
                            uint>::type
    write(T buf);

    uint write(bool buf) { return writeScalar(buf); }
    uint write(char buf) { return writeScalar(buf); }
    uint write(uchar buf) { return writeScalar(buf); }
    uint write(short buf) { return writeScalar(buf); }
    uint write(ushort buf) { return writeScalar(buf); }
    uint write(int buf) { return writeScalar(buf); }
    uint write(uint buf) { return writeScalar(buf); }
    uint write(long buf) {
        return writeScalar(static_cast<int32_t>(buf));
    }
    uint write(ulong buf) {
        return writeScalar(static_cast<uint32_t>(buf));
    }

    uint flush();
    void resize(int size);

    int capacity() const { return m_BufferLen; }
    uint length() const;
    uint size() const { return length(); }
    char* getBuffer() const { return m_Buffer; }
    bool isEmpty() const { return m_Head == m_Tail; }
    std::string toString() const {
        StringStream msg;
        msg << "SocketOutputStream(m_BufferLen:" << m_BufferLen << ",m_Head:" << m_Head << ",m_Tail:" << m_Tail
            << ")";
        return msg.toString();
    }
    void InitSeq() { m_Sequence = 0; }

private:
    template <typename T> uint writeScalar(T buf);

private:
    Socket* m_Socket;
    char* m_Buffer;
    uint m_BufferLen;
    uint m_Head;
    uint m_Tail;
    BYTE m_Sequence;

public:
    WORD m_EncryptKey;
    BYTE* m_HashTable;
    void setKey(WORD EncryptKey, BYTE* HashTable) {
        m_EncryptKey = EncryptKey;
        m_HashTable = HashTable;
    }
    WORD EncryptData(WORD EncryptKey, char* buf, int len);
};

template <typename T> uint SocketOutputStream::writeScalar(T buf) {
    static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "writeScalar requires an integral-like type");

    char raw[sizeof(T)];
    endian::encode_little(buf, raw);
    return write(raw, sizeof(T));
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value, uint>::type SocketOutputStream::write(
    T buf) {
    return writeScalar(buf);
}

template <typename T>
typename std::enable_if<!std::is_integral<T>::value && !std::is_enum<T>::value && !std::is_pointer<T>::value,
                        uint>::type
SocketOutputStream::write(T buf) {
    return write(reinterpret_cast<const char*>(&buf), static_cast<uint>(sizeof(T)));
}

#endif
