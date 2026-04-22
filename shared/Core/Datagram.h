//////////////////////////////////////////////////////////////////////
//
// Filename    : Datagram.h
// Description : Shared canonical datagram wrapper
//
//////////////////////////////////////////////////////////////////////

#ifndef __DATAGRAM_H__
#define __DATAGRAM_H__

#include "Exception.h"
#include "Packet.h"
#include "SocketAPI.h"
#include "Types.h"

#if defined(__LINUX__) || defined(__APPLE__)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#elif __WINDOWS__
#include <winsock.h>
#endif

#include <cstdint>
#include <string>

class DatagramPacket;

class Datagram {
public:
    Datagram();
    ~Datagram() noexcept;

    void read(char* buf, uint len);
    void read(std::string& str, uint len);
    void read(DatagramPacket*& pPacket);

    void read(char& buf) { read((char*)&buf, szchar); }
    void read(uchar& buf) { read((char*)&buf, szuchar); }
    void read(short& buf) { read((char*)&buf, szshort); }
    void read(ushort& buf) { read((char*)&buf, szushort); }
    void read(int& buf) { read((char*)&buf, szint); }
    void read(uint& buf) { read((char*)&buf, szuint); }
    void read(long& buf) {
        int32_t tmp = 0;
        read((char*)&tmp, szlong);
        buf = static_cast<long>(tmp);
    }
    void read(ulong& buf) {
        uint32_t tmp = 0;
        read((char*)&tmp, szulong);
        buf = static_cast<ulong>(tmp);
    }

    void write(const char* buf, uint len);
    void write(const std::string& str);
    void write(const DatagramPacket* pPacket);

    void write(char buf) { write((char*)&buf, szchar); }
    void write(uchar buf) { write((char*)&buf, szuchar); }
    void write(short buf) { write((char*)&buf, szshort); }
    void write(ushort buf) { write((char*)&buf, szushort); }
    void write(int buf) { write((char*)&buf, szint); }
    void write(uint buf) { write((char*)&buf, szuint); }
    void write(long buf) {
        int32_t tmp = static_cast<int32_t>(buf);
        write((char*)&tmp, szlong);
    }
    void write(ulong buf) {
        uint32_t tmp = static_cast<uint32_t>(buf);
        write((char*)&tmp, szulong);
    }

    char* getData() { return m_Data; }

    void setData(char* data, uint len);
    void setData(uint len);

    uint getLength() const { return m_Length; }

    SOCKADDR* getAddress() { return (SOCKADDR*)&m_SockAddr; }
    void setAddress(SOCKADDR_IN* pSockAddr);

    std::string getHost() const { return std::string(inet_ntoa(m_SockAddr.sin_addr)); }
    void setHost(const std::string& host) { m_SockAddr.sin_addr.s_addr = inet_addr(host.c_str()); }

    uint getPort() const { return ntohs(m_SockAddr.sin_port); }
    void setPort(uint port) { m_SockAddr.sin_port = htons(port); }

    std::string toString() const;

private:
    bool isDatagram(PacketID_t packetID) const;

private:
    uint m_Length;
    uint m_InputOffset;
    uint m_OutputOffset;
    char* m_Data;
    SOCKADDR_IN m_SockAddr;
};

#endif
