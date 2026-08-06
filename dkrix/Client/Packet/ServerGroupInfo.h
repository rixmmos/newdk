//////////////////////////////////////////////////////////////////////
// 
// Filename    : ServerGroupInfo.h 
// Written By  : elca@ewestsoft.com

// 
//////////////////////////////////////////////////////////////////////

#ifndef __SERVER_GROUP_INFO_H__
#define __SERVER_GROUP_INFO_H__

// include files
#include "Packet/Types.h"
#include "Exception.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class ServerGroupInfo;
//

//
//////////////////////////////////////////////////////////////////////

class ServerGroupInfo {

public :
	
	// constructor
	ServerGroupInfo () throw ();
	
	// destructor
	~ServerGroupInfo () throw ();

public :
	
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// get packet's body size
	
	PacketSize_t getSize () throw ();

	static uint getMaxSize() throw() {
		return ( szBYTE + szBYTE + 20 + szBYTE ) * 37;
	}

	// get packet's debug std::string
	std::string toString () const throw ();

	// get / set GroupID
	BYTE getGroupID() const throw() { return m_GroupID; }
	void setGroupID( ServerGroupID_t GroupID ) throw() { m_GroupID = GroupID; }

	// get / set GroupName
	std::string getGroupName() const throw() { return m_GroupName; }
	void setGroupName( std::string GroupName ) throw() { m_GroupName = GroupName; }

	// get / set Group Stat
	BYTE getStat() const throw() { return m_Stat; }
	void setStat( BYTE Stat ) throw() { m_Stat = Stat; }

private :

	
	ServerGroupID_t m_GroupID;

	
	std::string m_GroupName;

	
	BYTE	m_Stat;

};

#endif
