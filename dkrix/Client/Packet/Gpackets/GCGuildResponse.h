//////////////////////////////////////////////////////////////////////////////
// Filename    : GCGuildResponse.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_GUILD_RESPONSE_H__
#define __GC_GUILD_RESPONSE_H__

#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class GCGuildResponse
//////////////////////////////////////////////////////////////////////////////
enum UNION_ERROR
{
		OK = 0,
        ALREADY_IN_UNION,		
        ALREADY_OFFER_SOMETHING,
        TARGET_IS_NOT_MASTER,   
        NOT_IN_UNION,           
        MASTER_CANNOT_QUIT,     
        NO_TARGET_UNION,        
        NOT_YOUR_UNION,         
        SOURCE_IS_NOT_MASTER,   
		YOU_HAVE_PENALTY,        
		NOT_ENOUGH_SLOT,			
		TOO_MANY_MEMBER,		
};

class GCGuildResponse : public Packet 
{

public:
	GCGuildResponse() throw() { m_Code = 0; m_Parameter = 0;}
	virtual ~GCGuildResponse() throw() {}

public:
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_GUILD_RESPONSE; }
	PacketSize_t getPacketSize() const throw();
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCGuildResponse"; }
	string toString() const throw();
#endif	
public:
	BYTE getCode(void) const throw() { return m_Code;}
	void setCode(WORD code) throw() { m_Code = code;}

	uint getParameter(void) const throw() { return m_Parameter; }
	void setParameter(uint parameter) throw() { m_Parameter = parameter; }

private: 
	WORD m_Code;
	uint m_Parameter;

};


//////////////////////////////////////////////////////////////////////////////
// class GCGuildResponseFactory;
//////////////////////////////////////////////////////////////////////////////

class GCGuildResponseFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new GCGuildResponse(); }
	string getPacketName() const throw() { return "GCGuildResponse"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_GUILD_RESPONSE; }
	PacketSize_t getPacketMaxSize() const throw() { return szWORD + szuint; }
};


//////////////////////////////////////////////////////////////////////////////
// class GCGuildResponseHandler
//////////////////////////////////////////////////////////////////////////////

class GCGuildResponseHandler 
{
public:
	static void execute( GCGuildResponse* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
