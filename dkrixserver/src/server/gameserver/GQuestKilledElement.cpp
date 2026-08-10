#include "GQuestKilledElement.h"

#include "Assert.h"

GQuestElement::ResultType GQuestKilledElement::checkMission(GQuestMission* pMission) const {
    GQuestKilledMission* pKilledMission = dynamic_cast<GQuestKilledMission*>(pMission);
    if (pKilledMission == NULL)
        return FAIL;

    if (pKilledMission->getCurrent() >= m_Goal)
        return OK;

    return WAIT;
}

GQuestMission* GQuestKilledElement::makeInitMission(PlayerCreature* pPC) const {
    GQuestKilledMission* pMission = new GQuestKilledMission(m_Goal);

    return pMission;
}

GQuestKilledElement* GQuestKilledElement::makeElement(XMLTree* pTree) {
    GQuestKilledElement* pRet = new GQuestKilledElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip the parse and leave m_Goal at the ctor's 0.
    bool bHasNum = pTree->GetAttribute("num", pRet->m_Goal);
    Assert(bHasNum);

    DWORD index;
    if (pTree->GetAttribute("index", index))
        pRet->m_Index = index;

    return pRet;
}

GQuestKilledElement g_KilledElement;
