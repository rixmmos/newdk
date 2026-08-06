#ifndef __MONSTER_INFO_TYPES__
#define __MONSTER_INFO_TYPES__


//////////////////////////////////////////////////////////////////////////////
// Class Monster;

//////////////////////////////////////////////////////////////////////////////
enum {
    CLAN_NONE,                    
    CLAN_VAMPIRE_MONSTER,         
    CLAN_VAMPIRE_BATHORY_MONSTER, 
    CLAN_VAMPIRE_TEPEZ_MONSTER,   
    CLAN_SLAYER_MONSTER,          

    CLAN_MAX
};

extern int DefaultClanID[CLAN_MAX];

//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
enum RegenType {
    REGENTYPE_PORTAL,    
    REGENTYPE_INVISIBLE, 
    REGENTYPE_BAT,       
    REGENTYPE_HIDE,      

    REGENTYPE_MAX
};

#endif
