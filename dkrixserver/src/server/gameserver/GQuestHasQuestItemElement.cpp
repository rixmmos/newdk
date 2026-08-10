#include "GQuestHasQuestItemElement.h"

#include "GQuestInventory.h"
#include "GQuestManager.h"
#include "PlayerCreature.h"

GQuestElement::ResultType GQuestHasQuestItemElement::checkCondition(PlayerCreature* pPC) const {
    const list<ItemType_t>& inventory = pPC->getGQuestManager()->getGQuestInventory().getItems();

    list<ItemType_t>::const_iterator itr = inventory.begin();
    list<ItemType_t>::const_iterator endItr = inventory.end();

    int count = 0;

    for (; itr != endItr; ++itr) {
        if ((*itr) == m_Type) {
            ++count;
            if (count >= m_Num)
                return OK;
        }
    }

    return FAIL;
}

GQuestHasQuestItemElement* GQuestHasQuestItemElement::makeElement(XMLTree* pTree) {
    GQuestHasQuestItemElement* pRet = new GQuestHasQuestItemElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip the read and assign an uninitialised `type` to
    // m_Type -- the quest would then test for an arbitrary item. Initialised
    // here as well.
    DWORD type = 0;
    bool bHasType = pTree->GetAttribute("type", type);
    Assert(bHasType);
    pRet->m_Type = (ItemType_t)type;

    DWORD num;
    if (pTree->GetAttribute("num", num))
        pRet->m_Num = num;

    return pRet;
}

GQuestHasQuestItemElement g_HasQuestItemElement;
