//////////////////////////////////////////////////////////////////////
// 
// Filename    : CGRequestStoreInfo.h 
// Written By  :
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __CG_REQUEST_STORE_INFO_H__
#define __CG_REQUEST_STORE_INFO_H__

// include files
#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGRequestStoreInfo;
//
//////////////////////////////////////////////////////////////////////

class CGRequestStoreInfo : public Packet
{
public:
	
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_REQUEST_STORE_INFO; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szObjectID; }

	// get packet name
	string getPacketName() const throw() { return "CGRequestStoreInfo"; }

	// get packet's debug string
	string toString() const throw();

	ObjectID_t	getOwnerObjectID() const { return m_OwnerObjectID; }
	void		setOwnerObjectID(ObjectID_t oid) { m_OwnerObjectID = oid; }

private:
	ObjectID_t	m_OwnerObjectID;	
};


//////////////////////////////////////////////////////////////////////
//
// class CGRequestStoreInfoFactory;
//
// Factory for CGRequestStoreInfo
//
//////////////////////////////////////////////////////////////////////

class CGRequestStoreInfoFactory : public PacketFactory {

public:
	
	// constructor
	CGRequestStoreInfoFactory() throw() {}
	
	// destructor
	virtual ~CGRequestStoreInfoFactory() throw() {}

	
public:
	
	// create packet
	Packet* createPacket() throw() { return new CGRequestStoreInfo(); }

	// get packet name
	string getPacketName() const throw() { return "CGRequestStoreInfo"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_REQUEST_STORE_INFO; }

	// get Packet Max Size
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID; }
};


//////////////////////////////////////////////////////////////////////
//
// class CGRequestStoreInfoHandler;
//
//////////////////////////////////////////////////////////////////////

class CGRequestStoreInfoHandler {

public:

	// execute packet's handler
	static void execute(CGRequestStoreInfo* pCGRequestStoreInfo, Player* pPlayer) throw ( ProtocolException , Error );

};

#endif
