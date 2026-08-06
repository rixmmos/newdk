//--------------------------------------------------------------------------------
// 
// Filename    : CGConnectSetKey.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//--------------------------------------------------------------------------------

#ifndef __CL_CONNECTSETKEY_H__
#define __CL_CONNECTSETKEY_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CGConnectSetKey;
//


//
//--------------------------------------------------------------------------------



class CGConnectSetKey : public Packet {

public:
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_CG_ENCODE_KEY; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () 
	{ 
		return szWORD 						// authentication key
			+ szWORD; 						// Slayer or Vampire?
	}

	// get packet name
	string getPacketName() const throw() { return "CGConnectSetKey"; }
	
	// get packet's debug string
	string toString() const throw() { return "CGConnectSetKey"; }

public:

	// get/set key
	WORD getEncryptKey () const throw () { return m_EncryptKey; }

	WORD getHashKey () const throw () { return m_HashKey; }

	void setEncryptKey ( WORD key ) throw () { m_EncryptKey = key; }

	void setHashKey ( WORD key )	throw () { m_HashKey = key; }

private :

	WORD m_EncryptKey;

	WORD m_HashKey;	
};


//--------------------------------------------------------------------------------
//
// class CLLoginFactory;
//
// Factory for CLLogin
//
//--------------------------------------------------------------------------------

class CGConnectSetKeyFactory : public PacketFactory {

public:
	
	// create packet
	Packet * createPacket () throw () { return new CGConnectSetKey(); }

	// get packet name
	string getPacketName() const throw() { return "CGConnectSetKey"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_ENCODE_KEY; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const throw() { return szWORD + szWORD;  }

};


//--------------------------------------------------------------------------------
//
// class CLLoginHandler;
//
//--------------------------------------------------------------------------------

class CGConnectSetKeyHandler {

public:

	// execute packet's handler
	static void execute(CGConnectSetKey* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
