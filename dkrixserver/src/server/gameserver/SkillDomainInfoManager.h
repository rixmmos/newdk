//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillDomainInfoManager.h
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __SKILL_DOMAIN_INFO_MANAGER_H__
#define __SKILL_DOMAIN_INFO_MANAGER_H__

#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class DomainInfo
//////////////////////////////////////////////////////////////////////////////

class DomainInfo {
public:
    DomainInfo();
    ~DomainInfo();

public:
    SkillDomainType_t getType() const {
        return m_Type;
    }
    void setType(SkillDomainType_t type) {
        m_Type = type;
    }

    Level_t getLevel() const {
        return m_Level;
    }
    void setLevel(Level_t level) {
        m_Level = level;
    }

    uint getGoalExp() const {
        return m_GoalExp;
    }
    void setGoalExp(Exp_t exp) {
        m_GoalExp = exp;
    }

    uint getAccumExp() const {
        return m_AccumExp;
    }
    void setAccumExp(Exp_t exp) {
        m_AccumExp = exp;
    }

    ItemType_t getBestItemType() const {
        return m_BestItemType;
    }
    void setBestItemType(ItemType_t it) {
        m_BestItemType = it;
    }

    string toString() const;

private:
    
    SkillDomainType_t m_Type;

    
    Level_t m_Level;

    
    Exp_t m_GoalExp;

    
    Exp_t m_AccumExp;

    
    ItemType_t m_BestItemType;
};

//--------------------------------------------------------------------
//
// Class DomainInfoManager
//
//--------------------------------------------------------------------
/*
class DomainInfoManager {

public:

    // constructor
    DomainInfoManager() ;

    // destructor
    ~DomainInfoManager() ;

    // initialize manager
    void init() ;

    // get sub info class manager
    DomainInfoManager* getInfoManager(Domain DomainType) const ;

    // get item info
    SkillDomainInfo* getSkillDomainInfo(Level_t Level) const ;

    uint getDomainCount() const ;

    // toString for debug
    string toString() const ;

private:

    DomainInfo ** m_DomainInfoLists;

};
*/

//////////////////////////////////////////////////////////////////////////////
// Class SkillDomainInfoManager
//////////////////////////////////////////////////////////////////////////////

class SkillDomainInfoManager {
public:
    SkillDomainInfoManager();
    ~SkillDomainInfoManager();

public:
    // initialize manager
    void init();

    // get item info
    DomainInfo* getDomainInfo(SkillDomain DomainType, Level_t Level) const;

    // addDomainInfo
    void addDomainInfo(DomainInfo* pDomainInfo) const;

    // toString for debug
    string toString() const;

private:
    DomainInfo** m_DomainInfoLists[SKILL_DOMAIN_MAX];

    // Number of entries actually allocated in m_DomainInfoLists[i]. init()
    // sizes each domain to MAX(Level) + 1 from its own SkillDomainInfo rows,
    // which is not the same for every domain -- in the live data SKILL_DOMAIN_ETC
    // stops at level 100 while the others reach 150 -- so 151 is not a usable
    // bound and there was previously nothing that recorded the real one.
    int m_DomainInfoCounts[SKILL_DOMAIN_MAX];
};

extern SkillDomainInfoManager* g_pSkillDomainInfoManager;

#endif
