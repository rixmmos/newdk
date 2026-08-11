#include "GQuestGiveEventQuestItemElement.h"

#include "CreatureUtil.h"
#include "DB.h"
#include "GCSystemMessage.h"
#include "GQuestInventory.h"
#include "Ousters.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "Slayer.h"
#include "Vampire.h"
#include "skill/SkillUtil.h"

Exp_t ExpRewardTable[3][25] = {

    {40,   50,   80,   135,  225,  360,   550,   800,   1130,  1530,  2030,  2620, 3320,
     4140, 5090, 6170, 7390, 8780, 10300, 12000, 13900, 16000, 18300, 20700, 23400},

    {110,  150,  190,  150,  320,  420,  540,   700,   920,   1200,  1550,  2020,
     2620, 3400, 4430, 5760, 7490, 9730, 12650, 16450, 20560, 25700, 32120, 40150},
    {110,  150,  190,  150,  320,  420,  540,   700,   920,   1200,  1550,  2020,
     2620, 3400, 4430, 5760, 7490, 9730, 12650, 16450, 20560, 25700, 32120, 40150}};

Gold_t GoldRewardTable[3][4] = {
    {70000, 84000, 98000, 11200}, {30000, 36000, 42000, 48000}, {85000, 102000, 119000, 136000}};

void giveMemberReward(PlayerCreature* pPC, uint type) {
    Exp_t exp = ExpRewardTable[pPC->getRace()][pPC->getLevel()];
    Gold_t gold = GoldRewardTable[pPC->getRace()][type];

    cout << exp << "  , " << gold << " " << endl;

    pPC->increaseGoldEx(gold);
    GCModifyInformation gcMI;
    gcMI.addLongData(MODIFY_GOLD, pPC->getGold());

    if (pPC->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
        increaseDomainExp(pSlayer, pSlayer->getHighestSkillDomain(), exp, gcMI);
        pSlayer->getPlayer()->sendPacket(&gcMI);
    } else if (pPC->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
        increaseVampExp(pVampire, exp, gcMI);
        pVampire->getPlayer()->sendPacket(&gcMI);
    } else if (pPC->isOusters()) {
        GCModifyInformation gcMI;
        Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
        increaseOustersExp(pOusters, exp, gcMI);
    }
}

GQuestElement::ResultType GQuestGiveEventQuestItemElement::checkCondition(PlayerCreature* pPC) const {
    if (m_Grade == 0) {
        GQuestInventory& inventory = pPC->getGQuestManager()->getGQuestInventory();
        ItemType_t base;
        if (pPC->isVampire()) {
            base = 4 + m_Type - 1;
        } else if (pPC->isSlayer()) {
            base = 7 + m_Type - 1;
        } else if (pPC->isOusters()) {
            base = 10 + m_Type - 1;
        }

        inventory.getItems().push_back(base);
        pPC->getPlayer()->sendPacket(inventory.getInventoryPacket());

        Statement* pStmt = NULL;

        BEGIN_DB {
            pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
            pStmt->executeQuery("INSERT INTO GQuestItemObject(ItemType, OwnerID) VALUES (%u, '%s')", base,
                                pPC->getName().c_str());
            SAFE_DELETE(pStmt);
        }
        END_DB(pStmt)
    }

    giveLotto(pPC, m_Type, m_Grade + 1);

    Party* pParty = pPC->getLocalPartyManager()->getParty(pPC->getPartyID());
    if (pParty != NULL) {
        unordered_map<string, Creature*> members = pParty->getMemberMap();
        unordered_map<string, Creature*>::iterator itr = members.begin();

        for (; itr != members.end(); ++itr) {
            PlayerCreature* pMember = dynamic_cast<PlayerCreature*>(itr->second);
            if (pMember == NULL || pMember == pPC)
                continue;
            if (pMember->getLevel() < 25) {
                giveMemberReward(pMember, m_Type);
            }
        }
    }

    return OK;
}

GQuestGiveEventQuestItemElement* GQuestGiveEventQuestItemElement::makeElement(XMLTree* pTree) {
    GQuestGiveEventQuestItemElement* pRet = new GQuestGiveEventQuestItemElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip both reads. m_Type would stay at the ctor's 0 --
    // outside the 1..3 range the Assert below is meant to enforce, and this
    // element hands out reward items -- and `grade` would stay empty, so the
    // grade[0] switch below would index an empty string.
    bool bHasType = pTree->GetAttribute("type", pRet->m_Type);
    Assert(bHasType);
    Assert(pRet->m_Type <= 3 && pRet->m_Type >= 1);
    string grade;
    bool bHasGrade = pTree->GetAttribute("grade", grade);
    Assert(bHasGrade);
    Assert(!grade.empty());
    // Both Asserts above are ((void)0) under NDEBUG, so in Release a missing or
    // empty "grade" attribute would reach grade[0] as an out-of-range read.
    // Feed the switch a sentinel instead and let it take the default arm, which
    // is where an unrecognised grade letter already ends up: m_Grade keeps the
    // constructor's 0.
    switch (grade.empty() ? '\0' : grade[0]) {
    case 'A':
        pRet->m_Grade = 0;
        break;
    case 'B':
        pRet->m_Grade = 1;
        break;
    case 'C':
        pRet->m_Grade = 2;
        break;
    case 'D':
        pRet->m_Grade = 3;
        break;

    default:
        Assert(false);
    }

    return pRet;
}

GQuestGiveEventQuestItemElement g_GiveEventQuestItemElement;
