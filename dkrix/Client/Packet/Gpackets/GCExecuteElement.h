//////////////////////////////////////////////////////////////////////////////
// Filename    : GCExecuteElement.h 
// Written By  : elca@ewestsoft.com
// Description : 

//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_EXECUTE_ELEMENT_H__
#define __GC_EXECUTE_ELEMENT_H__

#include "../Types.h"
#include "../Exception.h"
#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class GCExecuteElement;

//////////////////////////////////////////////////////////////////////////////

class GCExecuteElement : public Packet 
{
public:
	GCExecuteElement() throw();
	~GCExecuteElement() throw();
	
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error) { iStream.read(m_dwQuestID); iStream.read(m_Condition); iStream.read(m_Index); }
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error) { oStream.write(m_dwQuestID); oStream.write(m_Condition); oStream.write(m_Index); }
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_EXECUTE_ELEMENT; }
	PacketSize_t getPacketSize() const throw() { return szDWORD + szBYTE + szWORD; }
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCExecuteElement"; }
	string toString() const throw();
#endif
public:
	BYTE	getCondition() const { return m_Condition; }
	void	setCondition(BYTE cond) { m_Condition = cond; }

	WORD	getQuestID() const { return m_dwQuestID; }

	WORD	getIndex() const { return m_Index; }
	void	setIndex(WORD idx) { m_Index = idx; }

private:
	DWORD	m_dwQuestID;	
	BYTE	m_Condition;	
	WORD	m_Index;		
};


//////////////////////////////////////////////////////////////////////////////
// class GCExecuteElementFactory;
//////////////////////////////////////////////////////////////////////////////

class GCExecuteElementFactory : public PacketFactory 
{
public :
	GCExecuteElementFactory() throw() {}
	virtual ~GCExecuteElementFactory() throw() {}
	
public:
	Packet* createPacket() throw() { return new GCExecuteElement(); }
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCExecuteElement"; }
#endif
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_EXECUTE_ELEMENT; }
	PacketSize_t getPacketMaxSize() const throw() { return szDWORD + szBYTE + szWORD; }
};

//////////////////////////////////////////////////////////////////////////////
// class GCExecuteElementHandler;
//////////////////////////////////////////////////////////////////////////////

class GCExecuteElementHandler 
{
public:
	static void execute(GCExecuteElement* pGCExecuteElement, Player* pPlayer) throw ( ProtocolException , Error );

};

#endif
