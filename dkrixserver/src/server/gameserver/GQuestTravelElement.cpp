#include "GQuestTravelElement.h"

#include "Assert.h"
#include "GQuestCheckPoint.h"
#include "MonsterInfo.h"
#include "PlayerCreature.h"

void GQuestTravelMission::updateStr() {
    m_StrArg = "";
    vector<DWORD>::iterator itr = m_TargetList.begin();
    for (; itr != m_TargetList.end(); ++itr) {
        if (!isVisited(*itr))
            m_StrArg += GQuestCheckPoint::Instance().getStringFromWayPoint(*itr) + ",";
    }
}

GQuestElement::ResultType GQuestTravelElement::checkMission(GQuestMission* pMission) const {
    GQuestTravelMission* pTravelMission = dynamic_cast<GQuestTravelMission*>(pMission);
    if (pTravelMission == NULL)
        return FAIL;

    if (pTravelMission->getTargetList().size() <= pTravelMission->getVisitedList().size())
        return OK;

    return WAIT;
}

GQuestMission* GQuestTravelElement::makeInitMission(PlayerCreature* pPC) const {
    GQuestTravelMission* pMission = new GQuestTravelMission;
    pMission->m_StrArg = GQuestCheckPoint::Instance().getTargetList(pPC->getRace(), pPC->getLevel(), m_Grade, m_BaseNum,
                                                                    pMission->getTargetList());

    return pMission;
}

GQuestTravelElement* GQuestTravelElement::makeElement(XMLTree* pTree) {
    GQuestTravelElement* pRet = new GQuestTravelElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip both reads and every travel mission would be
    // built from grade 0 / basenum 0.
    bool bHasGrade = pTree->GetAttribute("grade", pRet->m_Grade);
    bool bHasBaseNum = pTree->GetAttribute("basenum", pRet->m_BaseNum);

    Assert(bHasGrade);
    Assert(bHasBaseNum);

    DWORD index;
    if (pTree->GetAttribute("index", index))
        pRet->m_Index = index;

    return pRet;
}

GQuestTravelElement g_TravelElement;
