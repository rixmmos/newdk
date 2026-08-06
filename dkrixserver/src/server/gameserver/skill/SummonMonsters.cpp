//////////////////////////////////////////////////////////////////////////////
// Filename    : SummonMonsters.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "SummonMonsters.h"

#include "GCSay.h"
#include "GCSkillToTileOK5.h"
#include "MasterLairInfoManager.h"
#include "MasterLairManager.h"
#include "MonsterSummonInfo.h"
#include "ZoneUtil.h"


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
SummonMonsters::SummonMonsters() throw() {
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
 

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void SummonMonsters::execute(Monster* pMonster)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    // cout << "SummonMonsters" << endl;

    Assert(pMonster != NULL);
    Zone* pZone = pMonster->getZone();
    Assert(pZone != NULL);


    try {
        if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE)) {
            // cout << "SummonMonsters: hide" << endl;
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
            //--------------------------------------------------------
            
            //--------------------------------------------------------
            // SkillInput input(pMonster);
            // SkillOutput output;
            // computeOutput(input, output);

            SUMMON_INFO2 summonInfo;

            bool hasInfo = pMonster->getMonsterSummonInfo(summonInfo);

            if (!hasInfo || summonInfo.pMonsters == NULL) {
                // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
                
                executeSkillFailNormal(pMonster, getSkillType(), NULL);

                
                if (pZone->isMasterLair() && pMonster->isMaster()) {
                    MasterLairManager* pMasterLairManager = pZone->getMasterLairManager();
                    Assert(pMasterLairManager != NULL);

                    
                    
                    pMasterLairManager->setMasterReady();
                    // cout << "no more SummonMonsters: set MasterReady" << endl;
                }
            }

            if (pMonster->isMaster() && pZone->isMasterLair()) {
                MasterLairManager* pMasterLairManager = pZone->getMasterLairManager();
                Assert(pMasterLairManager != NULL);
                

                MasterLairInfo* pInfo = g_pMasterLairInfoManager->getMasterLairInfo(pZone->getZoneID());
                Assert(pInfo != NULL);

                if (!pMasterLairManager->isMasterReady()) {
                    x = pInfo->getSummonX();
                    y = pInfo->getSummonY();

                    GCSay gcSay;
                    gcSay.setObjectID(pMonster->getObjectID());
                    gcSay.setColor(MASTER_SAY_COLOR);
                    gcSay.setMessage(pInfo->getRandomMasterSummonSay());
                    if (!gcSay.getMessage().empty())
                        pZone->broadcastPacket(pMonster->getX(), pMonster->getY(), &gcSay);
                }

                
                // by sigi. 2002.11.21
                summonInfo.hasItem = false;
            }

            summonInfo.scanEnemy = true;
            summonInfo.clanType = SUMMON_INFO::CLAN_TYPE_GROUP;
            summonInfo.clanID = pMonster->getClanType(); 
            summonInfo.X = x;
            summonInfo.Y = y;
            summonInfo.regenType = REGENTYPE_PORTAL;

            
            addMonstersToZone(pZone, summonInfo);

            // cout << "SummonMonsters OK" << endl;
            GCSkillToTileOK5 _GCSkillToTileOK5;

            _GCSkillToTileOK5.setObjectID(pMonster->getObjectID());
            _GCSkillToTileOK5.setSkillType(getSkillType());
            _GCSkillToTileOK5.setX(x);
            _GCSkillToTileOK5.setY(y);
            _GCSkillToTileOK5.setDuration(0);

            pZone->broadcastPacket(x, y, &_GCSkillToTileOK5);
        } else {
            executeSkillFailNormal(pMonster, getSkillType(), NULL);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pMonster, getSkillType());
    }

    __END_CATCH
}

SummonMonsters g_SummonMonsters;
