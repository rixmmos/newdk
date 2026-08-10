#ifndef __PET_EXP_INFO_H__
#define __PET_EXP_INFO_H__

#include <vector>

#include "Types.h"
#include "types/PetTypes.h"

const PetLevel_t PetMaxLevel = 50;

class PetExpInfo {
public:
    PetExpInfo(PetLevel_t petLevel, PetExp_t goalExp) : m_PetLevel(petLevel), m_PetGoalExp(goalExp) {}

    PetLevel_t getPetLevel() const {
        return m_PetLevel;
    }
    PetExp_t getPetGoalExp() const {
        return m_PetGoalExp;
    }

private:
    PetLevel_t m_PetLevel;
    PetExp_t m_PetGoalExp;

    /*	static PetExp_t getTargetExp( PetLevel_t level ) { return 250 * level * level + 1000; }
        static bool canLevelUp( PetLevel_t level, PetExp_t exp )
        {
            if ( level >= 50 ) return false;
            return exp >= getTargetExp(level+1);
        }*/
};

class PetExpInfoManager {
public:
    PetExpInfoManager() {
        // assign, not reserve: reserve() leaves size() at 0, so load()'s
        // m_PetExpInfos[PetLevel] = new PetExpInfo(...) wrote into
        // unconstructed storage, and clear()'s begin()..end() loop below saw an
        // empty range and freed nothing -- leaking all 51 objects. Identical to
        // Bug 18-J in PetAttrInfo; AddressSanitizer cannot see it because the
        // reserved capacity covers every write. PetMaxLevel + 1 because
        // PetExpInfo carries Level 0..50 inclusive.
        m_PetExpInfos.assign(PetMaxLevel + 1, static_cast<PetExpInfo*>(NULL));
    }
    ~PetExpInfoManager() {
        clear();
    }

    void clear();
    void load();

    bool canLevelUp(PetLevel_t level, PetExp_t exp);
    static PetExpInfoManager& Instance() {
        static PetExpInfoManager theInstance;
        return theInstance;
    }

    PetExpInfo* getPetExpInfo(PetLevel_t level) const {
        return m_PetExpInfos[level];
    }

private:
    vector<PetExpInfo*> m_PetExpInfos;
};

#endif
