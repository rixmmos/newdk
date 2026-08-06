//////////////////////////////////////////////////////////////////////////////
// Filename    : MonsterFunctionObject.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __MONSTER_FUNCTION_OBJECT_H__
#define __MONSTER_FUNCTION_OBJECT_H__

#include "Monster.h"
#include "Ousters.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////////
//
// class StrongerSlayer, WeakerSlayer, StrongerVampire, WeakerVampire;
//



//







//




//


//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// class StrongerSlayer;



//////////////////////////////////////////////////////////////////////////////

class StrongerSlayer {
public:
    StrongerSlayer(Zone* pZone, SkillLevel_t skillLevelSum) {
        m_pZone = pZone;
        m_SkillDomainLevelSum = skillLevelSum;
    }

    bool operator()(ObjectID_t objectID) {
        Assert(m_pZone != NULL); // by sigi

        Creature* pCreature = NULL;

        try {
            pCreature = m_pZone->getCreature(objectID);
        } catch (NoSuchElementException& nsee) {
            // cout << nsee.toString() << endl;
            pCreature = NULL;
        }

        if (pCreature == NULL)
            return false; // by sigi

        if (!pCreature->isSlayer())
            return true;

        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

        return pSlayer->getSkillDomainLevelSum() > m_SkillDomainLevelSum;
    }

private:
    Zone* m_pZone;
    SkillLevel_t m_SkillDomainLevelSum;
};


//////////////////////////////////////////////////////////////////////////////
// class WeakerSlayer;




//////////////////////////////////////////////////////////////////////////////

class WeakerSlayer {
public:
    // constructor
    WeakerSlayer(Zone* pZone, SkillLevel_t skillLevelSum) {
        m_pZone = pZone;
        m_SkillDomainLevelSum = skillLevelSum;
    }

    bool operator()(ObjectID_t objectID) {
        Assert(m_pZone != NULL); // by sigi

        Creature* pCreature = NULL;

        try {
            pCreature = m_pZone->getCreature(objectID);
        } catch (NoSuchElementException& nsee) {
            // cout << nsee.toString() << endl;
            pCreature = NULL;
        }

        if (pCreature == NULL)
            return false; // by sigi

        if (!pCreature->isSlayer())
            return true;

        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

        return pSlayer->getSkillDomainLevelSum() < m_SkillDomainLevelSum;
    }

private:
    Zone* m_pZone;
    SkillLevel_t m_SkillDomainLevelSum;
};


//////////////////////////////////////////////////////////////////////////////
// class StrongerVampire;



//////////////////////////////////////////////////////////////////////////////

class StrongerVampire {
public:
    StrongerVampire(Zone* pZone, Level_t level) {
        m_pZone = pZone;
        m_Level = level;
    }

    bool operator()(ObjectID_t objectID) {
        Assert(m_pZone != NULL); // by sigi

        Creature* pCreature = NULL;

        try {
            pCreature = m_pZone->getCreature(objectID);
        } catch (NoSuchElementException& nsee) {
            // cout << nsee.toString() << endl;
            pCreature = NULL;
        }

        if (pCreature == NULL)
            return false; // by sigi

        if (!pCreature->isVampire())
            return false;

        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

        return pVampire->getLevel() > m_Level;
    }

private:
    Zone* m_pZone;
    Level_t m_Level;
};


//////////////////////////////////////////////////////////////////////////////
// class WeakerVampire;



//////////////////////////////////////////////////////////////////////////////

class WeakerVampire {
public:
    WeakerVampire(Zone* pZone, Level_t level) {
        m_pZone = pZone;
        m_Level = level;
    }

    bool operator()(ObjectID_t objectID) {
        Assert(m_pZone != NULL); // by sigi

        Creature* pCreature = NULL;

        try {
            pCreature = m_pZone->getCreature(objectID);
        } catch (NoSuchElementException& nsee) {
            // cout << nsee.toString() << endl;
            pCreature = NULL;
        }

        if (pCreature == NULL)
            return false; // by sigi

        if (!pCreature->isVampire())
            return false;

        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

        return pVampire->getLevel() < m_Level;
    }

private:
    Zone* m_pZone;
    Level_t m_Level;
};

class StrongerOusters {
public:
    StrongerOusters(Zone* pZone, Level_t level) {
        m_pZone = pZone;
        m_Level = level;
    }

    bool operator()(ObjectID_t objectID) {
        Assert(m_pZone != NULL); // by sigi

        Creature* pCreature = NULL;

        try {
            pCreature = m_pZone->getCreature(objectID);
        } catch (NoSuchElementException& nsee) {
            // cout << nsee.toString() << endl;
            pCreature = NULL;
        }

        if (pCreature == NULL)
            return false; // by sigi

        if (!pCreature->isOusters())
            return false;

        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

        return pOusters->getLevel() > m_Level;
    }

private:
    Zone* m_pZone;
    Level_t m_Level;
};


//////////////////////////////////////////////////////////////////////////////
// class WeakerOusters;



//////////////////////////////////////////////////////////////////////////////

class WeakerOusters {
public:
    WeakerOusters(Zone* pZone, Level_t level) {
        m_pZone = pZone;
        m_Level = level;
    }

    bool operator()(ObjectID_t objectID) {
        Assert(m_pZone != NULL); // by sigi

        Creature* pCreature = NULL;

        try {
            pCreature = m_pZone->getCreature(objectID);
        } catch (NoSuchElementException& nsee) {
            // cout << nsee.toString() << endl;
            pCreature = NULL;
        }

        if (pCreature == NULL)
            return false; // by sigi

        if (!pCreature->isOusters())
            return false;

        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

        return pOusters->getLevel() < m_Level;
    }

private:
    Zone* m_pZone;
    Level_t m_Level;
};

//////////////////////////////////////////////////////////////////////////////
// class StrongerMonster;



//////////////////////////////////////////////////////////////////////////////

class StrongerMonster {
public:
    StrongerMonster(Zone* pZone, Level_t level) {
        m_pZone = pZone;
        m_Level = level;
    }

    bool operator()(ObjectID_t objectID) {
        Assert(m_pZone != NULL); // by sigi

        Creature* pCreature = NULL;

        try {
            pCreature = m_pZone->getCreature(objectID);
        } catch (NoSuchElementException& nsee) {
            // cout << nsee.toString() << endl;
            pCreature = NULL;
        }

        if (pCreature == NULL)
            return false; // by sigi

        if (!pCreature->isMonster())
            return false;

        Monster* pMonster = dynamic_cast<Monster*>(pCreature);

        return pMonster->getLevel() > m_Level;
    }

private:
    Zone* m_pZone;
    Level_t m_Level;
};


//////////////////////////////////////////////////////////////////////////////
// class WeakerMonster;



//////////////////////////////////////////////////////////////////////////////

class WeakerMonster {
public:
    WeakerMonster(Zone* pZone, Level_t level) {
        m_pZone = pZone;
        m_Level = level;
    }

    bool operator()(ObjectID_t objectID) {
        Assert(m_pZone != NULL); // by sigi

        Creature* pCreature = NULL;

        try {
            pCreature = m_pZone->getCreature(objectID);
        } catch (NoSuchElementException& nsee) {
            // cout << nsee.toString() << endl;
            pCreature = NULL;
        }

        if (pCreature == NULL)
            return false; // by sigi

        if (!pCreature->isMonster())
            return false;

        Monster* pMonster = dynamic_cast<Monster*>(pCreature);

        return pMonster->getLevel() < m_Level;
    }

private:
    Zone* m_pZone;
    Level_t m_Level;
};


#endif
