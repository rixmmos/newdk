//////////////////////////////////////////////////////////////////////////////
// Filename    : ModifyInfo.h
// Written By  : elca@ewestsoft.com
// Description :



//////////////////////////////////////////////////////////////////////////////

#ifndef __MODIFY_INFO_H__
#define __MODIFY_INFO_H__

#include <list>

#include "Exception.h"
#include "Packet.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
enum ModifyType {
    MODIFY_BASIC_STR = 0,           
    MODIFY_CURRENT_STR,             
    MODIFY_MAX_STR,                 
    MODIFY_STR_EXP,                 
    MODIFY_BASIC_DEX,               
    MODIFY_CURRENT_DEX,             
    MODIFY_MAX_DEX,                 
    MODIFY_DEX_EXP,                 
    MODIFY_BASIC_INT,               
    MODIFY_CURRENT_INT,             
    MODIFY_MAX_INT,                 
    MODIFY_INT_EXP,                 
    MODIFY_CURRENT_HP,              
    MODIFY_MAX_HP,                  
    MODIFY_CURRENT_MP,              
    MODIFY_MAX_MP,                  
    MODIFY_MIN_DAMAGE,              
    MODIFY_MAX_DAMAGE,              
    MODIFY_DEFENSE,                 
    MODIFY_PROTECTION,              
    MODIFY_TOHIT,                   
    MODIFY_VISION,                  
    MODIFY_FAME,                    
    MODIFY_GOLD,                    
    MODIFY_SWORD_DOMAIN_LEVEL,      
    MODIFY_SWORD_DOMAIN_EXP,        
    MODIFY_SWORD_DOMAIN_GOAL_EXP,   
    MODIFY_BLADE_DOMAIN_LEVEL,      
    MODIFY_BLADE_DOMAIN_EXP,        
    MODIFY_BLADE_DOMAIN_GOAL_EXP,   
    MODIFY_HEAL_DOMAIN_LEVEL,       
    MODIFY_HEAL_DOMAIN_EXP,         
    MODIFY_HEAL_DOMAIN_GOAL_EXP,    
    MODIFY_ENCHANT_DOMAIN_LEVEL,    
    MODIFY_ENCHANT_DOMAIN_EXP,      
    MODIFY_ENCHANT_DOMAIN_GOAL_EXP, 
    MODIFY_GUN_DOMAIN_LEVEL,        
    MODIFY_GUN_DOMAIN_EXP,          
    MODIFY_GUN_DOMAIN_GOAL_EXP,     
    MODIFY_ETC_DOMAIN_LEVEL,        
    MODIFY_ETC_DOMAIN_EXP,          
    MODIFY_ETC_DOMAIN_GOAL_EXP,     
    MODIFY_SKILL_LEVEL,             
    MODIFY_LEVEL,                   
    MODIFY_EFFECT_STAT,             
    MODIFY_DURATION,                
    MODIFY_BULLET,                  
    MODIFY_BONUS_POINT,             
    MODIFY_DURABILITY,              
    MODIFY_NOTORIETY,               
    MODIFY_VAMP_GOAL_EXP,           
    MODIFY_SILVER_DAMAGE,           
    MODIFY_ATTACK_SPEED,            
    MODIFY_ALIGNMENT,               
    MODIFY_SILVER_DURABILITY,       
    MODIFY_REGEN_RATE,              
    MODIFY_GUILDID,                 
    MODIFY_RANK,                    
    MODIFY_RANK_EXP,                
    MODIFY_OUSTERS_GOAL_EXP,        
    MODIFY_SKILL_BONUS_POINT,       

    MODIFY_ELEMENTAL_FIRE,
    MODIFY_ELEMENTAL_WATER,
    MODIFY_ELEMENTAL_EARTH,
    MODIFY_ELEMENTAL_WIND,

    MODIFY_SKILL_EXP, 

    MODIFY_PET_HP,
    MODIFY_PET_EXP, 

    MODIFY_LAST_TARGET, 
    MODIFY_UNIONID,     
    MODIFY_UNIONGRADE,  

    MODIFY_ADVANCEMENT_CLASS_LEVEL,    
    MODIFY_ADVANCEMENT_CLASS_GOAL_EXP, 

    MODIFY_MAX
};

const string ModifyType2String[] = {"BASIC_STR",
                                    "CURRENT_STR",
                                    "MAX_STR",
                                    "STR_EXP",
                                    "BASIC_DEX",
                                    "CURRENT_DEX",
                                    "MAX_DEX",
                                    "DEX_EXP",
                                    "BASIC_INT",
                                    "CURRENT_INT",
                                    "MAX_INT",
                                    "INT_EXP",
                                    "CURRENT_HP",
                                    "MAX_HP",
                                    "CURRENT_MP",
                                    "MAX_MP",
                                    "MIN_DAMAGE",
                                    "MAX_DAMAGE",
                                    "DEFENSE",
                                    "PROTECTION",
                                    "TOHIT",
                                    "VISION",
                                    "FAME",
                                    "GOLD",
                                    "SWORD_DOMAIN_LEVEL",
                                    "SWORD_DOMAIN_EXP",
                                    "SWORD_DOMAIN_GOAL_EXP",
                                    "BLADE_DOMAIN_LEVEL",
                                    "BLADE_DOMAIN_EXP",
                                    "BLADE_DOMAIN_GOAL_EXP",
                                    "HEAL_DOMAIN_LEVEL",
                                    "HEAL_DOMAIN_EXP",
                                    "HEAL_DOMAIN_GOAL_EXP",
                                    "ENCHANT_DOMAIN_LEVEL",
                                    "ENCHANT_DOMAIN_EXP",
                                    "ENCHANT_DOMAIN_GOAL_EXP",
                                    "GUN_DOMAIN_LEVEL",
                                    "GUN_DOMAIN_EXP",
                                    "GUN_DOMAIN_GOAL_EXP",
                                    "ETC_DOMAIN_LEVEL",
                                    "ETC_DOMAIN_EXP",
                                    "ETC_DOMAIN_GOAL_EXP",
                                    "SKILL_LEVEL",
                                    "LEVEL",
                                    "EFFECT_STAT",
                                    "DURATION",
                                    "BULLET",
                                    "BONUS_POINT",
                                    "DURABILITY",
                                    "NOTORIETY",
                                    "VAMP_EXP",
                                    "SILVER_DAMAGE",
                                    "ATTACK_SPEED",
                                    "ALIGNMENT",
                                    "SILVER_DURABILITY",
                                    "REGEN_RATE",
                                    "GUILDID",
                                    "RANK",
                                    "RANK_EXP",
                                    "MODIFY_OUSTERS_EXP",
                                    "MODIFY_SKILL_BONUS_POINT",

                                    "MODIFY_ELEMENTAL_FIRE",
                                    "MODIFY_ELEMENTAL_WATER",
                                    "MODIFY_ELEMENTAL_EARTH",
                                    "MODIFY_ELEMENTAL_WIND",

                                    "MODIFY_SKILL_EXP", 

                                    "MODIFY_PET_HP",
                                    "MODIFY_PET_EXP", 

                                    "MODIFY_LAST_TARGET", 
                                    "MODIFY_UNIONID",
                                    "MODIFY_UNIONGRADE",

                                    "MODIFY_ADVANCEMENT_CLASS_LEVEL",    
                                    "MODIFY_ADVANCEMENT_CLASS_GOAL_EXP", 

                                    "MAX"};

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
typedef struct _SHORTDATA {
    BYTE type;
    ushort value;

} SHORTDATA;

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
typedef struct _LONGDATA {
    BYTE type;
    DWORD value;

} LONGDATA;


//////////////////////////////////////////////////////////////////////////////
// class ModifyInfo;


//////////////////////////////////////////////////////////////////////////////

class ModifyInfo : public Packet {
public:
    ModifyInfo();
    virtual ~ModifyInfo() noexcept;

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    PacketSize_t getPacketSize() const {
        return szBYTE * 2 + m_ShortCount * (szBYTE + szshort) + m_LongCount * (szBYTE + szDWORD);
    }
    static PacketSize_t getPacketMaxSize() {
        return szBYTE * 2 + 255 * (szBYTE + szshort + szBYTE + szDWORD);
    }
    string toString() const;

public:
    BYTE getShortCount(void) const {
        return m_ShortCount;
    }
    BYTE getLongCount(void) const {
        return m_LongCount;
    }

    void addShortData(ModifyType type, ushort value);
    void addLongData(ModifyType type, ulong value);

    void popShortData(SHORTDATA& rData);
    void popLongData(LONGDATA& rData);

    void clearList(void) {
        m_ShortCount = 0;
        m_LongCount = 0;
        m_ShortList.clear();
        m_LongList.clear();
    }

protected:
    BYTE m_ShortCount;
    list<SHORTDATA> m_ShortList;

    BYTE m_LongCount;
    list<LONGDATA> m_LongList;
};

#endif
