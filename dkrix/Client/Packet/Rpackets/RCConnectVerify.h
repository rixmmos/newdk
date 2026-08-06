//////////////////////////////////////////////////////////////////////
// 
// Filename    : RCConnectVerify.h 
// Written By  : crazydog

// 
//////////////////////////////////////////////////////////////////////

#ifndef __RC_CONNECT_VERIFY_H__
#define __RC_CONNECT_VERIFY_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class RCConnectVerify;
//


//
//////////////////////////////////////////////////////////////////////
enum RC_CONNECT_VERIFY_CODE
{
	RC_CONNECT_NULL,			

	RC_CONNECT_OK,				
	RC_CONNECT_REJECT,			
	RC_CONNECT_WRONG,			
};

class RCConnectVerify : public Packet
{

public :
	
	// constructor
	RCConnectVerify () throw ();
	
	// destructor
	~RCConnectVerify () throw ();
	
public :
    PacketID_t getPacketID () const throw () { return PACKET_RC_CONNECT_VERIFY; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "RCConnectVerify"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif


    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szBYTE; }
	static PacketSize_t getPacketMaxSize() throw() { return szBYTE;}

	// get / set ListNumber
	RC_CONNECT_VERIFY_CODE	getCode() const throw() { return m_Code; }
	void					setCode( RC_CONNECT_VERIFY_CODE code ) throw() { m_Code = code; }

protected :
	
	RC_CONNECT_VERIFY_CODE		m_Code;

};

//////////////////////////////////////////////////////////////////////
//
// class RCConnectVerifyFactory;
//
// Factory for RCConnectVerify
//
//////////////////////////////////////////////////////////////////////

class RCConnectVerifyFactory : public PacketFactory {

public :
	
	// constructor
	RCConnectVerifyFactory () throw () {}
	
	// destructor
	virtual ~RCConnectVerifyFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new RCConnectVerify(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "RCConnectVerify"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_RC_CONNECT_VERIFY; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return 255;}

};


//////////////////////////////////////////////////////////////////////
//
// class RCConnectVerifyHandler;
//
//////////////////////////////////////////////////////////////////////

class RCConnectVerifyHandler {

public :

	// execute packet's handler
	static void execute ( RCConnectVerify * pRCConnectVerify , Player * pPlayer ) throw ( ProtocolException , Error );

};


#endif
