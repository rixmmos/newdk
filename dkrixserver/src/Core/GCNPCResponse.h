//////////////////////////////////////////////////////////////////////////////
// Filename    : GCNPCResponse.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_NPC_RESPONSE_H__
#define __GC_NPC_RESPONSE_H__

#include "Packet.h"
#include "PacketFactory.h"

enum {
    
    NPC_RESPONSE_INTERFACE_REPAIR = 0,
    NPC_RESPONSE_INTERFACE_STASHOPEN,         // 1
    NPC_RESPONSE_INTERFACE_SILVER_COATING,    // 2
    NPC_RESPONSE_INTERFACE_CREATE_GUILD,      // 3
    NPC_RESPONSE_INTERFACE_DESTROY_GUILD,     // 4
    NPC_RESPONSE_INTERFACE_NEWBIE_ITEM,       // 5
    NPC_RESPONSE_INTERFACE_TUTORIAL_COMPUTER, // 6
    NPC_RESPONSE_INTERFACE_TUTORIAL_BRIEFING, // 7
    NPC_RESPONSE_INTERFACE_TUTORIAL_BOOKCASE, // 8
    NPC_RESPONSE_INTERFACE_HELICOPTER,        // 9

    // ...
    NPC_RESPONSE_QUIT_DIALOGUE, // 10
    NPC_RESPONSE_HEAL,          // 11

    
    NPC_RESPONSE_REPAIR_OK,                  // 12
    NPC_RESPONSE_REPAIR_FAIL_ITEM_NOT_EXIST, // 13
    NPC_RESPONSE_REPAIR_FAIL_ITEM_TYPE,      // 14
    NPC_RESPONSE_REPAIR_FAIL_MONEY,          // 15

    NPC_RESPONSE_STASH_SELL_OK,         // 16
    NPC_RESPONSE_STASH_SELL_FAIL_MAX,   // 17
    NPC_RESPONSE_STASH_SELL_FAIL_MONEY, // 18

    NPC_RESPONSE_SILVER_COATING_OK,                  // 19
    NPC_RESPONSE_SILVER_COATING_FAIL_ITEM_NOT_EXIST, // 20
    NPC_RESPONSE_SILVER_COATING_FAIL_ITEM_TYPE,      // 21
    NPC_RESPONSE_SILVER_COATING_FAIL_MONEY,          // 22

    NPC_RESPONSE_DONATION_OK,         // 23
    NPC_RESPONSE_DONATION_FAIL_MONEY, // 24

    
    
    NPC_RESPONSE_DECREASE_BALL, // 25

    
    NPC_RESPONSE_TEAM_REGIST_FAIL_ALREADY_JOIN, 
                                                // 26
    NPC_RESPONSE_TEAM_REGIST_FAIL_QUIT_TIMEOUT, 
                                                
    NPC_RESPONSE_TEAM_REGIST_FAIL_CANCEL_TIMEOUT, 
                                                  
                                                  // 28
    NPC_RESPONSE_TEAM_REGIST_FAIL_LEVEL, 
    NPC_RESPONSE_TEAM_REGIST_FAIL_MONEY, 
    NPC_RESPONSE_TEAM_REGIST_FAIL_FAME,  
                                         
    NPC_RESPONSE_TEAM_REGIST_FAIL_NAME,  
    NPC_RESPONSE_TEAM_REGIST_FAIL_DENY,  

    NPC_RESPONSE_TEAM_STARTING_FAIL_ALREADY_JOIN,   
    NPC_RESPONSE_TEAM_STARTING_FAIL_QUIT_TIMEOUT,   
                                                    
    NPC_RESPONSE_TEAM_STARTING_FAIL_CANCEL_TIMEOUT, 
                                                    
                                                    // // 36
    NPC_RESPONSE_TEAM_STARTING_FAIL_LEVEL, 
                                           // 37
    NPC_RESPONSE_TEAM_STARTING_FAIL_MONEY, 
    NPC_RESPONSE_TEAM_STARTING_FAIL_FAME,  
                                           
    NPC_RESPONSE_TEAM_STARTING_FAIL_DENY,  

    NPC_RESPONSE_CLAN_REGIST_FAIL_ALREADY_JOIN,   
                                                  
    NPC_RESPONSE_CLAN_REGIST_FAIL_QUIT_TIMEOUT,   
                                                  
    NPC_RESPONSE_CLAN_REGIST_FAIL_CANCEL_TIMEOUT, 
                                                  
    NPC_RESPONSE_CLAN_REGIST_FAIL_LEVEL, 
                                         // 44
    NPC_RESPONSE_CLAN_REGIST_FAIL_MONEY, 
    NPC_RESPONSE_CLAN_REGIST_FAIL_FAME,  
                                         
    NPC_RESPONSE_CLAN_REGIST_FAIL_NAME,  // 	// 47
    NPC_RESPONSE_CLAN_REGIST_FAIL_DENY,  

    NPC_RESPONSE_CLAN_STARTING_FAIL_ALREADY_JOIN,   
    NPC_RESPONSE_CLAN_STARTING_FAIL_QUIT_TIMEOUT,   
                                                    
    NPC_RESPONSE_CLAN_STARTING_FAIL_CANCEL_TIMEOUT, 
                                                    
    NPC_RESPONSE_CLAN_STARTING_FAIL_LEVEL, 
                                           
    NPC_RESPONSE_CLAN_STARTING_FAIL_MONEY, 
                                           
    NPC_RESPONSE_CLAN_STARTING_FAIL_FAME,  
                                           
    NPC_RESPONSE_CLAN_STARTING_FAIL_DENY,  

    NPC_RESPONSE_GUILD_SHOW_REGIST,        
    NPC_RESPONSE_GUILD_SHOW_STARTING_JOIN, 
    NPC_RESPONSE_GUILD_SHOW_JOIN,          
    NPC_RESPONSE_GUILD_SHOW_QUIT,          
    NPC_RESPONSE_GUILD_ERROR,              

    NPC_RESPONSE_TRADE_GIFT_BOX_OK,            
    NPC_RESPONSE_TRADE_GIFT_BOX_NO_ITEM,       
    NPC_RESPONSE_TRADE_GIFT_BOX_ALREADY_TRADE, 
    NPC_RESPONSE_TRADE_GIFT_BOX_ERROR,         

    NPC_RESPONSE_REWARD_OK,     
    NPC_RESPONSE_REWARD_FAIL,   
    NPC_RESPONSE_NO_EMPTY_SLOT, 

    NPC_RESPONSE_SHOW_TAX_BALANCE,  
    NPC_RESPONSE_WITHDRAW_TAX_OK,   
    NPC_RESPONSE_WITHDRAW_TAX_FAIL, 

    NPC_RESPONSE_NO_GUILD,         
    NPC_RESPONSE_NOT_GUILD_MASTER, 
    NPC_RESPONSE_HAS_NO_CASTLE,    
    NPC_RESPONSE_NOT_YOUR_CASTLE,  

    
    NPC_RESPONSE_NOT_ENOUGH_MONEY,       
    NPC_RESPONSE_WAR_SCHEDULE_FULL,      
    NPC_RESPONSE_WAR_ALREADY_REGISTERED, 
    NPC_RESPONSE_WAR_REGISTRATION_OK,    
    NPC_RESPONSE_ALREADY_HAS_CASTLE,     
    NPC_RESPONSE_WAR_UNAVAILABLE,        

    
    NPC_RESPONSE_RACE_WAR_JOIN_FAILED,     
    NPC_RESPONSE_RACE_WAR_JOIN_OK,         
    NPC_RESPONSE_RACE_WAR_GO_FIRST_SERVER, 
                                           // 83

    
    NPC_RESPONSE_GIVE_EVENT_ITEM_FAIL_NOW, 
    NPC_RESPONSE_GIVE_EVENT_ITEM_FAIL,     
    NPC_RESPONSE_GIVE_EVENT_ITEM_OK,       
    NPC_RESPONSE_GIVE_PREMIUM_USER_ONLY,   

    
    NPC_RESPONSE_WAIT_FOR_MEET_COUPLE,   
    NPC_RESPONSE_COUPLE_MEET_SUCCESS,    
    NPC_RESPONSE_COUPLE_CANNOT_MEET,     
    NPC_RESPONSE_MEET_WAIT_TIME_EXPIRED, 

    NPC_RESPONSE_WAIT_FOR_APART_COUPLE,   
    NPC_RESPONSE_COUPLE_APART_SUCCESS,    
    NPC_RESPONSE_NOT_COUPLE,              
    NPC_RESPONSE_APART_WAIT_TIME_EXPIRED, 

    NPC_RESPONSE_APART_COUPLE_FORCE, 

    NPC_RESPONSE_QUEST,      
    NPC_RESPONSE_LOTTERY,    
    NPC_RESPONSE_CANNOT_BUY, 

    NPC_RESPONSE_CLEAR_RANK_BONUS_OK,      
    NPC_RESPONSE_NO_RANK_BONUS,            
    NPC_RESPONSE_ALREADY_CLEAR_RANK_BONUS, 

    NPC_RESPONSE_GNOME_CONTRACT_OK, 
    NPC_RESPONSE_DOWN_SKILL,        

    
    NPC_RESPONSE_GUILD_REGIST_FAIL_ALREADY_JOIN,   
                                                   // // 105
    NPC_RESPONSE_GUILD_REGIST_FAIL_QUIT_TIMEOUT,   
                                                   
    NPC_RESPONSE_GUILD_REGIST_FAIL_CANCEL_TIMEOUT, 
                                                   
                                                   
    NPC_RESPONSE_GUILD_REGIST_FAIL_LEVEL, 
    NPC_RESPONSE_GUILD_REGIST_FAIL_MONEY, 
    NPC_RESPONSE_GUILD_REGIST_FAIL_FAME,  
                                          
    NPC_RESPONSE_GUILD_REGIST_FAIL_NAME,  
    NPC_RESPONSE_GUILD_REGIST_FAIL_DENY,  

    NPC_RESPONSE_GUILD_STARTING_FAIL_ALREADY_JOIN,   
    NPC_RESPONSE_GUILD_STARTING_FAIL_QUIT_TIMEOUT,   
                                                     
    NPC_RESPONSE_GUILD_STARTING_FAIL_CANCEL_TIMEOUT, 
                                                     
                                                     // // 115
    NPC_RESPONSE_GUILD_STARTING_FAIL_LEVEL, 
                                            // 116
    NPC_RESPONSE_GUILD_STARTING_FAIL_MONEY, 
    NPC_RESPONSE_GUILD_STARTING_FAIL_FAME,  
                                            
    NPC_RESPONSE_GUILD_STARTING_FAIL_DENY,  

    NPC_RESPONSE_TOO_MANY_GUILD_REGISTERED,  
    NPC_RESPONSE_REINFORCE_DENYED,           
    NPC_RESPONSE_ALREADY_REINFORCE_ACCEPTED, 
                                             // 122
    NPC_RESPONSE_NO_WAR_REGISTERED,          
    NPC_RESPONSE_CANNOT_ACCEPT,              
    NPC_RESPONSE_ACCEPT_OK,                  
    NPC_RESPONSE_CANNOT_DENY,                
    NPC_RESPONSE_DENY_OK,                    

    NPC_RESPONSE_SHOW_TAX_RATIO,        
    NPC_RESPONSE_MODIFY_TAX_RATIO_OK,   
    NPC_RESPONSE_MODIFY_TAX_RATIO_FAIL, 

    NPC_RESPONSE_SWAP_ADVANCEMENT_ITEM, 
    NPC_RESPONSE_NOT_ADVANCED,          

    NPC_RESPONSE_SHOW_DONATION_DIALOG,          
    NPC_RESPONSE_SHOW_DONATION_COMPLETE_DIALOG, 
                                                
    NPC_RESPONSE_SHOW_CONFIRM_GET_EVENT_ITEM_DIALOG, 
    NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG,         

    NPC_RESPONSE_SHOW_DONATION_WEDDING_COMPLETE_DIALOG, //
    NPC_RESPONSE_SHOW_ALEADY_DONATED_DIALOG,            

    NPC_RESPONSE_MAX // 139
};

enum CoupleMessage {
    COUPLE_MESSAGE_NOT_EVENT_TERM = 1, 
    COUPLE_MESSAGE_ALREADY_WAITING,    
    COUPLE_MESSAGE_LOGOFF,             
    COUPLE_MESSAGE_DIFFERENT_RACE,     
    COUPLE_MESSAGE_SAME_SEX,           
    COUPLE_MESSAGE_NOT_PAY_PLAYER,     
    COUPLE_MESSAGE_ALREADY_COUPLE,     
    COUPLE_MESSAGE_WAS_COUPLE,         
    COUPLE_MESSAGE_NOT_ENOUGH_GOLD,    
    COUPLE_MESSAGE_NOT_ENOUGH_ATTR,    
    COUPLE_MESSAGE_NOT_ENOUGH_LEVEL,   
    COUPLE_MESSAGE_INVENTORY_FULL,     
    COUPLE_MESSAGE_NO_WAITING,         
    COUPLE_MESSAGE_NOT_COUPLE,         

    COUPLE_MESSAGE_MAX
};

enum QuestMessage {
    START_SUCCESS = 0,
    START_FAIL_QUEST_NUM_EXCEEDED,    // 1
    START_FAIL_DUPLICATED_QUEST_ID,   // 2
    START_FAIL_PC,                    // 3
    COMPLETE_SUCCESS,                 // 4
    COMPLETE_FAIL_NOT_COMPLETE,       // 5
    COMPLETE_FAIL_NO_INVENTORY_SPACE, // 6
    COMPLETE_FAIL_NOT_IN_QUEST,       // 7
    COMPLETE_FAIL_TIME_EXPIRED,       // 8
    START_FAIL_CANNOT_APPLY_QUEST,    // 9
    CANCEL_SUCCESS,                   // 10
    CANCEL_NOT_IN_QUEST,              // 11
    COMPLETE_FAIL_INVALID_NPC,        // 12
    FAIL_BUG                          // 13
};

enum CommonMessage {
    YOU_CAN_GET_EVENT_200501_COMBACK_ITEM = 0,       
    YOU_CAN_GET_EVENT_200501_COMBACK_PREMIUM_ITEM,   
    YOU_CAN_GET_EVENT_200501_COMBACK_RECOMMEND_ITEM, 
    YOU_GET_EVENT_ITEM,                              
    NOT_ENOUGH_INVENTORY_SPACE,                      
    ALEADY_GET_EVENT_ITEM,                           
    FAIL_GET_EVENT_ITEM,                             
    YOU_ARE_NOT_EVENT_TARGET,                        

    COMMON_MESSAGE_MAX
};

//////////////////////////////////////////////////////////////////////////////
// class GCNPCResponse
//////////////////////////////////////////////////////////////////////////////

class GCNPCResponse : public Packet {
public:
    GCNPCResponse() {
        m_Code = NPC_RESPONSE_MAX;
        m_Parameter = 0;
    }
    virtual ~GCNPCResponse() {}

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);

    PacketID_t getPacketID() const {
        return PACKET_GC_NPC_RESPONSE;
    }
    PacketSize_t getPacketSize() const;
    string getPacketName() const {
        return "GCNPCResponse";
    }
    string toString() const;

public:
    BYTE getCode(void) const {
        return m_Code;
    }
    void setCode(WORD code) {
        m_Code = code;
    }

    uint getParameter(void) const {
        return m_Parameter;
    }
    void setParameter(uint parameter) {
        m_Parameter = parameter;
    }

private:
    WORD m_Code;
    uint m_Parameter;
};


//////////////////////////////////////////////////////////////////////////////
// class GCNPCResponseFactory;
//////////////////////////////////////////////////////////////////////////////

class GCNPCResponseFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCNPCResponse();
    }
    string getPacketName() const {
        return "GCNPCResponse";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_NPC_RESPONSE;
    }
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + szuint;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class GCNPCResponseHandler
//////////////////////////////////////////////////////////////////////////////

class GCNPCResponseHandler {
public:
    static void execute(GCNPCResponse* pPacket, Player* pPlayer);
};

#endif
