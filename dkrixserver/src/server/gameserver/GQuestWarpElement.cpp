#include "GQuestWarpElement.h"

#include "EventTransport.h"
#include "GamePlayer.h"
#include "PlayerCreature.h"
#include "ZoneUtil.h"

GQuestElement::ResultType GQuestWarpElement::checkCondition(PlayerCreature* pPC) const {
    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());

    EventTransport* pEventTransport = new EventTransport(pGamePlayer);

    pEventTransport->setDeadline(0);
    pEventTransport->setTargetZone(m_ZoneID, m_X, m_Y);

    pGamePlayer->addEvent(pEventTransport);

    return OK;
}

GQuestWarpElement* GQuestWarpElement::makeElement(XMLTree* pTree) {
    GQuestWarpElement* pRet = new GQuestWarpElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip all three reads and warp every player to zone 0
    // at (0,0) instead of the scripted destination.
    bool bHasZoneID = pTree->GetAttribute("zoneid", pRet->m_ZoneID);
    bool bHasX = pTree->GetAttribute("x", pRet->m_X);
    bool bHasY = pTree->GetAttribute("y", pRet->m_Y);

    Assert(bHasZoneID);
    Assert(bHasX);
    Assert(bHasY);

    return pRet;
}

GQuestWarpElement g_WarpElement;
