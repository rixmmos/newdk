//----------------------------------------------------------------------
// 
// Filename    : CLDeletePC.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//----------------------------------------------------------------------

#ifndef __CL_DELETE_PC_H__
#define __CL_DELETE_PC_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//----------------------------------------------------------------------
//
// class CLDeletePC;
//

//
//----------------------------------------------------------------------

class CLDeletePC : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CL_DELETE_PC; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return szBYTE + m_Name.size() + szSlot + szBYTE + m_SSN.size(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "CLDeletePC"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	// get/set name
	std::string getName () const throw () { return m_Name; }
	void setName ( std::string name ) throw () { m_Name = name; }

	// get/set Slot
	Slot getSlot () const throw () { return m_Slot; }
	void setSlot ( Slot slot ) throw () { m_Slot = slot; }

	// get/set SSN
	std::string getSSN() const throw() { return m_SSN; }
	void setSSN( const std::string & SSN ) throw() { m_SSN = SSN; }

private :
	
	// PC name
	std::string m_Name;

	// Slot
	Slot m_Slot;

	
	std::string m_SSN;

};


//////////////////////////////////////////////////////////////////////
//
// class CLDeletePCFactory;
//
// Factory for CLDeletePC
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CLDeletePCFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CLDeletePC(); }

	// get packet name
	std::string getPacketName () const throw () { return "CLDeletePC"; }
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CL_DELETE_PC; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize () const throw () { return szBYTE + 20 + szSlot + szBYTE + 20; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CLDeletePCHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CLDeletePCHandler {

	public :

		// execute packet's handler
		static void execute ( CLDeletePC * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

	};
#endif

#endif
