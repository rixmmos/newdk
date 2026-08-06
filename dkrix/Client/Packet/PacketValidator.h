//----------------------------------------------------------------------
//
// Filename    : PacketValidator
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __PACKET_VALIDATOR_H__
#define __PACKET_VALIDATOR_H__

// include files
#include "PacketIDSet.h"
#include "PlayerStatus.h"
#include <vector>

//----------------------------------------------------------------------
//
// class PacketValidator;
//




//
//----------------------------------------------------------------------

class PacketValidator {

public :

	// constructor
	PacketValidator () throw ();

	// destructor
	~PacketValidator () throw ();

	// init
	void init () throw ( ProtocolException , Error );

	// add packet id set
	void addPacketIDSet ( PlayerStatus playerStatus , PacketIDSet * pPacketID ) throw ( DuplicatedException , Error );

	// delete packet id
	void deletePacketIDSet ( PlayerStatus playerStatus , PacketIDSet * pPacketID ) throw ( NoSuchElementException , Error );

	// is valid packet?
	bool isValidPacketID ( PlayerStatus playerStatus , PacketID_t packetID ) throw ( NoSuchElementException , IgnorePacketException , Error );

	#ifdef __DEBUG_OUTPUT__
		// get debug string
		std::string toString () const throw ();
	#endif

private :

	std::vector< PacketIDSet * > m_PacketIDSets;

};

//----------------------------------------------------------------------
// global variable declaration
//----------------------------------------------------------------------
extern PacketValidator * g_pPacketValidator;

#endif
