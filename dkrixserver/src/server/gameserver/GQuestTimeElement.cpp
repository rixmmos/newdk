#include "GQuestTimeElement.h"

#include "Assert.h"
#include "GQuestManager.h"

GQuestElement::ResultType GQuestTimeElement::checkMission(GQuestMission* pMission) const {
    GQuestTimeMission* pTimeMission = dynamic_cast<GQuestTimeMission*>(pMission);
    if (pTimeMission == NULL)
        return FAIL;

    Timeval endTime = pTimeMission->getEndTime();
    if (gCurrentTime > endTime)
        return OK;

    return WAIT;
}

GQuestMission* GQuestTimeElement::makeInitMission(PlayerCreature* pPC) const {
    GQuestTimeMission* pMission = NULL;

    Timeval endTime;
    getCurrentTime(endTime);
    endTime.tv_sec += 60 * (int)m_LimitMinutes;
    pMission = new GQuestTimeMission(endTime);
    pMission->updateArg();

    return pMission;
}

GQuestTimeElement* GQuestTimeElement::makeElement(XMLTree* pTree) {
    GQuestTimeElement* pRet = new GQuestTimeElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip the parse and leave m_LimitMinutes at the ctor's
    // 0 -- a timed quest that expires immediately.
    bool bHasLimit = pTree->GetAttribute("limit", pRet->m_LimitMinutes);
    Assert(bHasLimit);

    DWORD index;
    if (pTree->GetAttribute("index", index))
        pRet->m_Index = index;

    return pRet;
}

GQuestTimeElement g_TimeElement;
