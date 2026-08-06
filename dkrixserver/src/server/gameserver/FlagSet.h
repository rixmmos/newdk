//////////////////////////////////////////////////////////////////////////////
// Filename    : FlagSet.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __FLAGSET_H__
#define __FLAGSET_H__

#include <string>

#include "Exception.h"
#include "Types.h"

#define FLAG_SIZE_MAX 3 * 8

enum FlagSetType {
    FLAGSET_RECEIVE_NEWBIE_ITEM,         
    FLAGSET_RECEIVE_NEWBIE_ITEM_FIGHTER, // 1 - sword, blade
    FLAGSET_RECEIVE_NEWBIE_ITEM_CLERIC,  // 2 - heal, enchant
    FLAGSET_RECEIVE_NEWBIE_ITEM_GUNNER,  // 3 - gun

    FLAGSET_TRADE_GIFT_BOX_2002_12, 
    FLAGSET_RECEIVE_GREEN_GIFT_BOX, 

    FLAGSET_RECEIVE_NEWBIE_ITEM_AUTO, 

    FLAGSET_RECEIVE_PREMIUM_EVENT_ITEM_2003_3, 

    FLAGSET_IS_COUPLE,  
    FLAGSET_WAS_COUPLE, 

    FLAGSET_NOT_JUST_CREATED, 

    FLAGSET_CLEAR_RANK_BONUS_5,  
    FLAGSET_CLEAR_RANK_BONUS_10, 
    FLAGSET_CLEAR_RANK_BONUS_15, 
    FLAGSET_CLEAR_RANK_BONUS_20, 

    FLAGSET_GNOMES_HORN, 

    FLAGSET_SWAP_COAT,    
    FLAGSET_SWAP_TROUSER, 
    FLAGSET_SWAP_WEAPON,  

    FLAGSET_MAX
};

//////////////////////////////////////////////////////////////////////////////
// class Flag
//////////////////////////////////////////////////////////////////////////////

class FlagSet {
    ///// Member methods /////

public:
    FlagSet();
    ~FlagSet();

public:
    void create(const string& owner);
    void load(const string& owner);
    void save(const string& owner);
    void destroy(const string& owner);

public:
    bool isOn(int index);
    bool turnOn(int index);
    bool turnOff(int index);

public:
    string toString(void);
    static FlagSet fromString(const string& text);

    static void initialize(void);

protected:
    bool isValidIndex(int index);
    BYTE* getData() {
        return &m_pData[0];
    }


    ///// Member data /////

protected:
    BYTE m_pData[FLAG_SIZE_MAX / 8];

    static string m_pLookup[256];
    static bool m_bInit;
};


#endif
