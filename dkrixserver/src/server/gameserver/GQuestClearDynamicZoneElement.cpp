#include "GQuestClearDynamicZoneElement.h"

#include "Assert.h"

GQuestElement::ResultType GQuestClearDynamicZoneElement::checkMission(GQuestMission* pMission) const {
    GQuestClearDynamicZoneMission* pClearDynamicZoneMission = dynamic_cast<GQuestClearDynamicZoneMission*>(pMission);
    if (pClearDynamicZoneMission == NULL)
        return WAIT;
    if (pClearDynamicZoneMission->isClear())
        return OK;

    return WAIT;
}

GQuestMission* GQuestClearDynamicZoneElement::makeInitMission(PlayerCreature* pPC) const {
    GQuestClearDynamicZoneMission* pMission = new GQuestClearDynamicZoneMission(m_TargetZoneID);

    return pMission;
}

GQuestClearDynamicZoneElement* GQuestClearDynamicZoneElement::makeElement(XMLTree* pTree) {
    GQuestClearDynamicZoneElement* pRet = new GQuestClearDynamicZoneElement;

    // Two fixes in one, both the shapes cleaned up in GQuestTouchWayPointElement
    // (18-AA) and CGBuyStoreItemHandler (18-K).
    //
    // The (int&) cast is an out-of-bounds write: m_TargetZoneID is ZoneID_t,
    // i.e. WORD, so GetAttribute's int& overload stored four bytes through a
    // two-byte member. It only survives because m_TargetZoneID is the last
    // member and the extra two bytes land in the object's tail padding --
    // declaring any member after it in the header would silently corrupt it.
    //
    // And the call must not live inside Assert(): that macro is ((void)0) under
    // NDEBUG and never evaluates its argument, so a Release build would skip the
    // parse entirely and leave m_TargetZoneID at its (uninitialised) ctor value.
    int targetZoneID = 0;
    bool bHasZoneID = pTree->GetAttribute("zoneid", targetZoneID);
    Assert(bHasZoneID);
    if (bHasZoneID)
        pRet->m_TargetZoneID = (ZoneID_t)targetZoneID;

    DWORD index;
    if (pTree->GetAttribute("index", index))
        pRet->m_Index = index;

    return pRet;
}

GQuestClearDynamicZoneElement g_ClearDynamicZoneElement;
