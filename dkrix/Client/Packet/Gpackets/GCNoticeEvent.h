//////////////////////////////////////////////////////////////////////////////
// Filename    : GCNoticeEvent.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_NOTICE_EVENT_H__
#define __GC_NOTICE_EVENT_H__

#include "../Packet.h"
#include "../PacketFactory.h"

#ifndef __GAME_CLIENT__
	#include "../Utility.h"
#endif

enum 
{
	NOTICE_EVENT_KICK_OUT_FROM_ZONE,			
	NOTICE_EVENT_CONTINUAL_GROUND_ATTACK,		
 	NOTICE_EVENT_CONTINUAL_GROUND_ATTACK_END,	
	NOTICE_EVENT_MASTER_COMBAT_TIME,			
	NOTICE_EVENT_MASTER_COMBAT_END,				
	NOTICE_EVENT_METEOR_STRIKE,					
	NOTICE_EVENT_METEOR_STRIKE_END,				
	NOTICE_EVENT_PREMIUM_HALF_START,			
	NOTICE_EVENT_PREMIUM_HALF_END,				
	NOTICE_EVENT_SHOP_TAX_CHANGE,				

	
	NOTICE_EVENT_WAR_OVER,						

    NOTICE_EVENT_RACE_WAR_OVER,                 
	NOTICE_EVENT_WELCOME_MESSAGE,				
	NOTICE_EVENT_MEET_GRANDMA,					
    NOTICE_EVENT_MEET_FISHSHOP_MASTER,			

    NOTICE_EVENT_START_QUEST_ENDING,			
	NOTICE_EVENT_RESULT_LOTTERY,				

	NOTICE_EVENT_RUN_HORN,						
	NOTICE_EVENT_MASTER_LAIR_OPEN,              
    NOTICE_EVENT_MASTER_LAIR_CLOSED,            
    NOTICE_EVENT_MASTER_LAIR_COUNT,             
    NOTICE_EVENT_CONTRACT_GNOMES_HORN,          
    NOTICE_EVENT_NOT_ENOUGH_MONEY,              
    NOTICE_EVENT_MINI_GAME,				        
	NOTICE_EVENT_GET_RIFINIUM,					
	NOTICE_EVENT_INVENTORY_FULL,				
	
    NOTICE_EVENT_FLAG_WAR_READY,                
    NOTICE_EVENT_FLAG_WAR_START,                
    NOTICE_EVENT_FLAG_WAR_FINISH,               
	NOTICE_EVENT_FLAG_POURED_ITEM,				
	NOTICE_EVENT_ENTER_BEGINNER_ZONE,			
	NOTICE_EVENT_LOGIN_JUST_NOW,                

	NOTICE_EVENT_LEVEL_WAR_ARRANGED,            
	NOTICE_EVENT_LEVEL_WAR_STARTED,             
    NOTICE_EVENT_RACE_WAR_SOON,                 
	
	NOTICE_EVENT_LEVEL_WAR_OVER,				
    NOTICE_EVENT_NETMARBLE_CARD_FULL,           
    NOTICE_EVENT_HOLYDAY,                       
	
	NOTICE_EVENT_CAN_PET_QUEST,					 
	NOTICE_EVENT_SEND_SMS, 
	
	NOTICE_EVENT_GDR_LAIR_ENDING_1,             
	NOTICE_EVENT_GOLD_MEDALS,                   

	NOTICE_EVENT_CROWN_PRICE,                   

	NOTICE_EVENT_GIVE_PRESENT_1,                
    NOTICE_EVENT_GIVE_PRESENT_2,                
    NOTICE_EVENT_GIVE_PRESENT_3,                
    NOTICE_EVENT_GIVE_PRESENT_4,                
    NOTICE_EVENT_GIVE_PRESENT_5,                
    NOTICE_EVENT_GIVE_PRESENT_6,                
    NOTICE_EVENT_GIVE_PRESENT_7,                
    NOTICE_EVENT_GIVE_PRESENT_8,                
    NOTICE_EVENT_CAN_OPEN_PRESENT_8,            

    NOTICE_EVENT_RACE_WAR_IN_20,                
    NOTICE_EVENT_RACE_WAR_IN_5,                 

	NOTICE_EVENT_RACE_WAR_STARTED_IN_OTHER_SERVER, 

	NOTICE_EVENT_CANNOT_FIND_STORE,             
    NOTICE_EVENT_STORE_CLOSED,                  
    NOTICE_EVENT_ITEM_NOT_FOUND,                
    NOTICE_EVENT_TOO_MUCH_MONEY,                
	NOTICE_EVENT_NO_INVENTORY_SPACE,            
    NOTICE_EVENT_ALREADY_DISPLAYED,             
    NOTICE_EVENT_CANNOT_SELL,                   


	NOTICE_EVENT_MAX
};

//////////////////////////////////////////////////////////////////////////////
// class GCNoticeEvent
//////////////////////////////////////////////////////////////////////////////

class GCNoticeEvent : public Packet 
{

public:
	GCNoticeEvent() throw() { m_Code = NOTICE_EVENT_MAX; m_Parameter = 0;}
	virtual ~GCNoticeEvent() throw() {}

public:
	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_NOTICE_EVENT; }
	PacketSize_t getPacketSize() const throw();
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const throw() { return "GCNoticeEvent"; }
	std::string toString() const throw();
#endif
	
public:
	BYTE getCode(void) const throw() { return m_Code;}
	void setCode(WORD code) throw() { m_Code = code;}

	uint getParameter(void) const throw() { return m_Parameter; }
	void setParameter(uint parameter) throw() { m_Parameter = parameter; }

#ifndef __GAME_CLIENT__
	void setParameter(WORD hiWord, WORD loWord) throw() { m_Code = makeDWORD(hiWord, loWord); }
#endif

private: 
	WORD m_Code;
	uint m_Parameter;

};


//////////////////////////////////////////////////////////////////////////////
// class GCNoticeEventFactory;
//////////////////////////////////////////////////////////////////////////////

class GCNoticeEventFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new GCNoticeEvent(); }
	std::string getPacketName() const throw() { return "GCNoticeEvent"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_NOTICE_EVENT; }
	PacketSize_t getPacketMaxSize() const throw() { return szWORD + szuint; }
};


//////////////////////////////////////////////////////////////////////////////
// class GCNoticeEventHandler
//////////////////////////////////////////////////////////////////////////////

class GCNoticeEventHandler 
{
public:
	static void execute( GCNoticeEvent* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
