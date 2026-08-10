#ifndef __PET_ATTR_INFO_H__
#define __PET_ATTR_INFO_H__

#include <vector>

#include <unordered_map>

#include "Exception.h"
#include "PetExpInfo.h"
#include "Types.h"
#include "types/PetTypes.h"

class PetInfo;

class PetAttrInfo {
public:
    PetAttrInfo(PetAttr_t PetAttr) : m_PetAttr(PetAttr) {
        // reserve() sets capacity, not size, and clear() then leaves size 0 --
        // so every m_PetAttrLevels[PetLevel] below wrote into unconstructed
        // storage. AddressSanitizer reports a heap-buffer-overflow WRITE in
        // setPetAttrLevel during PetAttrInfoManager::load(). assign() gives the
        // vector a real size. PetMaxLevel + 1 because levels are 0-based and
        // inclusive: PetAttrBalanceInfo carries Level 0..50 [measured].
        m_PetAttrLevels.assign(PetMaxLevel + 1, 0);
        m_EnchantRatio = 0;
    }
    PetAttr_t getPetAttr() const {
        return m_PetAttr;
    }

    // Bounds-checked: PetLevel comes from a database column, so a bad row must
    // not corrupt the heap. Real runtime checks, not Assert().
    PetAttrLevel_t getPetAttrLevel(PetLevel_t PetLevel) {
        if (PetLevel >= m_PetAttrLevels.size())
            return 0;

        return m_PetAttrLevels[PetLevel];
    }
    void setPetAttrLevel(PetLevel_t PetLevel, PetAttrLevel_t PetAttrLevel) {
        if (PetLevel >= m_PetAttrLevels.size())
            return;

        m_PetAttrLevels[PetLevel] = PetAttrLevel;
    }

    int getEnchantRatio() {
        return m_EnchantRatio;
    }
    void setEnchantRatio(int EnchantRatio) {
        m_EnchantRatio = EnchantRatio;
    }

private:
    PetAttr_t m_PetAttr;
    int m_EnchantRatio;
    vector<PetAttrLevel_t> m_PetAttrLevels;
};

class PetAttrInfoManager {
public:
    PetAttrInfoManager() {
        m_PetAttrInfoMap.clear();
    }
    ~PetAttrInfoManager() {
        clear();
    }

    void clear();
    void load();

    PetAttrInfo* getPetAttrInfo(PetAttr_t PetAttr) const;

    static PetAttrInfoManager& Instance() {
        static PetAttrInfoManager theInstance;
        return theInstance;
    }

    bool enchantRandomAttr(PetInfo* pPetInfo, int ratio);
    bool enchantSpecAttr(PetInfo* pPetInfo, PetAttr_t PetAttr);

private:
    unordered_map<PetAttr_t, PetAttrInfo*> m_PetAttrInfoMap;
};

#endif
