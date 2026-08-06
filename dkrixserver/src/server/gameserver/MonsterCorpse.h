//////////////////////////////////////////////////////////////////////////////
// Filename    : MonsterCorpse.h
// Written by  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __MONSTER_CORPSE_H__
#define __MONSTER_CORPSE_H__

#include "Corpse.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class MonsterCorpse;

//////////////////////////////////////////////////////////////////////////////

class Monster;

class MonsterCorpse : public Corpse {
public:
    MonsterCorpse(Monster* pMonster);
    MonsterCorpse(MonsterType_t MType, const string& name, Dir_t dir);
    virtual ~MonsterCorpse();

public:
    virtual void create(const string& ownerID, Storage storage, DWORD storageID, BYTE x, BYTE y, ItemID_t itemID = 0);
    virtual bool destroy();
    virtual void save(const string& ownerID, Storage storage, DWORD storageID, BYTE x, BYTE y);
    void tinysave(const string& field) const {
        tinysave(field.c_str());
    }
    void tinysave(const char* field) const {}

    virtual ItemClass getItemClass() const {
        return ITEM_CLASS_CORPSE;
    }

    virtual ItemType_t getItemType() const {
        return MONSTER_CORPSE;
    }
    virtual void setItemType(ItemType_t monsterType) {
        throw UnsupportedError(__PRETTY_FUNCTION__);
    }

    bool isNextTreasureHead() {
        return (!m_Treasures.empty() && m_Treasures.front()->getItemClass() == ITEM_CLASS_SKULL);
    }

    virtual string toString() const;

    // own methods
public:
    MonsterType_t getMonsterType() const {
        return m_MonsterType;
    }
    void setMonsterType(MonsterType_t monsterType) {
        m_MonsterType = monsterType;
    }

    string getName() const {
        return m_Name;
    }
    void setName(string name) {
        m_Name = name;
    }

    string getMonsterName() const {
        return m_Name;
    }
    void setMonsterName(string name) {
        m_Name = name;
    }

    Dir_t getDir() const {
        return m_Dir;
    }
    void setDir(Dir_t dir) {
        m_Dir = dir;
    }

    bool gethasHead() const {
        return m_bhasHead;
    }
    void addHead() {
        m_bhasHead = true;
    }
    void removeHead() {
        m_bhasHead = false;
    }

    const string& getHostName(void) const {
        return m_HostName;
    }
    void setHostName(const string& name) {
        m_HostName = name;
    }

    int getHostPartyID(void) const {
        return m_HostPartyID;
    }
    void setHostPartyID(int ID) {
        m_HostPartyID = ID;
    }

    const string& getQuestHostName() const {
        return m_QuestHostName;
    }
    void setQuestHostName(const string& name) {
        m_QuestHostName = name;
    }

    bool isShrine() const {
        return m_bShrine;
    }
    void setShrine(bool bShrine = true) {
        m_bShrine = bShrine;
    }

    void setLastKiller(ObjectID_t id) {
        m_LastKiller = id;
    }
    ObjectID_t getLastKiller() const {
        return m_LastKiller;
    }

    // data members
protected:
    MonsterType_t m_MonsterType; 
    string m_Name;               
    Dir_t m_Dir;                 
    bool m_bhasHead;             
    string m_HostName;           
    int m_HostPartyID;           
    string m_QuestHostName;      

    bool m_bShrine; 
    ObjectID_t m_LastKiller;
};

#endif
