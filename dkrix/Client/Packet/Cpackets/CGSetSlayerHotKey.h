//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGSetSlayerHotKey.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SET_SLAYER_HOT_KEY_H__
#define __CG_SET_SLAYER_HOT_KEY_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class CGSetSlayerHotKey;
//
//////////////////////////////////////////////////////////////////////

class CGSetSlayerHotKey : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_SET_SLAYER_HOT_KEY; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szSkillType * 4; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGSetSlayerHotKey"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	
public :

	// get/set X Coordicate
	SkillType_t getHotKey ( BYTE pos ) const throw () { return m_HotKey[pos]; }
	void setHotKey ( BYTE pos , SkillType_t SkillType ) throw () { m_HotKey[pos] = SkillType; }

private :

	SkillType_t m_HotKey[4];
	
};


//////////////////////////////////////////////////////////////////////
//
// class CGSetSlayerHotKeyFactory;
//
// Factory for CGSetSlayerHotKey
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGSetSlayerHotKeyFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new CGSetSlayerHotKey(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CGSetSlayerHotKey"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CG_SET_SLAYER_HOT_KEY; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szSkillType * 4; }

};

#endif


//////////////////////////////////////////////////////////////////////
//
// class CGSetSlayerHotKeyHandler;
//
//////////////////////////////////////////////////////////////////////
#ifndef __GAME_CLIENT__
	class CGSetSlayerHotKeyHandler {
		
	public :

		// execute packet's handler
		static void execute ( CGSetSlayerHotKey * pPacket , Player * player ) throw ( ProtocolException , Error );
	};
#endif

#endif
