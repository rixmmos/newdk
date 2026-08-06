//////////////////////////////////////////////////////////////////////
// 
// Filename    : Player.h 
// Written by  : reiot@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __PLAYER_H__
#define __PLAYER_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"

// forward declaration
class Socket;
class SocketInputStream;
class SocketOutputStream;
class Packet;

//////////////////////////////////////////////////////////////////////
//
// class Player
//





//
// *CAUTION*
//


//
//////////////////////////////////////////////////////////////////////

const bool UNDISCONNECTED = true;
const bool DISCONNECTED   = false;

class Player {

public :

	// constructor
	Player() throw (Error);
	Player ( Socket * pSocket ) throw ( ProtocolException , Error );
	
	// destructor
	virtual ~Player () throw ( ProtocolException , Error );

	// read socket's receive buffer and fill input buffer
	virtual void processInput () throw ( IOException , Error );
	
	// parse packet and execute handler for the packet
	virtual void processCommand () throw ( IOException , Error );
	
	// flush output buffer to socket's send buffer
	virtual void processOutput () throw ( IOException , Error );
	
	// send packet to player's output buffer
	virtual void sendPacket ( Packet * pPacket ) throw ( ProtocolException , Error );

	// disconnect
	
	
	
	virtual void disconnect ( bool bDisconnected = DISCONNECTED ) throw ( ProtocolException , Error );
	
	// get/set socket
	Socket * getSocket () throw () { return m_pSocket; }
	void setSocket ( Socket * pSocket ) throw ();

	// get/set player ID
	std::string getID () const throw () { return m_ID; }
	void setID ( const std::string & id ) throw () { m_ID = id; }

	// get debug std::string
	virtual std::string toString () const throw ( ProtocolException , Error );

	//add by viva
	void setKey(WORD EncryptKey, WORD HashKey) throw();
	void delKey() throw();
	void resetStreams() throw();

	BYTE* pHashTable;
	//end
	
protected :

	// player id
	std::string m_ID;
	
	// TCP client socket
	Socket * m_pSocket;
	
	// buffered socket input stream
	SocketInputStream * m_pInputStream;

	// buffered socket output stream
	SocketOutputStream * m_pOutputStream;
	
};

#endif
