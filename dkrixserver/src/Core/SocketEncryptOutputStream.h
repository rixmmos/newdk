//////////////////////////////////////////////////////////////////////
//
// SocketEncryptOutputStream.h
//
// by Reiot
//
//////////////////////////////////////////////////////////////////////

#ifndef __SOCKET_ENCRYPT_OUTPUT_STREAM_H__
#define __SOCKET_ENCRYPT_OUTPUT_STREAM_H__

// include files
#include "Encrypter.h"
#include "Exception.h"
#include "SocketOutputStream.h"
#include "Types.h"

const unsigned int DefaultSocketEncryptOutputBufferSize = 81920;

//////////////////////////////////////////////////////////////////////
//
// class SocketEncryptOutputStream
//
//////////////////////////////////////////////////////////////////////

class SocketEncryptOutputStream : public SocketOutputStream {
    //////////////////////////////////////////////////
    // constructor/destructor
    //////////////////////////////////////////////////
public:
    // constructor
    SocketEncryptOutputStream(Socket* sock, uint BufferSize = DefaultSocketEncryptOutputBufferSize);
    ~SocketEncryptOutputStream();

    //////////////////////////////////////////////////
    // methods
    //////////////////////////////////////////////////
public:
    // write data to stream (output buffer)
    // *CAUTION*
    
    
    
    
    uint writeEncrypt(bool buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(char buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(uchar buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(short buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(ushort buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(int buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(uint buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(long buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }
    uint writeEncrypt(ulong buf) {
        buf = m_Encrypter.convert(buf);
        return write(buf);
    }

    /*    uint writeEncrypt (bool   buf)  { buf = m_Encrypter.convert(buf); return write((const char*)&buf, szbool  ); }
        uint writeEncrypt (char   buf)  { buf = m_Encrypter.convert(buf); return write((const char*)&buf, szchar  ); }
        uint writeEncrypt (uchar  buf)  { buf = m_Encrypter.convert(buf); return write((const char*)&buf, szuchar ); }
        uint writeEncrypt (short  buf)  { buf = m_Encrypter.convert(buf); return write((const char*)&buf, szshort ); }
        uint writeEncrypt (ushort buf)  { buf = m_Encrypter.convert(buf); return write((const char*)&buf, szushort); }
        uint writeEncrypt (int    buf)  { buf = m_Encrypter.convert(buf); return write((const char*)&buf, szint   ); }
        uint writeEncrypt (uint   buf)  { buf = m_Encrypter.convert(buf); return write((const char*)&buf, szuint  ); }
        uint writeEncrypt (long   buf)  { buf = m_Encrypter.convert(buf); return write((const char*)&buf, szlong  ); }
        uint writeEncrypt (ulong  buf)  { buf = m_Encrypter.convert(buf); return write((const char*)&buf, szulong ); }
    */
    void setEncryptCode(uchar code) {
        m_Encrypter.setCode(code);
    }
    uchar getEncryptCode() const {
        return m_Encrypter.getCode();
    }

    //////////////////////////////////////////////////
    // attributes
    //////////////////////////////////////////////////
private:
    Encrypter m_Encrypter;
};

#endif
