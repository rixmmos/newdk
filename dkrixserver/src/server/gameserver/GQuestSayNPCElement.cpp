#include "GQuestSayNPCElement.h"

#include "Assert.h"

GQuestElement::ResultType GQuestSayNPCElement::checkMission(GQuestMission* pMission) const {
    GQuestSayNPCMission* pSayNPCMission = dynamic_cast<GQuestSayNPCMission*>(pMission);
    if (pSayNPCMission == NULL)
        return FAIL;
    if (pSayNPCMission->isMet())
        return OK;

    return WAIT;
}

GQuestMission* GQuestSayNPCElement::makeInitMission(PlayerCreature* pPC) const {
    GQuestSayNPCMission* pMission = new GQuestSayNPCMission;

    return pMission;
}

GQuestSayNPCElement* GQuestSayNPCElement::makeElement(XMLTree* pTree) {
    GQuestSayNPCElement* pRet = new GQuestSayNPCElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip the read and assign an uninitialised `target` to
    // m_Target. Initialised here as well.
    int target = 0;
    bool bHasTarget = pTree->GetAttribute("target", target);
    Assert(bHasTarget);
    pRet->m_Target = (NPCID_t)target;

    DWORD index;
    if (pTree->GetAttribute("index", index))
        pRet->m_Index = index;

    return pRet;
}

GQuestSayNPCElement g_SayNPCElement;
