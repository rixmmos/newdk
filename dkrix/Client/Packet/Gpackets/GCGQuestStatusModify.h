//////////////////////////////////////////////////////////////////////////////
// Filename    : GCGQuestStatusModify.h 
// Written By  : elca@ewestsoft.com
// Description : 

//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_GQUEST_STATUS_MODIFY_H__
#define __GC_GQUEST_STATUS_MODIFY_H__

#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

#include "../QuestStatusInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class GCGQuestStatusModify;

//////////////////////////////////////////////////////////////////////////////

class GCGQuestStatusModify : public Packet 
{
public:
	enum ModifyType
	{
		NO_MODIFY,		
		CURRENT,		
		SUCCESS,		
		FAIL,			
	};

	GCGQuestStatusModify() throw();
	~GCGQuestStatusModify() throw();
	
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error) { iStream.read(m_Type); m_pInfo = new QuestStatusInfo(); m_pInfo->read(iStream); }
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error) { oStream.write(m_Type); m_pInfo->write(oStream); }
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_GQUEST_STATUS_MODIFY; }
	PacketSize_t getPacketSize() const throw() { return szBYTE + m_pInfo->getSize(); }
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCGQuestStatusModify"; }
	string toString() const throw();
#endif
public:
	BYTE	getType() const { return m_Type; }
	void	setType(BYTE type) { m_Type = type; }

	QuestStatusInfo* getInfo() const { return m_pInfo; }
	void setInfo(QuestStatusInfo* pInfo) { m_pInfo = pInfo; }

private:
	BYTE				m_Type;
	QuestStatusInfo*	m_pInfo;
};


//////////////////////////////////////////////////////////////////////////////
// class GCGQuestStatusModifyFactory;
//////////////////////////////////////////////////////////////////////////////

class GCGQuestStatusModifyFactory : public PacketFactory 
{
public :
	GCGQuestStatusModifyFactory() throw() {}
	virtual ~GCGQuestStatusModifyFactory() throw() {}
	
public:
	Packet* createPacket() throw() { return new GCGQuestStatusModify(); }
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCGQuestStatusModify"; }
#endif
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_GQUEST_STATUS_MODIFY; }
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + QuestStatusInfo::getMaxSize(); }
};

//////////////////////////////////////////////////////////////////////////////
// class GCGQuestStatusModifyHandler;
//////////////////////////////////////////////////////////////////////////////

class GCGQuestStatusModifyHandler 
{
public:
	static void execute(GCGQuestStatusModify* pGCGQuestStatusModify, Player* pPlayer) throw ( ProtocolException , Error );

};

#endif
