//////////////////////////////////////////////////////////////////////
// 
// Filename    : LCRegisterPlayerOK.h 
// Written By  : Reiot
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __LC_REGISTER_PLAYER_OK_H__
#define __LC_REGISTER_PLAYER_OK_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCRegisterPlayerOK;
//

//
//////////////////////////////////////////////////////////////////////

class LCRegisterPlayerOK : public Packet {

public :

    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_LC_REGISTER_PLAYER_OK; }
	
	// get packet body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szBYTE + m_GroupName.size() + szBYTE; }
	
	
    // get / set Groupname
	std::string getGroupName() const throw() { return m_GroupName; }
	void setGroupName( const std::string & GroupName ) throw() { m_GroupName = GroupName; }
	
	// get / set GoreLevel
	bool isAdult() const throw() { return m_isAdult; }
	void setAdult( bool isAdult ) throw() { m_isAdult = isAdult; }
	
	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "LCRegisterPlayerOK"; }

		// get packet's debug std::string
		std::string toString () const throw () { return "LCRegisterPlayerOK"; }
	#endif

private :
	
	std::string m_GroupName;
	
	
	
	
	bool m_isAdult;
};


//////////////////////////////////////////////////////////////////////
//
// class LCRegisterPlayerOKFactory;
//
// Factory for LCRegisterPlayerOK
//
//////////////////////////////////////////////////////////////////////

class LCRegisterPlayerOKFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new LCRegisterPlayerOK(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "LCRegisterPlayerOK"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_LC_REGISTER_PLAYER_OK; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () { return szBYTE + 20 + szBYTE; }
	
};


//////////////////////////////////////////////////////////////////////
//
// class LCRegisterPlayerOKHandler;
//
//////////////////////////////////////////////////////////////////////

class LCRegisterPlayerOKHandler {

public :

	// execute packet's handler
	static void execute ( LCRegisterPlayerOK * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
