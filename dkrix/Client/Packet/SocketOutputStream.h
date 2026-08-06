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
#include "Packet/Types.h"
#include "Exception.h"
#include "Socket.h"

// constant definitions
const unsigned int DefaultSocketOutputBufferSize = 8192;

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
public :
	
	// constructor
	SocketOutputStream ( Socket * sock , uint BufferSize = DefaultSocketOutputBufferSize ) throw ( ProtocolException , Error );
	
	// destructor
	virtual ~SocketOutputStream () throw ( ProtocolException , Error );

	
//////////////////////////////////////////////////
// methods
//////////////////////////////////////////////////
public :
	
	// write data to stream (output buffer)
	// *CAUTION*
	
	
	
	
	uint write ( const char * buf , uint len ) throw ( ProtocolException , Error );
	uint write ( const std::string & buf ) throw ( ProtocolException , Error ) { return write(buf.c_str(),buf.size()); }
	void write ( const Packet * pPacket ) throw ( ProtocolException , Error );
	
    uint write ( bool   buf ) throw ( ProtocolException , Error ) { return write( (const char*)&buf, szbool   ); }
    uint write ( char   buf ) throw ( ProtocolException , Error ) { return write( (const char*)&buf, szchar   ); }
    uint write ( uchar  buf ) throw ( ProtocolException , Error ) { return write( (const char*)&buf, szuchar  ); }
    uint write ( short  buf ) throw ( ProtocolException , Error ) { return write( (const char*)&buf, szshort  ); }
    uint write ( ushort buf ) throw ( ProtocolException , Error ) { return write( (const char*)&buf, szushort ); }
    uint write ( int    buf ) throw ( ProtocolException , Error ) { return write( (const char*)&buf, szint    ); }
    uint write ( uint   buf ) throw ( ProtocolException , Error ) { return write( (const char*)&buf, szuint   ); }
    uint write ( long   buf ) throw ( ProtocolException , Error ) {
        int32_t tmp = static_cast<int32_t>(buf);
        return write( (const char*)&tmp, szlong );
    }
    uint write ( ulong  buf ) throw ( ProtocolException , Error ) {
        uint32_t tmp = static_cast<uint32_t>(buf);
        return write( (const char*)&tmp, szulong );
    }

	// flush stream (output buffer) to socket
	uint flush () throw ( IOException , Error );

	// resize buffer 
	void resize ( int size ) throw ( IOException , Error );

	// get buffer length
	int capacity () const throw () { return m_BufferLen; }
 
    // get data length in buffer
    uint length () const throw ();
    uint size () const throw () { return length(); }
 
    // check if buffer is empty
    bool isEmpty () const throw () { return m_Head == m_Tail; }

    // get debug string
    std::string toString () const throw ()
    {
        StringStream msg;
        msg << "SocketOutputStream(m_BufferLen:"<<m_BufferLen<<",m_Head:"<<m_Head<<",m_Tail:"<<m_Tail
<<")";
        return msg.toString();
    }
	
	void InitSeq(){ m_Sequence =0;}
	virtual void resetState () throw ()
	{
		m_Head = 0;
		m_Tail = 0;
		m_Sequence = 0;
		m_EncryptKey = 0;
		m_HashTable = NULL;
	}
//////////////////////////////////////////////////
// attributes
//////////////////////////////////////////////////
private :
	
	// socket
	Socket * m_Socket;
	
	// output buffer
	char * m_Buffer;
	
	// buffer length
	uint m_BufferLen;
	
	// buffer head/tail
	uint m_Head;
	uint m_Tail;
	
	BYTE m_Sequence;
//add by viva 2008-12-31
public :
	WORD m_EncryptKey;
	BYTE* m_HashTable;
	void setKey(WORD EncryptKey, BYTE* HashTable) throw() { m_EncryptKey = EncryptKey; m_HashTable = HashTable;};
	WORD EncryptData(WORD EncryptKey, char* buf, int len) throw();
//end
};

#endif
