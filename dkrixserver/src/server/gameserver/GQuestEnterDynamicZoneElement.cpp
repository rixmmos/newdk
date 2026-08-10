#include "GQuestEnterDynamicZoneElement.h"

#include "Assert.h"

GQuestElement::ResultType GQuestEnterDynamicZoneElement::checkMission(GQuestMission* pMission) const {
    GQuestEnterDynamicZoneMission* pEnterDynamicZoneMission = dynamic_cast<GQuestEnterDynamicZoneMission*>(pMission);
    if (pEnterDynamicZoneMission == NULL)
        return WAIT;
    if (pEnterDynamicZoneMission->isEnter())
        return OK;

    return WAIT;
}

GQuestMission* GQuestEnterDynamicZoneElement::makeInitMission(PlayerCreature* pPC) const {
    GQuestEnterDynamicZoneMission* pMission = new GQuestEnterDynamicZoneMission(m_TargetZoneID);

    return pMission;
}

GQuestEnterDynamicZoneElement* GQuestEnterDynamicZoneElement::makeElement(XMLTree* pTree) {
    GQuestEnterDynamicZoneElement* pRet = new GQuestEnterDynamicZoneElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip the read and assign an uninitialised `temp` to
    // m_TargetZoneID. Initialised here as well, so the NDEBUG path is at worst
    // zone 0 rather than stack garbage.
    int temp = 0;
    bool bHasZoneID = pTree->GetAttribute("zoneid", temp);
    Assert(bHasZoneID);
    pRet->m_TargetZoneID = (ZoneID_t)temp;

    DWORD index;
    if (pTree->GetAttribute("index", index))
        pRet->m_Index = index;

    return pRet;
}

GQuestEnterDynamicZoneElement g_EnterDynamicZoneElement;
