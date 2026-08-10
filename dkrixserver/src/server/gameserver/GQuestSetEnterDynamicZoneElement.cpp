#include "GQuestSetEnterDynamicZoneElement.h"

#include "CreatureUtil.h"
#include "PlayerCreature.h"
#include "Zone.h"

GQuestElement::ResultType GQuestSetEnterDynamicZoneElement::checkCondition(PlayerCreature* pPC) const {
    pPC->getGQuestManager()->setEnterDynamicZone(m_DynamicZoneID, m_CanEnter);

    return OK;
}

GQuestSetEnterDynamicZoneElement* GQuestSetEnterDynamicZoneElement::makeElement(XMLTree* pTree) {
    GQuestSetEnterDynamicZoneElement* pRet = new GQuestSetEnterDynamicZoneElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip both reads and grant/deny dynamic-zone entry
    // against zone 0 with the ctor's m_CanEnter.
    int temp = 0;
    bool bHasZoneID = pTree->GetAttribute("zoneid", temp);
    Assert(bHasZoneID);
    pRet->m_DynamicZoneID = (ZoneID_t)temp;

    bool bHasCanEnter = pTree->GetAttribute("canenter", temp);
    Assert(bHasCanEnter);
    pRet->m_CanEnter = (BYTE)temp;

    cout << (int)pRet->m_DynamicZoneID << endl;
    cout << (int)pRet->m_CanEnter << endl;

    return pRet;
}

GQuestSetEnterDynamicZoneElement g_SetEnterDynamicZoneElement;
