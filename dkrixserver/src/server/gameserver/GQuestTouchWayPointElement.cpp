#include "GQuestTouchWayPointElement.h"

#include "Assert.h"
#include "MonsterCorpse.h"
#include "Zone.h"
#include "ZoneUtil.h"

GQuestElement::ResultType GQuestTouchWayPointElement::checkMission(GQuestMission* pMission) const {
    GQuestTouchWayPointMission* pTouchWayPointMission = dynamic_cast<GQuestTouchWayPointMission*>(pMission);
    if (pTouchWayPointMission == NULL)
        return FAIL;
    if (pTouchWayPointMission->isTouch())
        return OK;

    return WAIT;
}

GQuestMission* GQuestTouchWayPointElement::makeInitMission(PlayerCreature* pPC) const {
    GQuestTouchWayPointMission* pMission = new GQuestTouchWayPointMission;

    return pMission;
}

GQuestTouchWayPointElement* GQuestTouchWayPointElement::makeElement(XMLTree* pTree) {
    GQuestTouchWayPointElement* pRet = new GQuestTouchWayPointElement;

    // These five reads used to cast each member to DWORD& and let
    // XMLTree::GetAttribute store four bytes through it. m_ZoneID/m_X/m_Y/
    // m_Type are WORD and m_Dir is BYTE, so every one of them wrote past the
    // member it was aiming at. The first UBSan boot run (2026-08-10) reported
    // it as "reference binding to misaligned address ... for type 'unsigned
    // int'" here, and as the matching store inside SXml.cpp. It happened to
    // produce the right values only because the members are written in
    // declaration order, so each overwrite was repaired by the next call --
    // reordering the members in the header would have silently corrupted them.
    // Read into DWORD locals and narrow explicitly instead.
    DWORD zoneID = 0;
    DWORD x = 0;
    DWORD y = 0;
    DWORD type = 0;
    DWORD dir = 0;

    Assert(pTree->GetAttribute("zoneid", zoneID));
    Assert(pTree->GetAttribute("x", x));
    Assert(pTree->GetAttribute("y", y));
    Assert(pTree->GetAttribute("type", type));
    Assert(pTree->GetAttribute("dir", dir));

    pRet->m_ZoneID = (ZoneID_t)zoneID;
    pRet->m_X = (ZoneCoord_t)x;
    pRet->m_Y = (ZoneCoord_t)y;
    pRet->m_Type = (MonsterType_t)type;
    pRet->m_Dir = (Dir_t)dir;

    DWORD index;
    if (pTree->GetAttribute("index", index))
        pRet->m_Index = index;

    MonsterCorpse* pMonsterCorpse = new MonsterCorpse(pRet->m_Type, " ", pRet->m_Dir);
    pMonsterCorpse->setTreasureCount(255);
    Zone* pZone = getZoneByZoneID(pRet->m_ZoneID);
    Assert(pZone != NULL);
    pZone->registerObject(pMonsterCorpse);
    pZone->addItem(pMonsterCorpse, pRet->m_X, pRet->m_Y, true, 0xffffffff);

    Assert(pMonsterCorpse->getX() == pRet->m_X);
    Assert(pMonsterCorpse->getY() == pRet->m_Y);

    return pRet;
}

GQuestTouchWayPointElement g_TouchWayPointElement;
