//--------------------------------------------------------------------------------
// 
// Filename    : RequestServerPlayer.h 
// Written by  : sigi
// 
//--------------------------------------------------------------------------------

#ifndef __REQUEST_SERVER_PLAYER_H__
#define __REQUEST_SERVER_PLAYER_H__

// include files
#include "Player.h"
#include "PlayerStatus.h"
#include "Packet.h"
#include "types/RequestTypes.h"

//--------------------------------------------------------------------------------
//
// class RequestServerPlayer
//

//
//--------------------------------------------------------------------------------

class RequestServerPlayer : public Player {

public :
	
	// constructor
	RequestServerPlayer ( Socket * pSocket ) throw ( ProtocolException , Error );
	
	// destructor
	~RequestServerPlayer () throw ( ProtocolException , Error );

public :

	// read socket's receive buffer and fill input buffer
	//virtual void processInput () throw ( IOException , Error );
	
	// parse packet and execute handler for the packet
	virtual void processCommand () throw ( IOException , Error );
	
	// flush output buffer to socket's send buffer
	//virtual void processOutput () throw ( IOException , Error );
	
	// send packet to player's output buffer
	//virtual void sendPacket ( Packet * packet ) throw ( ProtocolException , Error );


	uint	send(const char* pBuffer, uint nBytes) throw( ProtocolException , Error );
	

	// disconnect
	
	virtual void disconnect ( bool bDisconnected = DISCONNECTED ) throw ( ProtocolException , Error );
	
	void				setName(const char* pName)	{ m_Name = pName; }
	const std::string&	getName() const	{ return m_Name; }

	// get debug string
	virtual std::string toString () const throw ( ProtocolException , Error );
	
public :

	// get player's status
	PlayerStatus getPlayerStatus () const throw () { return m_PlayerStatus; }

	// set player's status
	void setPlayerStatus ( PlayerStatus playerStatus ) throw () { m_PlayerStatus = playerStatus; }

	void		setRequestMode(REQUEST_CLIENT_MODE mode)	{ m_RequestMode = mode; }
	REQUEST_CLIENT_MODE			getRequestMode() const	{ return m_RequestMode; }

private :

	DWORD		m_ExpireTime;
	std::string		m_Name;

	
	REQUEST_CLIENT_MODE			m_RequestMode;

	// player status
	PlayerStatus m_PlayerStatus;

};

#endif
