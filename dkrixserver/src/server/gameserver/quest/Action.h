//////////////////////////////////////////////////////////////////////////////
// Filename    : Action.h
// Written By  :
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_H__
#define __ACTION_H__

#include "Exception.h"
#include "PropertyBuffer.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class Action
//////////////////////////////////////////////////////////////////////////////

class Creature;

class Action {
public:
    enum ActionTypes {
        ACTION_SET_POSITION,            
        ACTION_WANDER,                  
        ACTION_SAY,                     
        ACTION_RANDOM_SAY,              
        ACTION_ASK,                     
        ACTION_QUIT_DIALOGUE,           
        ACTION_PREPARE_SHOP,            
        ACTION_SELL,                    
        ACTION_BUY,                     
        ACTION_REGEN_SHOP,              
        ACTION_PREPARE_TEACH,           
        ACTION_TEACH_SKILL,             
        ACTION_HEAL,                    
        ACTION_REDEEM_MOTORCYCLE,       
        ACTION_SEARCH_MOTORCYCLE,       
        ACTION_REPAIR,                  
        ACTION_STASH_SELL,              
        ACTION_STASH_OPEN,              
        ACTION_RESTORE,                 
        ACTION_SET_RESURRECT_ZONE,      
        ACTION_SILVER_COATING,          
        ACTION_CREATE_GUILD,            
        ACTION_DESTROY_GUILD,           
        ACTION_TUTORIAL,                
        ACTION_GIVE_NEWBIE_ITEM,        
        ACTION_ACTIVATE_PORTAL,         
        ACTION_TURN_ON_FLAG,            
        ACTION_TURN_OFF_FLAG,           
        ACTION_SYSTEM_MESSAGE,          
        ACTION_TAKE_DONATION,           
        ACTION_CALL_HELICOPTER,         
        ACTION_REGEN_EVENT_SHOP,        
        ACTION_SAY_DYNAMIC,             
        ACTION_ASK_DYNAMIC,             
        ACTION_GIVE_SPECIAL_EVENT_ITEM, //
        ACTION_REDISTRIBUTE_ATTR,       //
        ACTION_GIVE_GARBAGE_ITEM,       

        // ACTION_TALK ,
        // ACTION_RANDOM_TALK ,
        // ACTION_GIVE ,
        // ACTION_TAKE ,
        // ACTION_DROP ,
        // ACTION_CREATE_ITEM ,
        // ACTION_DESTROY_ITEM ,
        // ACTION_SET_ATTRIBUTE ,
        // ACTION_SET_DISCOUNT_RATE ,
        // ACTION_DIE ,
        // ACTION_USE_SKILL ,
        // ACTION_USE_ITEM ,
        // ACTION_REMOVE_QUEST ,
        // ACTION_AFTER ,

        // Monster AI specific actions
        ACTION_ATTACK,
        ACTION_RETREAT,
        ACTION_MOVE,
        ACTION_ATTACK_MOVE,
        ACTION_STOP,
        ACTION_HOLD_POSITION,
        ACTION_PATROL,

        ACTION_TRADE_EVENT_ITEM,
        ACTION_SHOW_GUILD_DIALOG,

        ACTION_TRADE_LAIR_ITEM,

        ACTION_SIMPLE_QUEST_LIST,
        ACTION_SIMPLE_QUEST_COMPLETE,
        ACTION_SIMPLE_QUEST_REGEN,

        ACTION_TRADE_GIFT_BOX,

        ACTION_GIVE_TEST_SERVER_REWARD,

        
        // 2003. 1.20. by bezz,Sequoia
        ACTION_ENTER_CASTLE_WITH_FEE,

        
        ACTION_SHOW_TAX_BALANCE,

        ACTION_ASK_VARIABLE,

        
        ACTION_WAR_REGISTRATION,
        ACTION_SHOW_WAR_SCHEDULE,

        
        ACTION_JOIN_RACE_WAR,

        
        ACTION_GIVE_EVENT_ITEM,         
        ACTION_GIVE_ACCOUNT_EVENT_ITEM, 

        
        ACTION_INIT_PARTNER_WAITING_MANAGER,
        ACTION_WAIT_FOR_MEET_COUPLE,
        ACTION_WAIT_FOR_APART_COUPLE,
        ACTION_ACCEPT_COUPLE_REQUEST,
        ACTION_ACCEPT_APART_COUPLE,

        ACTION_FORCE_APART_COUPLE,

        ACTION_ENTER_PK_ZONE,
        ACTION_WARP_TO_RESURRECT_POSITION,

        ACTION_SELECT_QUEST,
        ACTION_QUEST_REWARD,

        ACTION_WARP_TO_NOVICE_ZONE,
        ACTION_CANCEL_QUEST,

        ACTION_INIT_SIMPLE_QUEST,
        ACTION_WANDER_ZONE,

        ACTION_INIT_EVENT_QUEST,
        ACTION_ASK_BY_QUEST_LEVEL,
        ACTION_GIVE_EVENT_QUEST,
        ACTION_ADVANCE_EVENT_QUEST,
        ACTION_REWARD_EVENT_QUEST,
        ACTION_CANCEL_EVENT_QUEST_SCRIPT,
        ACTION_GIVE_LOTTO,
        ACTION_GIVE_EVENT_QUEST_SCRIPT,
        ACTION_START_EVENT_QUEST,
        ACTION_WARP_IN_ZONE,
        ACTION_EVENT_MEET,
        ACTION_GIVE_FINAL_LOTTO_SCRIPT,
        ACTION_GIVE_QUEST_ITEM,
        ACTION_TAKE_OUT_GOODS,
        ACTION_CLEAR_RANK_BONUS,
        ACTION_CONTRACT_GNOMES_HORN,
        ACTION_DOWN_SKILL,
        ACTION_MINI_GAME,
        ACTION_GIVE_ITEM,
        ACTION_ACTIVATE_MAZE_EXIT,
        ACTION_ACTIVATE_MAZE_ENTER,
        ACTION_ACTIVATE_MAZE_RETURN,

        ACTION_SYSTEM_MESSAGE_PER_RACE, 

        ACTION_WARP_LEVEL_WAR_ZONE,

        ACTION_CHECK_PARTNER_NAME, 

        ACTION_START_PET_QUEST, 

        ACTION_PET_WITHDRAW, 
        ACTION_PET_DEPOSIT,  

        ACTION_ENTER_EVENT_ZONE, 
        ACTION_ENTER_GDR_LAIR,   

        ACTION_TRADE_GQUEST_EVENT_ITEM, 

        ACTION_ENTER_SIEGE, 

        ACTION_REGISTER_SIEGE,     
        ACTION_REGISTER_REINFORCE, 
        ACTION_ACCEPT_REINFORCE,   
        ACTION_DENY_REINFORCE,     

        ACTION_RECALL_SIEGE,       
        ACTION_SELECT_BLOOD_BIBLE, 
        ACTION_CLEAR_BLOOD_BIBLE,  

        ACTION_MODIFY_TAX_RATIO,      
        ACTION_SWAP_ADVANCEMENT_ITEM, 

        ACTION_SHOW_DONATION_DIALOG,               
        ACTION_ENTER_QUEST_ZONE,                   
        ACTION_SHOW_CONFIRM_GET_EVENT_ITEM_DIALOG, 

        ACTION_MAX
    };

public:
    virtual ~Action() {}
    virtual ActionType_t getActionType() const = 0;
    virtual void read(PropertyBuffer& buffer) = 0;
    virtual void execute(Creature* pCreature1, Creature* pCreature2 = NULL) = 0;
    virtual string toString() const = 0;
};

#endif
