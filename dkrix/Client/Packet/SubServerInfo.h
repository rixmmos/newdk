//----------------------------------------------------------------------
//
// Filename    : SubServerInfo.h
// Written By  : elca
// Description :
//
//----------------------------------------------------------------------

#ifndef __SUB_SERVER_INFO_H__
#define __SUB_SERVER_INFO_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//----------------------------------------------------------------------
//

//


//
//----------------------------------------------------------------------

class SubServerInfo {

public :

	// read data from socket input stream
	void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );

	// write data to socket output stream
	void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// get size of object
	uint getSize () throw ()
	{ return szServerID + szBYTE + m_ServerName.size() + szBYTE; }

	// get max size of object
	static uint getMaxSize () throw ()
	{ return szServerID + szBYTE + 20 + szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		// get debug std::string
		std::string toString () const throw ();
	#endif

public :

	// get/set ServerID
	void setServerID( ServerID_t ServerID) throw() { m_ServerID = ServerID; }
	ServerID_t getServerID() const throw() { return m_ServerID; }

	// get/set ServerName
	void setServerName( std::string ServerName ) throw() { m_ServerName = ServerName; }
	std::string getServerName() const throw() { return m_ServerName; }

	// get/set ServerStat
	void setServerStat( BYTE ServerStat ) throw() { m_ServerStat = ServerStat; }
	BYTE getServerStat() const throw() { return m_ServerStat; }

private :

	
	ServerID_t m_ServerID; 

	
	std::string m_ServerName;

	
	BYTE m_ServerStat;

};

#endif
