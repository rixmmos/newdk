//////////////////////////////////////////////////////////////////////////////
// Filename    : DuplicateSelf.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "DuplicateSelf.h"

#include "GCFakeMove.h"
#include "GCSay.h"
#include "GCSkillToTileOK5.h"
#include "MasterLairInfoManager.h"
#include "MasterLairManager.h"
#include "MonsterManager.h"
#include "MonsterSummonInfo.h"
#include "ZoneUtil.h"


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
DuplicateSelf::DuplicateSelf() throw() {
    __BEGIN_TRY

    
    m_DuplicateMonsterTypes[432] = 435; 
    m_DuplicateMonsterTypes[434] = 436; 

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
 

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void DuplicateSelf::execute(Monster* pMonster)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    // cout << "DuplicateSelf" << endl;
    MonsterType_t MType = pMonster->getMonsterType();

    unordered_map<MonsterType_t, MonsterType_t>::const_iterator itr = m_DuplicateMonsterTypes.find(MType);

    
    if (itr == m_DuplicateMonsterTypes.end()) {
        // cout <<"DuplicateSelf::noMonsterType: " << (int)MType << endl;
        return;
    }

    MonsterType_t DuplicateMType = itr->second;

    Assert(pMonster != NULL);

    try {
        Zone* pZone = pMonster->getZone();
        Assert(pZone != NULL);

        if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE)) {
            // cout << "DuplicateSelf: hide" << endl;
            return;
        }
        if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)) {
            addVisibleCreature(pZone, pMonster, true);
        }

        // GCSkillToSelfOK2 _GCSkillToSelfOK2;

        ZoneCoord_t x = pMonster->getX();
        ZoneCoord_t y = pMonster->getY();

        bool bRangeCheck = checkZoneLevelToUseSkill(pMonster);
        // bool bMoveModeCheck = pMonster->isWalking();

        if (bRangeCheck) // && bMoveModeCheck)
        {
            // cout << "DuplicateSelf OK" << endl;
            GCSkillToTileOK5 _GCSkillToTileOK5;

            _GCSkillToTileOK5.setObjectID(pMonster->getObjectID());
            _GCSkillToTileOK5.setSkillType(getSkillType());
            _GCSkillToTileOK5.setX(x);
            _GCSkillToTileOK5.setY(y);
            _GCSkillToTileOK5.setDuration(0);

            pZone->broadcastPacket(x, y, &_GCSkillToTileOK5);

            //--------------------------------------------------------
            
            //--------------------------------------------------------
            // SkillInput input(pMonster);
            // SkillOutput output;
            // computeOutput(input, output);
            
            SUMMON_INFO summonInfo;
            summonInfo.scanEnemy = true;
            summonInfo.hasItem = false;
            summonInfo.initHPPercent = pMonster->getHP(ATTR_CURRENT) * 100 / pMonster->getHP(ATTR_MAX);

            int numFake = min((1 + rand() % 3), pMonster->getINT() / 100);

            // cout << "numDuplicate =  " << numFake<< endl;
            MonsterManager* pMonsterManager = pZone->getMonsterManager();
            Assert(pMonsterManager != NULL);

            list<Monster*> summonedMonsters;

            for (int i = 0; i < numFake; i++) {
                int X = max(0, min((int)pZone->getWidth() - 1, (x - 5 + rand() % 11)));
                int Y = max(0, min((int)pZone->getHeight() - 1, (y - 5 + rand() % 11)));

                try {
                    pMonsterManager->addMonsters(X, Y, DuplicateMType, 1, summonInfo, &summonedMonsters);
                } catch (Throwable& t) {
                    cerr << t.toString() << endl;
                }
            }

            
            list<Monster*>::const_iterator iMonster = summonedMonsters.begin();

            for (; iMonster != summonedMonsters.end(); iMonster++) {
                Monster* pFakeMonster = *iMonster;

                GCFakeMove gcFakeMove;
                gcFakeMove.setObjectID(pMonster->getObjectID());
                gcFakeMove.setXY(pFakeMonster->getX(), pFakeMonster->getY());

                pZone->broadcastPacket(x, y, &gcFakeMove);
            }

            
            
            for (int i = 0; i < 50; i++) {
                int X = max(0, min((int)pZone->getWidth() - 1, (x - 8 + rand() % 11)));
                int Y = max(0, min((int)pZone->getHeight() - 1, (y - 8 + rand() % 11)));

                if (pZone->moveFastMonster(pMonster, x, y, X, Y, getSkillType())) {
                    break;
                }
            }
        } else {
            // cout << "RangeCheckFailed" << endl;
            executeSkillFailNormal(pMonster, getSkillType(), NULL);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pMonster, getSkillType());
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

DuplicateSelf g_DuplicateSelf;
