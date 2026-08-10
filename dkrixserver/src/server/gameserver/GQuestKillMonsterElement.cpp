#include "GQuestKillMonsterElement.h"

#include "Assert.h"

GQuestElement::ResultType GQuestKillMonsterElement::checkMission(GQuestMission* pMission) const {
    GQuestKillMonsterMission* pKillMonsterMission = dynamic_cast<GQuestKillMonsterMission*>(pMission);
    if (pKillMonsterMission == NULL)
        return FAIL;

    if (pKillMonsterMission->getCurrent() >= m_Goal)
        return OK;

    return WAIT;
}

GQuestMission* GQuestKillMonsterElement::makeInitMission(PlayerCreature* pPC) const {
    GQuestKillMonsterMission* pMission = new GQuestKillMonsterMission;

    int TotalNum = m_TargetList.size();
    int TargetNum = min((int)m_TargetNum, TotalNum);
    int selected = 0;
    cout << TotalNum << " " << TargetNum << "  ." << endl;

    for (int i = 0; i < TotalNum; ++i) {
        int toSelect = TargetNum - selected;
        if (toSelect <= 0)
            break;

        int last = TotalNum - i;

        if ((rand() % last) < toSelect) {
            pMission->getTargetList().push_back(m_TargetList[i]);
            ++selected;
            cout << (int)m_TargetList[i] << " ." << endl;
        }
    }

    cout << "~" << endl;

    return pMission;
}

GQuestKillMonsterElement* GQuestKillMonsterElement::makeElement(XMLTree* pTree) {
    GQuestKillMonsterElement* pRet = new GQuestKillMonsterElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip both reads and leave m_Goal at the ctor's 0 --
    // a kill mission that is complete before the first monster dies.
    bool bHasNum = pTree->GetAttribute("num", pRet->m_Goal);
    bool bHasSort = pTree->GetAttribute("sort", pRet->m_TargetNum);

    Assert(bHasNum);
    Assert(bHasSort);

    DWORD index;
    if (pTree->GetAttribute("index", index))
        pRet->m_Index = index;

    for (size_t i = 0; i < pTree->GetChildCount(); ++i) {
        XMLTree* pChild = pTree->GetChild(i);
        Assert(pChild->GetName() == "Target");
        // Hoisted out of Assert() for the same reason -- under NDEBUG every
        // target would be pushed as sprite type 0 (and from an uninitialised
        // local at that).
        DWORD targetSType = 0;
        bool bHasType = pChild->GetAttribute("type", targetSType);
        Assert(bHasType);
        m_TargetList.push_back((SpriteType_t)targetSType);
    }

    return pRet;
}

GQuestKillMonsterElement g_KillMonsterElement;
