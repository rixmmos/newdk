#include "GQuestBloodDrainElement.h"

#include "Assert.h"

GQuestElement::ResultType GQuestBloodDrainElement::checkMission(GQuestMission* pMission) const {
    GQuestBloodDrainMission* pBloodDrainMission = dynamic_cast<GQuestBloodDrainMission*>(pMission);
    if (pBloodDrainMission == NULL)
        return FAIL;

    if (pBloodDrainMission->getCurrent() >= m_Goal)
        return OK;

    return WAIT;
}

GQuestMission* GQuestBloodDrainElement::makeInitMission(PlayerCreature* pPC) const {
    GQuestBloodDrainMission* pMission = new GQuestBloodDrainMission(m_Goal);

    return pMission;
}

GQuestBloodDrainElement* GQuestBloodDrainElement::makeElement(XMLTree* pTree) {
    GQuestBloodDrainElement* pRet = new GQuestBloodDrainElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG and never
    // evaluates its argument, so a Release build would skip the parse entirely
    // and leave m_Goal at the ctor's 0 -- a blood drain mission already complete
    // before it starts.
    bool bHasNum = pTree->GetAttribute("num", pRet->m_Goal);
    Assert(bHasNum);

    DWORD index;
    if (pTree->GetAttribute("index", index))
        pRet->m_Index = index;

    return pRet;
}

GQuestBloodDrainElement g_BloodDrainElement;
