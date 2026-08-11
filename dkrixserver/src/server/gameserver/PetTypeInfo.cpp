
#include "PetTypeInfo.h"

#include "DB.h"

MonsterType_t PetTypeInfo::getPetCreatureType(PetLevel_t petLevel) const {
    if (petLevel < 10)
        return getPetCreatureTypeByIndex(0);
    return getPetCreatureTypeByIndex(petLevel / 10 - 1);
}

void PetTypeInfoManager::clear() {
    vector<PetTypeInfo*>::iterator itr = m_PetTypeInfos.begin();
    vector<PetTypeInfo*>::iterator endItr = m_PetTypeInfos.end();

    for (; itr != endItr; ++itr) {
        SAFE_DELETE((*itr));
    }

    m_PetTypeInfos.clear();
}

void PetTypeInfoManager::load() {
    clear();

    // Must be NULL-initialised: END_DB's `delete STMT` runs on the catch path, which
    // is reachable before the assignment below.
    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        Result* pResult = pStmt->executeQuery("SELECT MAX(PetType) FROM PetTypeInfo");

        if (!pResult->next())
            throw Error("PetTypeInfo  .");

        PetType_t MaxPetType = pResult->getInt(1);

        // Was reserve(), which sets capacity and leaves size() at 0. Every slot
        // addPetTypeInfo wrote and getPetTypeInfo read was therefore past the end
        // of the vector, and the slots no row filled held indeterminate pointers
        // rather than NULL -- reserve() does not value-initialise. That is live in
        // the shipped data: PetTypeInfo holds PetType 1..5, so MAX is 5, the
        // allocation is 6 wide, and slot 0 is never written [measured 2026-08-11,
        // initdb/DARKEDEN.sql]. getPetTypeInfo(0) passed its bounds check and
        // returned a garbage pointer for the caller to dereference.
        //
        // resize() gives real, NULL-filled elements, which is what the NULL checks
        // at the call sites already assume, and it makes clear() free them instead
        // of iterating an empty range and leaking every PetTypeInfo.
        m_PetTypeInfos.resize(MaxPetType + 1, NULL);

        pResult = pStmt->executeQuery("SELECT PetType, OriginalMonsterType, CreatureType1, CreatureType2, "
                                      "CreatureType3, CreatureType4, CreatureType5, FoodType "
                                      "FROM PetTypeInfo");

        while (pResult->next()) {
            PetTypeInfo* pPetTypeInfo = new PetTypeInfo(pResult->getInt(1));
            pPetTypeInfo->m_OriginalMonsterType = pResult->getInt(2);
            pPetTypeInfo->m_PetCreatureType[0] = pResult->getInt(3);
            pPetTypeInfo->m_PetCreatureType[1] = pResult->getInt(4);
            pPetTypeInfo->m_PetCreatureType[2] = pResult->getInt(5);
            pPetTypeInfo->m_PetCreatureType[3] = pResult->getInt(6);
            pPetTypeInfo->m_PetCreatureType[4] = pResult->getInt(7);
            pPetTypeInfo->m_FoodType = pResult->getInt(8);

            addPetTypeInfo(pPetTypeInfo);
        }

        // END_DB frees the Statement only on the exception path; without this the
        // success path leaks the Statement and the Result it owns.
        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)
}

// Both bounds were capacity(), not size(). capacity() is only ever >= size(), and
// the standard lets an implementation hand back more than reserve() asked for, so
// an index in [size, capacity) passed the check and then indexed past the last
// element. size() is the number of elements that actually exist.
void PetTypeInfoManager::addPetTypeInfo(PetTypeInfo* pPetTypeInfo) {
    if (pPetTypeInfo->m_PetType >= m_PetTypeInfos.size())
        throw Error("PetTypeInfoManager::addPetTypeInfo: PetType out of range");

    m_PetTypeInfos[pPetTypeInfo->m_PetType] = pPetTypeInfo;
}

PetTypeInfo* PetTypeInfoManager::getPetTypeInfo(PetType_t PetType) {
    if (PetType >= m_PetTypeInfos.size())
        return NULL;

    return m_PetTypeInfos[PetType];
}
