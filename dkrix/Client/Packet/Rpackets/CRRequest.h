//////////////////////////////////////////////////////////////////////
// 
// Filename    : CRRequest.h 
// Written By  : crazydog

// 
//////////////////////////////////////////////////////////////////////

#ifndef __CR_REQUEST_H__
#define __CR_REQUEST_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

enum CR_REQUEST_CODE
{
	CR_REQUEST_NULL,

	
	CR_REQUEST_FILE_GUILD_MARK,
	CR_REQUEST_FILE_PROFILE,
};

//////////////////////////////////////////////////////////////////////
//
// class CRRequest;
//


//
//////////////////////////////////////////////////////////////////////
class CRRequest : public Packet
{

public :
	
	// constructor
	CRRequest () throw ();
	
	// destructor
	~CRRequest () throw ();
	
public :
    PacketID_t getPacketID () const throw () { return PACKET_CR_REQUEST; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "CRRequest"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif


    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szBYTE + szBYTE + m_RequestName.size(); }
	static PacketSize_t getPacketMaxSize() throw() { return szBYTE + szBYTE + 20;}

	// get / set ListNumber
	CR_REQUEST_CODE getCode() const throw() { return m_Code; }
	void setCode( CR_REQUEST_CODE code) throw() { m_Code = code; }

	const std::string& getRequestName() const throw() { return m_RequestName; }
	void setRequestName(const char* pName) throw() { m_RequestName = pName; }

protected :
	
	CR_REQUEST_CODE		m_Code;

	std::string				m_RequestName;
};

//////////////////////////////////////////////////////////////////////
//
// class CRRequestFactory;
//
// Factory for CRRequest
//
//////////////////////////////////////////////////////////////////////

class CRRequestFactory : public PacketFactory {

public :
	
	// constructor
	CRRequestFactory () throw () {}
	
	// destructor
	virtual ~CRRequestFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new CRRequest(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CRRequest"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CR_REQUEST; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return szBYTE + szBYTE + 20;}

};


//////////////////////////////////////////////////////////////////////
//
// class CRRequestHandler;
//
//////////////////////////////////////////////////////////////////////

class CRRequestHandler {

public :

	// execute packet's handler
	static void execute ( CRRequest * pCRRequest , Player * pPlayer ) throw ( ProtocolException , Error );

};


#endif
