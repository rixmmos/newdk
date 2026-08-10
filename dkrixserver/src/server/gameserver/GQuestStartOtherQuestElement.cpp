#include "GQuestStartOtherQuestElement.h"

#include "GQuestManager.h"
#include "PlayerCreature.h"

GQuestElement::ResultType GQuestStartOtherQuestElement::checkCondition(PlayerCreature* pPC) const {
    pPC->getGQuestManager()->accept(m_QuestID);
    return OK;
}

GQuestStartOtherQuestElement* GQuestStartOtherQuestElement::makeElement(XMLTree* pTree) {
    GQuestStartOtherQuestElement* pRet = new GQuestStartOtherQuestElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip the parse and leave m_QuestID at the ctor's 0 --
    // every StartOtherQuest element would then chain into quest 0.
    bool bHasID = pTree->GetAttribute("id", pRet->m_QuestID);
    Assert(bHasID);

    return pRet;
}

GQuestStartOtherQuestElement g_StartOtherQuestElement;
