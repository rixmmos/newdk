//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCWhisperFailed.h 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_WHISPER_FAILED_H__
#define __GC_WHISPER_FAILED_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCWhisperFailed;
//


//
//////////////////////////////////////////////////////////////////////

class GCWhisperFailed : public Packet {

public :
	
	// constructor
	GCWhisperFailed () throw ();
	
	// destructor
	~GCWhisperFailed () throw ();

	
public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_WHISPER_FAILED; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return 0; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCWhisperFailed"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

private :

};

//////////////////////////////////////////////////////////////////////
//
// class GCWhisperFailedFactory;
//
// Factory for GCWhisperFailed
//
//////////////////////////////////////////////////////////////////////

class GCWhisperFailedFactory : public PacketFactory {

public :
	
	// constructor
	GCWhisperFailedFactory () throw () {}
	
	// destructor
	virtual ~GCWhisperFailedFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new GCWhisperFailed(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCWhisperFailed"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_WHISPER_FAILED; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return 0; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCWhisperFailedHandler;
//
//////////////////////////////////////////////////////////////////////

class GCWhisperFailedHandler {

public :

	// execute packet's handler
	static void execute ( GCWhisperFailed * pGCWhisperFailed , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
