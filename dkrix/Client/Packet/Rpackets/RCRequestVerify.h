//////////////////////////////////////////////////////////////////////
// 
// Filename    : RCRequestVerify.h 
// Written By  : elca@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __RC_REQUEST_VERIFY_H__
#define __RC_REQUEST_VERIFY_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

enum REQUEST_VERIFY
{
	REQUEST_VERIFY_NULL,
	REQUEST_VERIFY_WHISPER_FAILED,
	REQUEST_VERIFY_PROFILE_NOT_EXIST,		
	REQUEST_VERIFY_PROFILE_WRONG_USER,		
	REQUEST_VERIFY_PROFILE_DONE,			
};

//////////////////////////////////////////////////////////////////////
//
// class RCRequestVerify;
//
//////////////////////////////////////////////////////////////////////

class RCRequestVerify : public Packet {

public :
	
	// constructor
	RCRequestVerify () throw ();
	
	// destructor
	~RCRequestVerify () throw ();

	
public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_RC_REQUEST_VERIFY; }
	
	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "RCRequestVerify"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

	REQUEST_VERIFY getCode() const throw () { return m_Code ; }
	void setCode ( REQUEST_VERIFY code ) throw () { m_Code = code ; }


private :
	REQUEST_VERIFY		m_Code;

};

//////////////////////////////////////////////////////////////////////
//
// class RCRequestVerifyFactory;
//
// Factory for RCRequestVerify
//
//////////////////////////////////////////////////////////////////////

class RCRequestVerifyFactory : public PacketFactory {

public :
	
	// constructor
	RCRequestVerifyFactory () throw () {}
	
	// destructor
	virtual ~RCRequestVerifyFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new RCRequestVerify(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "RCRequestVerify"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_RC_REQUEST_VERIFY; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE; }

};


//////////////////////////////////////////////////////////////////////
//
// class RCRequestVerifyHandler;
//
//////////////////////////////////////////////////////////////////////

class RCRequestVerifyHandler {

public :

	// execute packet's handler
	static void execute ( RCRequestVerify * pRCRequestVerify , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
