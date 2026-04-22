//////////////////////////////////////////////////////////////////////
//
// SocketEncryptInputStream.h
//
// by Reiot
//
//////////////////////////////////////////////////////////////////////

#ifndef __SOCKET_ENCRYPT_INPUT_STREAM_H__
#define __SOCKET_ENCRYPT_INPUT_STREAM_H__

#include <cstdint>

#include "Encrypter.h"
#include "Exception.h"
#include "SocketInputStream.h"
#include "Types.h"

const unsigned int DefaultSocketEncryptInputBufferSize = 81920;

class SocketEncryptInputStream : public SocketInputStream {
public:
    SocketEncryptInputStream(Socket* sock, uint BufferSize = DefaultSocketEncryptInputBufferSize);
    ~SocketEncryptInputStream() noexcept override = default;

public:
    uint readEncrypt(bool& buf) {
        uint n = read(buf);
        buf = m_Encrypter.convert(buf);
        return n;
    }
    uint readEncrypt(char& buf) {
        uint n = read(buf);
        buf = static_cast<char>(m_Encrypter.convert(buf));
        return n;
    }
    uint readEncrypt(uchar& buf) {
        uint n = read(buf);
        buf = m_Encrypter.convert(buf);
        return n;
    }
    uint readEncrypt(short& buf) {
        uint n = read(buf);
        buf = static_cast<short>(m_Encrypter.convert(buf));
        return n;
    }
    uint readEncrypt(ushort& buf) {
        uint n = read(buf);
        buf = m_Encrypter.convert(buf);
        return n;
    }
    uint readEncrypt(int& buf) {
        uint n = read(buf);
        buf = static_cast<int>(m_Encrypter.convert(buf));
        return n;
    }
    uint readEncrypt(uint& buf) {
        uint n = read(buf);
        buf = m_Encrypter.convert(buf);
        return n;
    }
    uint readEncrypt(long& buf) {
        int32_t tmp = 0;
        uint n = read(tmp);
        tmp = static_cast<int32_t>(m_Encrypter.convert(static_cast<long>(tmp)));
        buf = static_cast<long>(tmp);
        return n;
    }
    uint readEncrypt(ulong& buf) {
        uint32_t tmp = 0;
        uint n = read(tmp);
        tmp = static_cast<uint32_t>(m_Encrypter.convert(static_cast<ulong>(tmp)));
        buf = static_cast<ulong>(tmp);
        return n;
    }

    void setEncryptCode(uchar code) { m_Encrypter.setCode(code); }
    uchar getEncryptCode() const { return m_Encrypter.getCode(); }

private:
    Encrypter m_Encrypter;
};

#endif
