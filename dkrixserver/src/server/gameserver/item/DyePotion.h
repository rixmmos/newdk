//////////////////////////////////////////////////////////////////////////////
// Filename    : DyePotion.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __DYE_POTION_H__
#define __DYE_POTION_H__

#include "ConcreteItem.h"
#include "InfoClassManager.h"
#include "Item.h"
#include "ItemFactory.h"
#include "ItemInfo.h"
#include "ItemLoader.h"
#include "ItemPolicies.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class DyePotion;
//////////////////////////////////////////////////////////////////////////////

class DyePotion : public ConcreteItem<Item::ITEM_CLASS_DYE_POTION, Stackable, NoDurability, NoOption, NoGrade,
                                      NoAttacking, NoEnchantLevel> {
public:
    DyePotion();
    DyePotion(ItemType_t itemType, const list<OptionType_t>& optionType, ItemNum_t Num);

public:
    virtual void create(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y,
                        ItemID_t itemID = 0);
    virtual void save(const string& ownerID, Storage storage, StorageID_t storageID, BYTE x, BYTE y);
    void tinysave(const string& field) const {
        tinysave(field.c_str());
    }
    void tinysave(const char* field) const;
    virtual string toString() const;

    static void initItemIDRegistry(void);

public:
    //	virtual ItemClass getItemClass() const  { return Item::ITEM_CLASS_DYE_POTION; }
    //	virtual string getObjectTableName() const  { return "DyePotionObject"; }

    /*	virtual ItemType_t getItemType() const  { return m_ItemType; }
        virtual void setItemType(ItemType_t itemType)  { m_ItemType = itemType; }

        virtual VolumeWidth_t getVolumeWidth() const ;
        virtual VolumeHeight_t getVolumeHeight() const ;
        virtual Weight_t getWeight() const ;

    public:
        virtual ItemNum_t getNum() const  { return m_Num; }
        virtual void setNum(ItemNum_t Num)  { m_Num = Num; }

        bool    isStackable() const  { return false; }
    */
private:
    //	ItemType_t m_ItemType;
    //	ItemNum_t  m_Num;

    static Mutex m_Mutex;             
    static ItemID_t m_ItemIDRegistry; 
};

//////////////////////////////////////////////////////////////////////////////
// class DyePotionInfo
//////////////////////////////////////////////////////////////////////////////

class DyePotionInfo : public ItemInfo {
public:
    enum ITEM_FUNCTION {
        FUNCTION_HAIR = 0,          
        FUNCTION_SKIN = 1,          
        FUNCTION_SEX = 2,           
        FUNCTION_BAT = 3,           
        FUNCTION_REGEN = 4,         
        FUNCTION_MASTER_EFFECT = 5, 
    };

public:
    virtual Item::ItemClass getItemClass() const {
        return Item::ITEM_CLASS_DYE_POTION;
    }
    virtual string toString() const;

public:
    virtual BYTE getFunctionFlag() const {
        return m_fFunction;
    }
    virtual void setFunctionFlag(BYTE flag) {
        m_fFunction = flag;
    }

    virtual int getFunctionValue() const {
        return m_FunctionValue;
    }
    virtual void setFunctionValue(int value) {
        m_FunctionValue = value;
    }


    BYTE m_fFunction;    
    int m_FunctionValue; 
};

//////////////////////////////////////////////////////////////////////////////
// class DyePotionInfoManager;
//////////////////////////////////////////////////////////////////////////////

class DyePotionInfoManager : public InfoClassManager {
public:
    virtual Item::ItemClass getItemClass() const {
        return Item::ITEM_CLASS_DYE_POTION;
    }
    virtual void load();
};

extern DyePotionInfoManager* g_pDyePotionInfoManager;

//////////////////////////////////////////////////////////////////////////////
// class DyePotionFactory
//////////////////////////////////////////////////////////////////////////////

class DyePotionFactory : public ItemFactory {
public:
    virtual Item::ItemClass getItemClass() const {
        return Item::ITEM_CLASS_DYE_POTION;
    }
    virtual string getItemClassName() const {
        return "DyePotion";
    }

public:
    virtual Item* createItem(ItemType_t ItemType, const list<OptionType_t>& OptionType) {
        return new DyePotion(ItemType, OptionType, 1);
    }
};

//////////////////////////////////////////////////////////////////////////////
// class DyePotionLoader;
//////////////////////////////////////////////////////////////////////////////

class DyePotionLoader : public ItemLoader {
public:
    virtual Item::ItemClass getItemClass() const {
        return Item::ITEM_CLASS_DYE_POTION;
    }
    virtual string getItemClassName() const {
        return "DyePotion";
    }

public:
    virtual void load(Creature* pCreature);
    virtual void load(Zone* pZone);
    virtual void load(StorageID_t storageID, Inventory* pInventory);
};

extern DyePotionLoader* g_pDyePotionLoader;

#endif
