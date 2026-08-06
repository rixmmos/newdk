//////////////////////////////////////////////////////////////////////
// 
// SocketEncryptInputStream.h 
// 
// by Reiot
// 
//////////////////////////////////////////////////////////////////////

#ifndef __SOCKET_ENCRYPT_INPUT_STREAM_H__
#define __SOCKET_ENCRYPT_INPUT_STREAM_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "SocketInputStream.h"
#include "Encrypter.h"

const unsigned int DefaultSocketEncryptInputBufferSize = 81920;

//////////////////////////////////////////////////////////////////////
//
// class SocketEncryptInputStream
//
//////////////////////////////////////////////////////////////////////

class SocketEncryptInputStream : public SocketInputStream {

//////////////////////////////////////////////////
// constructor/destructor
//////////////////////////////////////////////////
public :
	
	// constructor
	SocketEncryptInputStream (Socket* sock, uint BufferSize = DefaultSocketEncryptInputBufferSize) throw (Error);
	~SocketEncryptInputStream() {}
	
//////////////////////////////////////////////////
// methods
//////////////////////////////////////////////////
public :
	
	// read data to stream (output buffer)
	// *CAUTION*
	
	
	
	
    uint readEncrypt (bool   &buf) throw (ProtocolException, Error) { uint re = read((char*)&buf, szbool  ); buf = m_Encrypter.convert(buf); return re; }
    uint readEncrypt (char   &buf) throw (ProtocolException, Error) { uint re = read((char*)&buf, szchar  ); buf = m_Encrypter.convert(buf); return re; }
    uint readEncrypt (uchar  &buf) throw (ProtocolException, Error) { uint re = read((char*)&buf, szuchar ); buf = m_Encrypter.convert(buf); return re; }
    uint readEncrypt (short  &buf) throw (ProtocolException, Error) { uint re = read((char*)&buf, szshort ); buf = m_Encrypter.convert(buf); return re; }
    uint readEncrypt (ushort &buf) throw (ProtocolException, Error) { uint re = read((char*)&buf, szushort); buf = m_Encrypter.convert(buf); return re; }
    uint readEncrypt (int    &buf) throw (ProtocolException, Error) { uint re = read((char*)&buf, szint   ); buf = m_Encrypter.convert(buf); return re; }
    uint readEncrypt (uint   &buf) throw (ProtocolException, Error) { uint re = read((char*)&buf, szuint  ); buf = m_Encrypter.convert(buf); return re; }
    uint readEncrypt (long   &buf) throw (ProtocolException, Error) {
        int32_t tmp = 0;
        uint re = read((char*)&tmp, szlong);
        tmp = static_cast<int32_t>(m_Encrypter.convert(static_cast<long>(tmp)));
        buf = static_cast<long>(tmp);
        return re;
    }
    uint readEncrypt (ulong  &buf) throw (ProtocolException, Error) {
        uint32_t tmp = 0;
        uint re = read((char*)&tmp, szulong);
        tmp = static_cast<uint32_t>(m_Encrypter.convert(static_cast<ulong>(tmp)));
        buf = static_cast<ulong>(tmp);
        return re;
    }

	void	setEncryptCode(uchar code)	{ m_Encrypter.setCode(code); }
	uchar	getEncryptCode() const		{ return m_Encrypter.getCode(); }
	virtual void resetState() throw ()
	{
		SocketInputStream::resetState();
		m_Encrypter.setCode(0);
	}

//////////////////////////////////////////////////
// attributes
//////////////////////////////////////////////////
private :
	Encrypter m_Encrypter;
	
};

#endif
