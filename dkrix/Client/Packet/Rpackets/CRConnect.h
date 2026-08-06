//////////////////////////////////////////////////////////////////////
// 
// Filename    : CRConnect.h 
// Written By  : crazydog

// 
//////////////////////////////////////////////////////////////////////

#ifndef __CR_CONNECT_H__
#define __CR_CONNECT_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CRConnect;
//

//

//
//////////////////////////////////////////////////////////////////////

class CRConnect : public Packet
{

public :
	
	// constructor
	CRConnect () throw ();
	
	// destructor
	~CRConnect () throw ();
	
public :
    PacketID_t getPacketID () const throw () { return PACKET_CR_CONNECT; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const throw () { return "CRConnect"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif


    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );
	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet's body size
	
	PacketSize_t getPacketSize () const throw () { return szBYTE + m_RequestServerName.size() + szBYTE + m_RequestClientName.size(); }
	static PacketSize_t getPacketMaxSize() throw() { return  szBYTE + 10 + szBYTE + 10;}

	// get / set ListNumber
	const std::string& getRequestServerName() const throw() { return m_RequestServerName; }
	void setRequestServerName(const char* pName) throw() { m_RequestServerName = pName; }

	const std::string& getRequestClientName() const throw() { return m_RequestClientName; }
	void setRequestClientName(const char* pName) throw() { m_RequestClientName = pName; }
	

protected :
	
	std::string		m_RequestServerName;
	std::string		m_RequestClientName;
};

//////////////////////////////////////////////////////////////////////
//
// class CRConnectFactory;
//
// Factory for CRConnect
//
//////////////////////////////////////////////////////////////////////

class CRConnectFactory : public PacketFactory {

public :
	
	// constructor
	CRConnectFactory () throw () {}
	
	// destructor
	virtual ~CRConnectFactory () throw () {}

	
public :
	
	// create packet
	Packet * createPacket () throw () { return new CRConnect(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "CRConnect"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_CR_CONNECT; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize () const throw () { return  szBYTE + 10 + szBYTE + 10; }

};


//////////////////////////////////////////////////////////////////////
//
// class CRConnectHandler;
//
//////////////////////////////////////////////////////////////////////

class CRConnectHandler {

public :

	// execute packet's handler
	static void execute ( CRConnect * pCRConnect , Player * pPlayer ) throw ( ProtocolException , Error );

};


#endif
