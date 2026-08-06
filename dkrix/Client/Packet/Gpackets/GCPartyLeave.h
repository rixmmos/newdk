//////////////////////////////////////////////////////////////////////////////
// Filename    : GCPartyLeave.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_PARTY_LEAVE_H__
#define __GC_PARTY_LEAVE_H__

#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class GCPartyLeave;
//////////////////////////////////////////////////////////////////////////////

class GCPartyLeave : public Packet 
{
public:
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_PARTY_LEAVE; }
	PacketSize_t getPacketSize() const throw() 
	{ 
		return szBYTE*2 + m_Expeller.size() + m_Expellee.size();
	}
	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName() const throw() { return "GCPartyLeave"; }
		std::string toString() const throw();
	#endif

public:
	std::string getExpeller(void) const { return m_Expeller; }
	void setExpeller(const std::string& name) { m_Expeller = name; }

	std::string getExpellee(void) const { return m_Expellee; }
	void setExpellee(const std::string& name) { m_Expellee = name; }

private:
	
	
	
	
	
	std::string m_Expeller; 
	std::string m_Expellee; 
};


//////////////////////////////////////////////////////////////////////////////
// class GCPartyLeaveFactory;
//////////////////////////////////////////////////////////////////////////////

class GCPartyLeaveFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new GCPartyLeave(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName() const throw() { return "GCPartyLeave"; }
	#endif
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_PARTY_LEAVE; }
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE*2 + 20; }
};


//////////////////////////////////////////////////////////////////////////////
// class GCPartyLeaveHandler;
//////////////////////////////////////////////////////////////////////////////

class GCPartyLeaveHandler 
{
public:
	static void execute(GCPartyLeave* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
