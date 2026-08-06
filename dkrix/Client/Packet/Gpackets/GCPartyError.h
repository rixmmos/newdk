//////////////////////////////////////////////////////////////////////////////
// Filename    : GCPartyError.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_PARTY_ERROR_H__
#define __GC_PARTY_ERROR_H__

#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
enum
{
	
	GC_PARTY_ERROR_TARGET_NOT_EXIST = 0,

	
	GC_PARTY_ERROR_RACE_DIFFER,

	
	GC_PARTY_ERROR_NOT_SAFE,

	
	GC_PARTY_ERROR_NOT_NORMAL_FORM,

	
	GC_PARTY_ERROR_ALREADY_INVITING,

	
	GC_PARTY_ERROR_NOT_INVITING,

	
	GC_PARTY_ERROR_NO_AUTHORITY,

	
	GC_PARTY_ERROR_UNKNOWN,

	GC_PARTY_ERROR_MAX
};

//////////////////////////////////////////////////////////////////////////////
// class GCPartyError;
//////////////////////////////////////////////////////////////////////////////

class GCPartyError : public Packet 
{
public:
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_PARTY_ERROR; }
	PacketSize_t getPacketSize() const throw() { return szBYTE + szObjectID; }
	std::string getPacketName() const throw() { return "GCPartyError"; }
	std::string toString() const throw();

public:
	BYTE getCode() const throw() { return m_Code; }
	void setCode(BYTE code) { m_Code = code; }

	ObjectID_t getTargetObjectID(void) const { return m_TargetObjectID; }
	void setTargetObjectID(ObjectID_t id) { m_TargetObjectID = id; }

private :
	ObjectID_t m_TargetObjectID;
	BYTE       m_Code; 
};


//////////////////////////////////////////////////////////////////////////////
// class GCPartyErrorFactory;
//////////////////////////////////////////////////////////////////////////////

class GCPartyErrorFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new GCPartyError(); }
	std::string getPacketName() const throw() { return "GCPartyError"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_PARTY_ERROR; }
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + szObjectID; }
};


//////////////////////////////////////////////////////////////////////////////
// class GCPartyErrorHandler;
//////////////////////////////////////////////////////////////////////////////

class GCPartyErrorHandler 
{
public:
	static void execute(GCPartyError* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
