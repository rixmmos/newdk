//////////////////////////////////////////////////////////////////////////////
// Filename    : GCPetStashVerify.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_PET_STASH_VERIFY_H__
#define __GC_PET_STASH_VERIFY_H__

#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class GCPetStashVerify
//////////////////////////////////////////////////////////////////////////////

class GCPetStashVerify : public Packet 
{
public:
	enum
	{
		PET_STASH_OK,
		PET_STASH_NOT_ENOUGH_MONEY,
		PET_STASH_NO_INVENTORY_SPACE,
		PET_STASH_RACK_IS_NOT_EMPTY,	
		PET_STASH_RACK_IS_EMPTY			
	};

	GCPetStashVerify() throw() { m_Code = PET_STASH_OK; }
	virtual ~GCPetStashVerify() throw() {}

public:
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_PET_STASH_VERIFY; }
	PacketSize_t getPacketSize() const throw() { return szBYTE; }
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCPetStashVerify"; }
	string toString() const throw();
#endif	
public:
	BYTE getCode(void) const throw() { return m_Code;}
	void setCode(BYTE code) throw() { m_Code = code;}

private: 
	BYTE m_Code;
};


//////////////////////////////////////////////////////////////////////////////
// class GCPetStashVerifyFactory;
//////////////////////////////////////////////////////////////////////////////

class GCPetStashVerifyFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new GCPetStashVerify(); }
	string getPacketName() const throw() { return "GCPetStashVerify"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_PET_STASH_VERIFY; }
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE; }
};


//////////////////////////////////////////////////////////////////////////////
// class GCPetStashVerifyHandler
//////////////////////////////////////////////////////////////////////////////

class GCPetStashVerifyHandler 
{
public:
	static void execute( GCPetStashVerify* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
