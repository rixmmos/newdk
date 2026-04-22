//////////////////////////////////////////////////////////////////////
//
// SocketEncryptOutputStream.h
//
// by Reiot
//
//////////////////////////////////////////////////////////////////////

#ifndef __SOCKET_ENCRYPT_OUTPUT_STREAM_H__
#define __SOCKET_ENCRYPT_OUTPUT_STREAM_H__

#include <cstdint>

#include "Encrypter.h"
#include "Exception.h"
#include "SocketOutputStream.h"
#include "Types.h"

const unsigned int DefaultSocketEncryptOutputBufferSize = 81920;

class SocketEncryptOutputStream : public SocketOutputStream {
public:
    SocketEncryptOutputStream(Socket* sock, uint BufferSize = DefaultSocketEncryptOutputBufferSize);
    ~SocketEncryptOutputStream() noexcept override = default;

public:
    uint writeEncrypt(bool buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(char buf) {
        buf = static_cast<char>(m_Encrypter.convert(buf));
        return write(buf);
    }
    uint writeEncrypt(uchar buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(short buf) {
        buf = static_cast<short>(m_Encrypter.convert(buf));
        return write(buf);
    }
    uint writeEncrypt(ushort buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(int buf) {
        buf = static_cast<int>(m_Encrypter.convert(buf));
        return write(buf);
    }
    uint writeEncrypt(uint buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(long buf) {
        int32_t tmp = static_cast<int32_t>(buf);
        tmp = static_cast<int32_t>(m_Encrypter.convert(static_cast<long>(tmp)));
        return write(tmp);
    }
    uint writeEncrypt(ulong buf) {
        uint32_t tmp = static_cast<uint32_t>(buf);
        tmp = static_cast<uint32_t>(m_Encrypter.convert(static_cast<ulong>(tmp)));
        return write(tmp);
    }

    void setEncryptCode(uchar code) { m_Encrypter.setCode(code); }
    uchar getEncryptCode() const { return m_Encrypter.getCode(); }

private:
    Encrypter m_Encrypter;
};

#endif
