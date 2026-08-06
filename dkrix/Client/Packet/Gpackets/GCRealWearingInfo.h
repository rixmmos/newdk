//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCRealWearingInfo.h 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_REAL_WEARING_INFO_H__
#define __GC_REAL_WEARING_INFO_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCRealWearingInfo;
//
//////////////////////////////////////////////////////////////////////

class GCRealWearingInfo : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_REAL_WEARING_INFO; }
	
	// get packet's body size
	PacketSize_t getPacketSize () const throw () { return szDWORD; }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCRealWearingInfo"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif
	
public :

	DWORD getInfo(void) const throw() { return m_Info;}
	void setInfo(DWORD info) throw() { m_Info = info;}

private :
	
	DWORD m_Info;

};


//////////////////////////////////////////////////////////////////////
//
// class GCRealWearingInfoFactory;
//
// Factory for GCRealWearingInfo
//
//////////////////////////////////////////////////////////////////////

class GCRealWearingInfoFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCRealWearingInfo(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCRealWearingInfo"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_REAL_WEARING_INFO; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szDWORD; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCRealWearingInfoHandler;
//
//////////////////////////////////////////////////////////////////////

class GCRealWearingInfoHandler {
	
public :

	// execute packet's handler
	static void execute ( GCRealWearingInfo * pPacket , Player * player ) throw ( ProtocolException , Error );
};

#endif
