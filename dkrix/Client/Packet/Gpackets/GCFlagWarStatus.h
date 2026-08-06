//////////////////////////////////////////////////////////////////////////////
// Filename    : GCFlagWarStatus.h 
// Written By  : elca@ewestsoft.com
// Description : 

//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_FLAG_WAR_STATUS_H__
#define __GC_FLAG_WAR_STATUS_H__

#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

#define RACE_MAX		3

//////////////////////////////////////////////////////////////////////////////
// class GCFlagWarStatus;

//////////////////////////////////////////////////////////////////////////////

class GCFlagWarStatus : public Packet 
{
public:
	GCFlagWarStatus() throw();
	~GCFlagWarStatus() throw();
	
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_FLAG_WAR_STATUS; }
	PacketSize_t getPacketSize() const throw() { return szWORD + szBYTE*RACE_MAX; }

#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const throw() { return "GCFlagWarStatus"; }
	std::string toString() const throw();
#endif

public:
	WORD	getTimeRemain() const { return m_TimeRemain; }
	void	setTimeRemain( WORD remainTime ) { m_TimeRemain = remainTime; }

	BYTE	getFlagCount( Race_t race ) const { return m_FlagCount[(int)race]; }
	void	setFlagCount( Race_t race, BYTE count ) { m_FlagCount[(int)race] = count; }

private :
	WORD	m_TimeRemain;
	BYTE	m_FlagCount[RACE_MAX];
};


//////////////////////////////////////////////////////////////////////////////
// class GCFlagWarStatusFactory;
//////////////////////////////////////////////////////////////////////////////

class GCFlagWarStatusFactory : public PacketFactory 
{
public :
	GCFlagWarStatusFactory() throw() {}
	virtual ~GCFlagWarStatusFactory() throw() {}
	
public:
	Packet* createPacket() throw() { return new GCFlagWarStatus(); }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const throw() { return "GCFlagWarStatus"; }
#endif
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_FLAG_WAR_STATUS; }
	PacketSize_t getPacketMaxSize() const throw() { return szWORD + szBYTE * RACE_MAX; }
};

//////////////////////////////////////////////////////////////////////////////
// class GCFlagWarStatusHandler;
//////////////////////////////////////////////////////////////////////////////

class GCFlagWarStatusHandler 
{
public:
	static void execute(GCFlagWarStatus* pGCFlagWarStatus, Player* pPlayer) throw ( ProtocolException , Error );

};

#endif
