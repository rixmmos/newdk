////////////////////////////////////////////////////////////////////////////////
// Filename    : MonsterManager.h
// Written By  : Reiot

// Description :
////////////////////////////////////////////////////////////////////////////////


#include "MonsterManager.h"

#include <stdlib.h> // atoi()

#include <fstream>

#include "Assert.h"
#include "CastleInfoManager.h"
#include "Creature.h"
#include "CreatureUtil.h"
#include "DB.h"
#include "DynamicZone.h"
#include "DynamicZoneGateOfAlter.h"
#include "EffectPacketSend.h"
#include "EventItemUtil.h"
#include "GCAddEffect.h"
#include "GCAddEffectToTile.h"
#include "GCCreatureDied.h"
#include "GCDeleteObject.h"
#include "GCSay.h"
#include "GDRLairManager.h"
#include "GamePlayer.h"
#include "ItemFactoryManager.h"
#include "ItemGradeManager.h"
#include "ItemInfoManager.h"
#include "ItemUtil.h"
#include "LogClient.h"
#include "LuckInfo.h"
#include "MasterLairInfoManager.h"
#include "Monster.h"
#include "MonsterCorpse.h"
#include "MonsterInfo.h"
#include "MonsterNameManager.h"
#include "OptionInfo.h"
#include "Player.h"
#include "Profile.h"
#include "Properties.h"
#include "Skull.h"
#include "StringPool.h"
#include "SweeperBonusManager.h"
#include "Thread.h"
#include "Tile.h"
#include "Treasure.h"
#include "VariableManager.h"
#include "WarSystem.h"
#include "Zone.h"
#include "ZoneUtil.h"
#include "skill/EffectHarpoonBomb.h"
#include "skill/SummonGroundElemental.h"

#define __MONSTER_FIGHTING__
extern bool isPotentialEnemy(Monster* pMonster, Creature* pCreature);
extern void countResurrectItem();

// #define __PROFILE_MONSTER__

#ifdef __PROFILE_MONSTER__
#define __BEGIN_PROFILE_MONSTER(name) beginProfileEx(name);
#define __END_PROFILE_MONSTER(name) endProfileEx(name);
#else
#define __BEGIN_PROFILE_MONSTER(name) ((void)0);
#define __END_PROFILE_MONSTER(name) ((void)0);
#endif

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// const uint g_pVariableManager->getPremiumItemProbePercent() = 220;

bool isLottoWinning();

////////////////////////////////////////////////////////////////////////////////
//
// constructor
//
////////////////////////////////////////////////////////////////////////////////
MonsterManager::MonsterManager(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);
    m_pZone = pZone;

    Assert(g_pCastleInfoManager != NULL);
    m_CastleZoneID = 0;
    g_pCastleInfoManager->getCastleZoneID(m_pZone->getZoneID(), m_CastleZoneID);

    m_nEventMonster = 0;
    m_pEventMonsterInfo = NULL;

    getCurrentTime(m_RegenTime);

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// destructor
//
////////////////////////////////////////////////////////////////////////////////
MonsterManager::~MonsterManager()

{
    __BEGIN_TRY

    SAFE_DELETE(m_pEventMonsterInfo);

    __END_CATCH_NO_RETHROW
}

////////////////////////////////////////////////////////////////////////////////
// load from database
////////////////////////////////////////////////////////////////////////////////
void MonsterManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    Result* pResult = NULL;
    string text, eventText;

    m_RICE_CAKE_PROB_RATIO[0] = 100;
    m_RICE_CAKE_PROB_RATIO[1] = 33;
    m_RICE_CAKE_PROB_RATIO[2] = 33;
    m_RICE_CAKE_PROB_RATIO[3] = 33;
    m_RICE_CAKE_PROB_RATIO[4] = 1;
    m_SumOfCakeRatio = 0;

    for (int i = 0; i < 5; i++)
        m_SumOfCakeRatio += m_RICE_CAKE_PROB_RATIO[i];

    
    bool bReload = false;
    unordered_map<SpriteType_t, MonsterCounter*>::iterator iMC = m_Monsters.begin();
    while (iMC != m_Monsters.end()) {
        MonsterCounter* pMC = iMC->second;
        SAFE_DELETE(pMC);

        iMC++;

        
        bReload = true;
    }

    BEGIN_DB {
        
        ZoneID_t zoneID = m_pZone->getZoneID();
        if (m_pZone->isDynamicZone()) {
            DynamicZone* pDynamicZone = m_pZone->getDynamicZone();
            Assert(pDynamicZone != NULL);

            zoneID = pDynamicZone->getTemplateZoneID();
        }

        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pResult = pStmt->executeQuery("SELECT MonsterList, EventMonsterList from ZoneInfo WHERE ZoneID=%d", zoneID);

        if (pResult->getRowCount() <= 0) {
            SAFE_DELETE(pStmt);
            
            return;
        }

        pResult->next();
        text = pResult->getString(1);
        eventText = pResult->getString(2);

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)


    parseMonsterList(text, bReload);
    parseEventMonsterList(eventText, bReload);

    __END_CATCH
}

void MonsterManager::parseMonsterList(const string& text, bool bReload)

{
    if (text.size() <= 0)
        return;

    //--------------------------------------------------------------------------------
    //
    
    
    //
    // (MonsterType1,#Monster1) (MonsterType2,#Monter2)(..,..)
    // i            j         k i            j        k
    //
    //--------------------------------------------------------------------------------

    size_t i = 0, j = 0, k = 0;

    do {
        // parse string
        i = text.find_first_of('(', k);
        j = text.find_first_of(',', i + 1);
        k = text.find_first_of(')', j + 1);

        if (i == string::npos || j == string::npos || k == string::npos || i > j || j > k)
            break;

        
        uint monsterType = atoi(text.substr(i + 1, j - i - 1).c_str());
        uint maxMonsters = atoi(text.substr(j + 1, k - j - 1).c_str());

        Assert(maxMonsters > 0);

        
        const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(monsterType);
        SpriteType_t spriteType = pMonsterInfo->getSpriteType();

        
        unordered_map<SpriteType_t, MonsterCounter*>::iterator itr = m_Monsters.find(spriteType);

        if (itr != m_Monsters.end()) {
            WORD CurrentMaxCount = itr->second->getMaxMonsters();
            WORD NewMaxCount = CurrentMaxCount + maxMonsters;
            itr->second->setMaxMonsters(NewMaxCount);
        } else {
            
            MonsterCounter* pMonsterCounter = new MonsterCounter(monsterType, maxMonsters, 0);

            
            m_Monsters[spriteType] = pMonsterCounter;
        }

        //--------------------------------------------------------------------------------
        
        //--------------------------------------------------------------------------------
        if (!bReload) 
        {
            for (uint m = 0; m < maxMonsters; m++) {
                
                ZoneCoord_t x, y;
                if (!findPosition(monsterType, x, y)) {
                    Assert(false);
                    return;
                }

                
                Monster* pMonster = new Monster(monsterType);

                ////////////////////////////////////////////////////////////////////////////////
                
                 
                ///////////////////////////////////////////////////////////////////////////
                Assert(pMonster != NULL);

                try {
                    m_pZone->addCreature(pMonster, x, y, Directions(rand() & 0x07));
                } catch (EmptyTileNotExistException&) {
                    
                    SAFE_DELETE(pMonster);
                }
            }
        }
    } while (k < text.size() - 1);
}

void MonsterManager::parseEventMonsterList(const string& text, bool bReload)

{
    if (text.size() <= 0)
        return;

    //--------------------------------------------------------------------------------
    //
    
    
    //
    // (MonsterType1,#Monster1,RegenDelay) (MonsterType2,#Monter2,RegenDelay)(..,..)
    // i            j         k          l i            j        k          l
    //
    //--------------------------------------------------------------------------------

    size_t i = 0, j = 0, k = 0, l = 0, m = 0, n = 0;

    do {
        // parse string
        i = text.find_first_of('(', l);
        j = text.find_first_of(',', i + 1);
        k = text.find_first_of(',', j + 1);
        l = text.find_first_of(')', k + 1);
        m = text.find_first_of(',', k + 1);
        n = text.find_first_of(',', m + 1);

        if (i == string::npos || j == string::npos || k == string::npos || l == string::npos || i > j || j > k || k > l)
            break;

        int tx = -1;
        int ty = -1;
        if (m != string::npos && n != string::npos && k < m && m < n && n < l) {
            tx = atoi(text.substr(m + 1, n - m - 1).c_str());
            ty = atoi(text.substr(n + 1, l - n - 1).c_str());
            cout << "x : " << tx << endl;
            cout << "y : " << ty << endl;
            l = m;
        }

        
        uint monsterType = atoi(text.substr(i + 1, j - i - 1).c_str());
        uint maxMonsters = atoi(text.substr(j + 1, k - j - 1).c_str());
        uint regenDelay = atoi(text.substr(k + 1, l - k - 1).c_str());

        Assert(maxMonsters > 0);

        //--------------------------------------------------------------------------------
        
        //--------------------------------------------------------------------------------
        if (!bReload) 
        {
            // cout << "[MM] load EventMonsterList: [" << m_pZone->getZoneID() << "] mtype=" << monsterType
            //	<< ", maxMonsters=" << maxMonsters
            //	<< ", regenDelay=" << regenDelay << endl;
            if (m_pEventMonsterInfo == NULL) {
                m_pEventMonsterInfo = new vector<EventMonsterInfo>;
                // m_pEventMonsterInfo->resize( maxMonsters );
            }

            /*
            if (m_pEventMonsterInfo->size() + maxMonsters < m_pEventMonsterInfo->capacity())
            {
                m_pEventMonsterInfo->resize( m_pEventMonsterInfo->size() + maxMonsters );
            }
            */

            for (uint m = 0; m < maxMonsters; m++) {
                if (g_pVariableManager->isActiveChiefMonster()) {
                    
                    ZoneCoord_t x, y;
                    if (tx != -1) {
                        x = tx;
                        y = ty;
                    } else if (!findPosition(monsterType, x, y)) {
                        Assert(false);
                        return;
                    }

                    
                    Monster* pMonster = new Monster(monsterType);
                    Assert(pMonster != NULL);

                    pMonster->setEventMonsterIndex(m_nEventMonster++);

                    EventMonsterInfo info;
                    info.monsterType = monsterType;
                    info.regenDelay = regenDelay;
                    info.bExist = true;
                    info.x = tx;
                    info.y = ty;

                    m_pEventMonsterInfo->push_back(info);

                    try {
                        m_pZone->addCreature(pMonster, x, y, Directions(rand() & 0x07));

                        
                         
                    } catch (EmptyTileNotExistException&) {
                        
                        SAFE_DELETE(pMonster);
                    }
                } else {
                    m_nEventMonster++;

                    EventMonsterInfo info;
                    info.monsterType = monsterType;
                    info.regenDelay = regenDelay;
                    getCurrentTime(info.regenTime);
                    info.bExist = false;

                    m_pEventMonsterInfo->push_back(info);
                }
            }
        }
    } while (l < text.size() - 1);
}

////////////////////////////////////////////////////////////////////////////////
void MonsterManager::addCreature(Creature* pCreature)

{
    __BEGIN_TRY

    Monster* pMonster = dynamic_cast<Monster*>(pCreature);

    
    CreatureManager::addCreature(pMonster);

    
    if (m_pEventMonsterInfo != NULL && pMonster->isEventMonster()) {
        uint index = pMonster->getEventMonsterIndex();

        if (index < m_pEventMonsterInfo->size()) {
            EventMonsterInfo& info = (*m_pEventMonsterInfo)[index];

            getCurrentTime(info.regenTime);
            info.regenTime.tv_sec += info.regenDelay;

            info.bExist = true;
        }

        // cout << "[MM] add EventMonster: [" << pMonster->getEventMonsterIndex()
        //	<< "] = " << info.monsterType << ", delay = " << info.regenDelay << endl;
        return;
    }

    
    unordered_map<SpriteType_t, MonsterCounter*>::iterator itr = m_Monsters.find(pMonster->getSpriteType());

    if (itr == m_Monsters.end()) {
        StringStream msg;
        msg << "       .\n"
            << "  [" << m_pZone->getZoneID() << "].\n"
            << "    [" << pMonster->getMonsterType() << "].\n";
        // throw Error(msg.toString());
    } else {
        
        itr->second->addMonster();
    }

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void MonsterManager::deleteCreature(ObjectID_t creatureID)
// NoSuchElementException , Error)
{
    __BEGIN_TRY

    
    unordered_map<ObjectID_t, Creature*>::iterator itr = m_Creatures.find(creatureID);

    if (itr == m_Creatures.end()) {
        cerr << "MonsterManager::deleteCreature() : NoSuchElementException" << endl;

        
        // by sigi. 2002.5.9
        

        return;
    }

    Monster* pMonster = dynamic_cast<Monster*>(itr->second);

    
    
    
    m_Creatures.erase(itr);


    
    if (m_pEventMonsterInfo != NULL && pMonster->isEventMonster() && pMonster->getMonsterType() != 764) {
        uint index = pMonster->getEventMonsterIndex();

        if (index < m_pEventMonsterInfo->size()) {
            EventMonsterInfo& info = (*m_pEventMonsterInfo)[index];
            info.bExist = false;
            // cout << "[MM] delete EventMonster: [" << pMonster->getEventMonsterIndex()
            //	<< "] = " << info.monsterType << endl;
        }

        return;
    }

    
    unordered_map<SpriteType_t, MonsterCounter*>::iterator itr2 = m_Monsters.find(pMonster->getSpriteType());

    if (itr2 == m_Monsters.end()) {
        cerr << "MonsterManager::deleteCreature() : NoSuchElementException" << endl;
        
    } else {
        
        itr2->second->deleteMonster();
    }


    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void MonsterManager::addPotentialEnemy(Monster* pAttackedMonster, Creature* pCreature)

{
    __BEGIN_TRY

    // cout << "MonsterManager::addPotentialEnemy()" << endl;

    unordered_map<ObjectID_t, Creature*>::const_iterator itr = m_Creatures.begin();

    for (; itr != m_Creatures.end(); itr++) {
        Creature* pMonsterCreature = itr->second;

        
        Distance_t dist = pMonsterCreature->getDistance(pCreature->getX(), pCreature->getY());

        if (dist <= pMonsterCreature->getSight()
            
            && pMonsterCreature != pAttackedMonster) {
            Monster* pMonster = dynamic_cast<Monster*>(pMonsterCreature);
            // cout << "addPotentialEnemy: " << pMonster->getName().c_str() << endl;
            pMonster->addPotentialEnemy(pCreature);
        }
    }


    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void MonsterManager::addEnemy(Monster* pAttackedMonster, Creature* pCreature)

{
    __BEGIN_TRY

    // cout << "MonsterManager::addEnemy()" << endl;

    unordered_map<ObjectID_t, Creature*>::const_iterator itr = m_Creatures.begin();

    for (; itr != m_Creatures.end(); itr++) {
        Creature* pMonsterCreature = itr->second;

        
        Distance_t dist = pMonsterCreature->getDistance(pCreature->getX(), pCreature->getY());

        if (dist <= pMonsterCreature->getSight()
            
            && pMonsterCreature != pAttackedMonster) {
            Monster* pMonster = dynamic_cast<Monster*>(pMonsterCreature);
            // cout << "addEnemy: " << pMonster->getName().c_str() << endl;
            pMonster->addEnemy(pCreature);
        }
    }


    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void MonsterManager::processCreatures()

{
    __BEGIN_TRY

    //	__BEGIN_PROFILE_MONSTER("MM_PROCESS_CREATURES");

    Timeval currentTime;
    getCurrentTime(currentTime);

    try {
        unordered_map<ObjectID_t, Creature*>::iterator before = m_Creatures.end();
        unordered_map<ObjectID_t, Creature*>::iterator current = m_Creatures.begin();

        while (current != m_Creatures.end()) {
            Creature* pCreature = current->second;

            Assert(pCreature != NULL);

            __BEGIN_PROFILE_MONSTER("MM_EFFECTMANAGER");

            pCreature->getEffectManager()->heartbeat(currentTime);

            __END_PROFILE_MONSTER("MM_EFFECTMANAGER");

            if (pCreature->isAlive()) {
                 
                __BEGIN_PROFILE_MONSTER("MM_CREATURE_ACT");
                pCreature->act(currentTime);
                before = current++;
                __END_PROFILE_MONSTER("MM_CREATURE_ACT");
            } else {
                Monster* pMonster = dynamic_cast<Monster*>(pCreature);
                Assert(pMonster != NULL);

                /*if ( pMonster->getMonsterType() == 371 ||
                     pMonster->getMonsterType() == 372 ||
                     pMonster->getMonsterType() == 373 ||
                     pMonster->getMonsterType() == 374 ||
                     pMonster->getMonsterType() == 375 ||
                     pMonster->getMonsterType() == 376)
                    return;
                else
                {*/

                if (pMonster->isEventMonster()) // by sigi. 2002.10.14
                {
                    if (m_pEventMonsterInfo != NULL) {
                        uint index = pMonster->getEventMonsterIndex();

                        if (index < m_pEventMonsterInfo->size()) {
                            EventMonsterInfo& info = (*m_pEventMonsterInfo)[index];
                            info.bExist = false;

                            // cout << "[MM] dead EventMonster: [" << pMonster->getEventMonsterIndex()
                            //	<< "] = " << info.monsterType << endl;
                        }
                    }
                } else {
                    
                    unordered_map<SpriteType_t, MonsterCounter*>::iterator itr =
                        m_Monsters.find(pMonster->getSpriteType());

                    if (itr == m_Monsters.end()) {
                        // cerr << "MonsterManager::processCreatures() : NoSuchElementException" << endl;
                        
                    } else {
                        
                        itr->second->deleteMonster();
                    }
                }

                __BEGIN_PROFILE_MONSTER("MM_CREATURE_DEADACTION");
                
                pMonster->actDeadAction();
                __END_PROFILE_MONSTER("MM_CREATURE_DEADACTION");

                __BEGIN_PROFILE_MONSTER("MM_KILL_CREATURE");
                
                killCreature(pMonster);
                __END_PROFILE_MONSTER("MM_KILL_CREATURE");

                
                
                if (before == m_Creatures.end()) {
                    m_Creatures.erase(current);
                    current = m_Creatures.begin();
                } else {
                    m_Creatures.erase(current);
                    current = before;
                    current++;
                }
                //}
            }
        }

        
        
        
        
        
        
        if (m_RegenTime < currentTime) {
            __BEGIN_PROFILE_MONSTER("MM_REGENERATE_CREATURES");

            regenerateCreatures();

            m_RegenTime.tv_sec = currentTime.tv_sec + 5; 
            m_RegenTime.tv_usec = currentTime.tv_usec;

            __END_PROFILE_MONSTER("MM_REGENERATE_CREATURES");
        }

        
        // regenerateCreatures();
    } catch (Throwable& t) {
        filelog("MonsterManagerBug.log", "ProcessCreatureBug : %s", t.toString().c_str());
        // cerr << t.toString() << endl;
    }

    //	__END_PROFILE_MONSTER("MM_PROCESS_CREATURES");

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void MonsterManager::regenerateCreatures()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    
    if (m_pZone->isHolyLand()) {
        
        if (g_pWarSystem->hasActiveRaceWar())
            return;

        
        if (m_CastleZoneID != 0 && g_pWarSystem->hasCastleActiveWar(m_CastleZoneID)) {
            CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(m_CastleZoneID);
            if (pCastleInfo != NULL) {
                GuildID_t OwnerGuildID = pCastleInfo->getGuildID();

                
                if (OwnerGuildID != SlayerCommon && OwnerGuildID != VampireCommon && OwnerGuildID != OustersCommon) {
                    return;
                }
            }
        }
    }

    
    ZoneID_t zoneID = m_pZone->getZoneID();
    if (zoneID == 1131 || zoneID == 1132 || zoneID == 1133 || zoneID == 1134) {
        if (!g_pSweeperBonusManager->isAble(zoneID))
            return;
    }

    unordered_map<SpriteType_t, MonsterCounter*>::iterator itr = m_Monsters.begin();
    for (; itr != m_Monsters.end(); itr++) {
        MonsterCounter* pCounter = itr->second;

        
        while (pCounter->getCurrentMonsters() < pCounter->getMaxMonsters()) {
            SpriteType_t SpriteType = itr->first;
            MonsterType_t monsterType = 0;

            vector<MonsterType_t> RegenVector = g_pMonsterInfoManager->getMonsterTypeBySprite(SpriteType);
            Assert(RegenVector.size() > 0);

            monsterType = RegenVector[rand() % RegenVector.size()];

            
            ZoneCoord_t x, y;
            if (!findPosition(monsterType, x, y)) {
                Assert(false);
                return;
            }

            
            Monster* pMonster = new Monster(monsterType);
            Assert(pMonster != NULL);

            /////////////////////////////////////////////////////////////////////
            
            
             
            /////////////////////////////////////////////////////////////////////

            try {
                m_pZone->addCreature(pMonster, x, y, Directions(rand() % 8));
            } catch (EmptyTileNotExistException&) {
                
                SAFE_DELETE(pMonster);
            }
        }
    }

    if (g_pVariableManager->isActiveChiefMonster() && m_pEventMonsterInfo != NULL) {
        Timeval currentTime;
        getCurrentTime(currentTime);

        // cout << "regenCheck [" << m_pZone->getZoneID() <<"] EventMonsterNum = "
        //	<< m_pEventMonsterInfo->size() << " : ";

        for (uint i = 0; i < m_pEventMonsterInfo->size(); i++) {
            EventMonsterInfo& info = (*m_pEventMonsterInfo)[i];

            if (!info.bExist && currentTime >= info.regenTime) {
                // cout << i << " ";
                MonsterType_t monsterType = info.monsterType;

                
                ZoneCoord_t x, y;
                if (info.x != -1) {
                    x = info.x;
                    y = info.y;
                } else if (!findPosition(monsterType, x, y)) {
                    Assert(false);
                    return;
                }

                
                Monster* pMonster = new Monster(monsterType);
                Assert(pMonster != NULL);

                pMonster->setEventMonsterIndex(i);

                try {
                    m_pZone->addCreature(pMonster, x, y, Directions(rand() % 8));

                    
                     
                } catch (EmptyTileNotExistException&) {
                    
                    SAFE_DELETE(pMonster);
                }
            }

            /*
            else
            {
                if (!info.bExist)
                    cout << "f ";
                else
                    cout << "t ";
            }
            */
        }

        // cout << endl;
    }

    __END_DEBUG
    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool MonsterManager::findPosition(MonsterType_t monsterType, ZoneCoord_t& RX, ZoneCoord_t& RY) const

{
    __BEGIN_TRY

    const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(monsterType);

    int count = 0;

     

    
    while (true) {
        const BPOINT& pt = m_pZone->getRandomMonsterRegenPosition();

        Tile& rTile = m_pZone->getTile(pt.x, pt.y);

        
        
        
        if (!rTile.isBlocked(pMonsterInfo->getMoveMode()) && !rTile.hasPortal() &&
            !(m_pZone->getZoneLevel(pt.x, pt.y) & SAFE_ZONE)) {
            RX = pt.x;
            RY = pt.y;
            return true;
        }

        if (++count >= 300) {
            cerr << "MonsterManager::findPosition() : Max Count Exceeded" << endl;
            throw("MonsterManager::findPosition() : Max Count Exceeded");
        }
    }

    
    return false;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void MonsterManager::killCreature(Creature* pDeadCreature)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Assert(pDeadCreature->isDead());

    Zone* pZone = pDeadCreature->getZone();
    Assert(m_pZone == pZone);

    Monster* pDeadMonster = dynamic_cast<Monster*>(pDeadCreature);
    ZoneCoord_t cx = pDeadMonster->getX();
    ZoneCoord_t cy = pDeadMonster->getY();

    
    PrecedenceTable* pTable = pDeadMonster->getPrecedenceTable();

    pTable->compute();

    if (pTable->getHostName() == "") {
        
        pDeadMonster->setTreasure(false);
    } else {
        pDeadMonster->setHostName(pTable->getHostName());
        pDeadMonster->setHostPartyID(pTable->getHostPartyID());
    }

    
    GCAddEffect gcAddEffect;
    gcAddEffect.setObjectID(pDeadCreature->getObjectID());
    gcAddEffect.setEffectID(Effect::EFFECT_CLASS_COMA);
    gcAddEffect.setDuration(0);
    pZone->broadcastPacket(cx, cy, &gcAddEffect);

    
    Tile& tile = m_pZone->getTile(cx, cy);
    tile.deleteCreature(pDeadMonster->getObjectID());

    
    if (m_pZone->isDynamicZone()) {
        DynamicZone* pDynamicZone = m_pZone->getDynamicZone();
        Assert(pDynamicZone != NULL);

        if (pDynamicZone->getTemplateZoneID() == 4001) {
            
            DynamicZoneGateOfAlter* pGateOfAlter = dynamic_cast<DynamicZoneGateOfAlter*>(pDynamicZone);
            Assert(pGateOfAlter != NULL);

            pGateOfAlter->removeEffect(cx, cy);
        }
    }

    
    if (pDeadMonster->getMonsterType() == GROUND_ELEMENTAL_TYPE) {
        GCDeleteObject* pGCDO = new GCDeleteObject;
        pGCDO->setObjectID(pDeadMonster->getObjectID());

        EffectPacketSend* pEffectPacketSend =
            new EffectPacketSend(pDeadMonster->getZone(), pDeadMonster->getX(), pDeadMonster->getY());
        pEffectPacketSend->setPacket(pGCDO);
        
        pEffectPacketSend->setDeadline(10);
        pDeadMonster->getZone()->registerObject(pEffectPacketSend);
        pDeadMonster->getZone()->addEffect(pEffectPacketSend);

        SAFE_DELETE(pDeadMonster);
        return;
    } else if (pDeadMonster->getMonsterType() == 764) {
        
        GCDeleteObject* pGCDO = new GCDeleteObject;
        pGCDO->setObjectID(pDeadMonster->getObjectID());

        EffectPacketSend* pEffectPacketSend =
            new EffectPacketSend(pDeadMonster->getZone(), pDeadMonster->getX(), pDeadMonster->getY());
        pEffectPacketSend->setPacket(pGCDO);
        
        pEffectPacketSend->setDeadline(10);
        pDeadMonster->getZone()->registerObject(pEffectPacketSend);
        pDeadMonster->getZone()->addEffect(pEffectPacketSend);

        Monster* pNewMonster = new Monster(765);
        pNewMonster->setClanType(pDeadMonster->getClanType());
        pNewMonster->setEventMonsterIndex(pDeadMonster->getEventMonsterIndex());
        pNewMonster->setName(pDeadMonster->getName());
        pDeadMonster->getZone()->addCreature(pNewMonster, pDeadMonster->getX(), pDeadMonster->getY(),
                                             pDeadMonster->getDir());

        SAFE_DELETE(pDeadMonster);
        return;
    } else if (pDeadMonster->getMonsterType() == 793 || pDeadMonster->getMonsterType() == 794 ||
               pDeadMonster->getMonsterType() == 795) {
        
        
        GCDeleteObject gcDO;
        gcDO.setObjectID(pDeadMonster->getObjectID());
        pDeadMonster->getZone()->broadcastPacket(pDeadMonster->getX(), pDeadMonster->getY(), &gcDO);

        SAFE_DELETE(pDeadMonster);
        return;
    }

    
    MonsterCorpse* pMonsterCorpse = new MonsterCorpse(pDeadMonster);
    pMonsterCorpse->setHostName(pDeadMonster->getHostName());
    pMonsterCorpse->setHostPartyID(pDeadMonster->getHostPartyID());
    pMonsterCorpse->setQuestHostName(pTable->getQuestHostName());
    pMonsterCorpse->setLevel((int)(pDeadMonster->getLevel()));
    pMonsterCorpse->setExp((Exp_t)computeCreatureExp(pDeadMonster, 100));
    pMonsterCorpse->setLastKiller(pDeadMonster->getLastKiller());

    
    addItem(pDeadMonster, pMonsterCorpse);

    // by sigi. 2002.12.12
    addCorpseToZone(pMonsterCorpse, m_pZone, cx, cy);

    if (pDeadMonster->isFlag(Effect::EFFECT_CLASS_HARPOON_BOMB)) {
        EffectHarpoonBomb* pEffect =
            dynamic_cast<EffectHarpoonBomb*>(pDeadMonster->findEffect(Effect::EFFECT_CLASS_HARPOON_BOMB));
        if (pEffect != NULL) {
            EffectHarpoonBomb* pZoneEffect =
                new EffectHarpoonBomb(m_pZone, pMonsterCorpse->getX(), pMonsterCorpse->getY());
            pZoneEffect->setDamage(pEffect->getDamage());
            pZoneEffect->setUserObjectID(pEffect->getUserObjectID());
            pZoneEffect->setNextTime(pEffect->getNextTime());
            pZoneEffect->setDeadline(pEffect->getRemainDuration());
            //			pDeadMonster->deleteEffect( Effect::EFFECT_CLASS_HARPOON_BOMB );
            pEffect->setDeadline(0);
            m_pZone->registerObject(pZoneEffect);
            m_pZone->getTile(pMonsterCorpse->getX(), pMonsterCorpse->getY()).addEffect(pZoneEffect);
            m_pZone->addEffect(pZoneEffect);
        }
    }

    
    GCCreatureDied gcCreatureDied;
    gcCreatureDied.setObjectID(pDeadMonster->getObjectID());
    m_pZone->broadcastPacket(cx, cy, &gcCreatureDied);

    
    if (pDeadMonster->isMaster()) {
        // MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo( pDeadMonster->getMonsterType() );
        MasterLairInfo* pMasterLairInfo = g_pMasterLairInfoManager->getMasterLairInfo(pZone->getZoneID());

        if (pMasterLairInfo != NULL && pMasterLairInfo->getMasterMonsterType() == pDeadMonster->getMonsterType()) {
            GCSay gcSay;
            gcSay.setObjectID(pDeadMonster->getObjectID());
            gcSay.setColor(MASTER_SAY_COLOR);

            if (pDeadMonster->getLastHitCreatureClass() == Creature::CREATURE_CLASS_SLAYER) {
                gcSay.setMessage(pMasterLairInfo->getRandomMasterDeadSlayerSay());
            } else {
                gcSay.setMessage(pMasterLairInfo->getRandomMasterDeadVampireSay());
            }

            if (!gcSay.getMessage().empty())
                pZone->broadcastPacket(cx, cy, &gcSay);
        }
    }

    if (pDeadMonster->getMonsterType() == 717) {
        GCSay gcSay;
        gcSay.setObjectID(pDeadMonster->getObjectID());
        gcSay.setColor(MASTER_SAY_COLOR);

        gcSay.setMessage(g_pStringPool->getString(345));
        cout << gcSay.getMessage() << endl;
        pZone->broadcastPacket(cx, cy, &gcSay);
    } else if (pDeadMonster->getMonsterType() == 723) {
        GCSay gcSay;
        gcSay.setObjectID(pDeadMonster->getObjectID());
        gcSay.setColor(MASTER_SAY_COLOR);

        gcSay.setMessage(g_pStringPool->getString(360));
        cout << gcSay.getMessage() << endl;
        pZone->broadcastPacket(cx, cy, &gcSay);
    }

    
    SAFE_DELETE(pDeadMonster);

    __END_DEBUG
    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// addCreature
//


////////////////////////////////////////////////////////////////////////////////
void MonsterManager::addMonsters(ZoneCoord_t x, ZoneCoord_t y, MonsterType_t monsterType, int num,
                                 const SUMMON_INFO& summonInfo, list<Monster*>* pSummonedMonsters) {
    TPOINT pt;

    ClanType_t clanType = CLAN_VAMPIRE_MONSTER; // default

    
    if (summonInfo.clanType == SUMMON_INFO::CLAN_TYPE_RANDOM_GROUP ||
        summonInfo.clanType == SUMMON_INFO::CLAN_TYPE_GROUP) {
        clanType = summonInfo.clanID; // rand()%90+2;
    }

    
    for (int i = 0; i < num; i++) {
        pt = findSuitablePosition(m_pZone, x, y, Creature::MOVE_MODE_WALKING);

        
        if (pt.x == -1 || (m_pZone->getZoneLevel(pt.x, pt.y) & SAFE_ZONE)) {
            return;
        }

        Monster* pMonster = NULL;

        
        try {
            pMonster = new Monster(monsterType);
            

            
            pMonster->setTreasure(summonInfo.hasItem);

            ////////////////////////////////////////////////////////////////////////////////
            
            
             
            ///////////////////////////////////////////////////////////////////////////

            Assert(pMonster != NULL);

            if (summonInfo.regenType == REGENTYPE_PORTAL) {
                
                pMonster->removeFlag(Effect::EFFECT_CLASS_HIDE);
                pMonster->removeFlag(Effect::EFFECT_CLASS_INVISIBILITY);
                pMonster->removeFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT);

                pMonster->setFlag(Effect::EFFECT_CLASS_VAMPIRE_PORTAL);
                pMonster->setMoveMode(Creature::MOVE_MODE_WALKING);
            }

            if (summonInfo.initHPPercent != 0) {
                int currentHP = pMonster->getHP(ATTR_CURRENT);
                int MaxHP = currentHP * 100 / summonInfo.initHPPercent;
                pMonster->setHP(MaxHP, ATTR_MAX);
            }

        } catch (OutOfBoundException& t) {
            filelog("MonsterManagerBug.log", "addMonsters : %s", t.toString().c_str());
            SAFE_DELETE(pMonster);
            return;
        } catch (NoSuchElementException& t) {
            filelog("MonsterManagerBug.log", "addMonsters : %s", t.toString().c_str());
            SAFE_DELETE(pMonster);
            return;
        }


        try {
            m_pZone->addCreature(pMonster, pt.x, pt.y, Directions(rand() % 8));


            // SUMMON_INFO
            if (summonInfo.clanType == SUMMON_INFO::CLAN_TYPE_RANDOM_EACH) {
                pMonster->setClanType(rand() % 90 + 2);
            } else {
                pMonster->setClanType(clanType);
            }

            // cout << "clanType=" << (int)pMonster->getClanType() << endl;

            //
            if (summonInfo.canScanEnemy) {
                pMonster->setScanEnemy();

                m_pZone->monsterScan(pMonster, pt.x, pt.y, pMonster->getDir());
            } else if (summonInfo.scanEnemy) {
                m_pZone->monsterScan(pMonster, pt.x, pt.y, pMonster->getDir());
            }

            if (pSummonedMonsters != NULL) {
                pSummonedMonsters->push_back(pMonster);
            }

        } catch (EmptyTileNotExistException&) {
            
            SAFE_DELETE(pMonster);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void MonsterManager::addItem(Monster* pDeadMonster, MonsterCorpse* pMonsterCorpse)

{
    __BEGIN_TRY

    if (pDeadMonster->getMonsterType() == 734) {
        if (pDeadMonster->getZoneID() >= 1500 && pDeadMonster->getZoneID() <= 1506) {
            ItemType_t iType = pDeadMonster->getZoneID() - min((int)pDeadMonster->getZoneID(), 1501);
            Item* pItem =
                g_pItemFactoryManager->createItem(Item::ITEM_CLASS_CASTLE_SYMBOL, iType, list<OptionType_t>());
            pMonsterCorpse->setZone(pDeadMonster->getZone());
            pMonsterCorpse->addTreasure(pItem);
        }
    }

    
    if (pDeadMonster->getQuestItem() != NULL) {
        pMonsterCorpse->addTreasure(pDeadMonster->getQuestItem());
        pDeadMonster->setQuestItem(NULL);
    }

    if (g_pVariableManager->getVariable(PREMIUM_TRIAL_EVENT) != 0 && pDeadMonster->getMonsterType() == 705) {
        if (rand() % 100 < 30) {
            int Num = 5 + (rand() % 5); // 5~9
            for (int i = 0; i < Num; ++i) {
                Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_LUCKY_BAG, 3, list<OptionType_t>());
                pMonsterCorpse->addTreasure(pItem);
            }
        }
    }

    
    // by sigi. 2002.9.2
    if (!pDeadMonster->hasTreasure())
        return;

    MonsterType_t MonsterType = pDeadMonster->getMonsterType();
    const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(MonsterType);
    TreasureList* pTreasureList = NULL;

    //----------------------------------------------------------------------
    
    
    //----------------------------------------------------------------------
    bool isHarvestFestivalItemAppeared = false;
    int PartialSumOfCakeRatio = 0;
    int itemBonusPercent = 0;

    if (g_pVariableManager->getHarvestFestivalItemRatio() > 0 &&
        rand() % g_pVariableManager->getHarvestFestivalItemRatio() == 0) {
        
        ITEM_TEMPLATE ricecake_template;
        ricecake_template.NextOptionRatio = 0;

        bool bOK = false;
        int EventSelector = rand() % m_SumOfCakeRatio;

        for (int i = 0; i < 5; i++) {
            PartialSumOfCakeRatio += m_RICE_CAKE_PROB_RATIO[i];

            
            if (EventSelector < PartialSumOfCakeRatio) {
                if (i == 0) {
                    if (pDeadMonster->getLastHitCreatureClass() == Creature::CREATURE_CLASS_SLAYER) {
                        ricecake_template.ItemClass = Item::ITEM_CLASS_POTION;
                        ricecake_template.ItemType = 11;
                    } else {
                        ricecake_template.ItemClass = Item::ITEM_CLASS_SERUM;
                        ricecake_template.ItemType = 5;
                    }
                } else {
                    
                    ricecake_template.ItemClass = Item::ITEM_CLASS_EVENT_STAR;
                    ricecake_template.ItemType = i + 7;
                }
                bOK = true;
                break;
            }
        }

        
        //<< "(" << ricecake_template.ItemClass << " " << ricecake_template.ItemType << ")" << endl;

        if (bOK) {
            Item* pItem = g_pItemFactoryManager->createItem(ricecake_template.ItemClass, ricecake_template.ItemType,
                                                            ricecake_template.OptionType);

            Assert(pItem != NULL);

            pMonsterCorpse->addTreasure(pItem);

            isHarvestFestivalItemAppeared = true;
        }
    }

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    int fireCrackerRatio = g_pVariableManager->getVariable(CHRISTMAS_FIRE_CRACKER_RATIO);
    if (fireCrackerRatio > 0) {
        int value = rand() % 10000;
        if (value < fireCrackerRatio) {
            
            ItemType_t fireCrackerType = value % 3;

            
            list<OptionType_t> optionType;
            Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_ETC, fireCrackerType, optionType);

            
            pMonsterCorpse->addTreasure(pItem);
        }
    }

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    int treePartRatio = g_pVariableManager->getVariable(CHRISTMAS_TREE_PART_RATIO);
    if (treePartRatio > 0) {
        int value = rand() % 10000;
        if (value < treePartRatio) {
            
            ItemType_t treeItemType = rand() % 12;

            
            list<OptionType_t> optionType;
            Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_TREE, treeItemType, optionType);

            
            pMonsterCorpse->addTreasure(pItem);
        }
    }
    int value = 0;
    int PayZonecheckIn = 0;
    ItemType_t treeItemType = 3;
    if (pDeadMonster->getMonsterType() == 765) {
        treeItemType = 6;
        if (pDeadMonster->getZoneID() >= 1402) {
            value = rand() % 250;
            PayZonecheckIn = 50;
            if (value < PayZonecheckIn) {
                list<OptionType_t> optionType;
                Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_STAR, treeItemType, optionType);
                pMonsterCorpse->addTreasure(pItem);
            }
        }
        treeItemType = 3;
        for (int l = 0; l < 3; l++) {
            value = rand() % 10000;
            PayZonecheckIn = 3000;
            if (value < PayZonecheckIn) {
                list<OptionType_t> optionType;
                Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_MOON_CARD, treeItemType, optionType);
                pMonsterCorpse->addTreasure(pItem);
            }
        }
    }
    // end

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    int giftBoxRatio = g_pVariableManager->getVariable(CHRISTMAS_GIFT_BOX_RATIO);
    if (giftBoxRatio > 0) {
        int value = rand() % 10000;
        if (value < giftBoxRatio) {
            
            list<OptionType_t> optionType;
            Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_EVENT_GIFT_BOX, 0, optionType);

            
            pMonsterCorpse->addTreasure(pItem);
        }
    }

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    
    
    
    //----------------------------------------------------------------------
    if (g_pVariableManager->isEventGiftBox()) {
        if (m_pZone != NULL) {
            Creature* pCreature = m_pZone->getCreature(pDeadMonster->getLastKiller());

            if (pCreature != NULL && pCreature->isPC()) {
                PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

                if (pPC != NULL) {
                    Item* pItem = getGiftBoxItem(getGiftBoxKind(pPC, pDeadMonster));

                    
                    if (pItem != NULL)
                        pMonsterCorpse->addTreasure(pItem);
                }
            }
        }
    }


    
    
    
    
    // by sigi. 2002.10.14
    
    
    Creature* pItemOwnerCreature = m_pZone->getPCManager()->getCreature(pDeadMonster->getHostName());

    Creature::CreatureClass ownerCreatureClass;

    int luckLevel = 0;
    if (pItemOwnerCreature != NULL) {
        ownerCreatureClass = pItemOwnerCreature->getCreatureClass();
        luckLevel = pItemOwnerCreature->getLuck();

        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pItemOwnerCreature->getPlayer());

        if (pGamePlayer != NULL)
            itemBonusPercent = pGamePlayer->getItemRatioBonusPoint();
    } else if (pDeadMonster->getHostPartyID() != 0) {
        Party* pParty = m_pZone->getLocalPartyManager()->getParty(pDeadMonster->getHostPartyID());

        if (pParty != NULL) {
            ownerCreatureClass = pParty->getCreatureClass();
        } else {
            ownerCreatureClass = pDeadMonster->getLastHitCreatureClass();
        }
    } else {
        ownerCreatureClass = pDeadMonster->getLastHitCreatureClass();
    }

    
    if (ownerCreatureClass == Creature::CREATURE_CLASS_SLAYER) {
        pTreasureList = pMonsterInfo->getSlayerTreasureList();
    } else if (ownerCreatureClass == Creature::CREATURE_CLASS_VAMPIRE) {
        pTreasureList = pMonsterInfo->getVampireTreasureList();
    } else if (ownerCreatureClass == Creature::CREATURE_CLASS_OUSTERS) {
        pTreasureList = pMonsterInfo->getOustersTreasureList();
    }

    
    bool bChiefMonsterBonus = pDeadMonster->isChief() && g_pVariableManager->isActiveChiefMonster();


    if (pTreasureList != NULL) {
        const list<Treasure*>& treasures = pTreasureList->getTreasures();

        list<Treasure*>::const_iterator itr = treasures.begin();
        for (; itr != treasures.end(); itr++) {
            Treasure* pTreasure = (*itr);
            ITEM_TEMPLATE it;

            it.ItemClass = Item::ITEM_CLASS_MAX;
            it.ItemType = 0;
            // it.OptionType = 0;

            int itemRatioBonus = 0;

            if (bChiefMonsterBonus) {
                it.NextOptionRatio = g_pVariableManager->getChiefMonsterRareItemPercent();
                itemRatioBonus = g_pVariableManager->getPremiumItemProbePercent();
            } else {
                it.NextOptionRatio = 0;
            }

            Item* pItem = NULL;

            
            Zone* pZone = pDeadMonster->getZone();

            
            static bool isNetMarble = g_pConfig->getPropertyInt("IsNetMarble") != 0;
            bool isLottoZone = pZone->isPayPlay() || isNetMarble;

            if (pZone->isPayPlay() || pZone->isPremiumZone()) {
                
                
                if (pDeadMonster->getZoneID() == 1013) 
                {
                    pTreasure->setRndItemOptionMax(3);
                } else
                    pTreasure->setRndItemOptionMax(2);
                
                if (pTreasure->getRandomItem(&it, itemRatioBonus + g_pVariableManager->getPremiumItemProbePercent() +
                                                      itemBonusPercent)) {
                    // by sigi. 2002.10.21
                    int upgradeLevel = upgradeItemTypeByLuck(luckLevel, ownerCreatureClass, it);
                    if (upgradeLevel != 0) {
                        GCAddEffectToTile gcAE;

                        if (upgradeLevel > 0)
                            gcAE.setEffectID(Effect::EFFECT_CLASS_LUCKY);
                        else
                            gcAE.setEffectID(Effect::EFFECT_CLASS_MISFORTUNE);

                        gcAE.setObjectID(0);
                        gcAE.setDuration(0);
                        gcAE.setXY(pDeadMonster->getX(), pDeadMonster->getY());

                        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pItemOwnerCreature);
                        if (pPC != NULL) {
                            if (canGiveEventItem(pPC, pDeadMonster) &&
                                !GDRLairManager::Instance().isGDRLairZone(pPC->getZoneID())) {
                                if (upgradeLevel > 0)
                                    addOlympicStat(pPC, 5);
                                else
                                    addOlympicStat(pPC, 6);
                            }
                        }

                        
                        // pDeadMonster->getY() << endl;

                        pZone->broadcastPacket(pDeadMonster->getX(), pDeadMonster->getY(), &gcAE);
                    }

                    if (!it.OptionType.empty()) {
                        upgradeOptionByLuck(luckLevel, ownerCreatureClass, it);
                    }

                    
                    // by sigi. 2002.10.23
                    if (bChiefMonsterBonus
                        
                        
                        && rand() % 100 < g_pVariableManager->getChiefMonsterRareItemPercent() &&
                        isPossibleUpgradeItemType(it.ItemClass)) {
                        
                        int upgradeCount = 1;

                        it.ItemType = getUpgradeItemType(it.ItemClass, it.ItemType, upgradeCount);
                    }

#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)
                    it.ItemType = getItemTypeByItemLimit(it.ItemClass, it.OptionType);
#endif

                    pItem = g_pItemFactoryManager->createItem(it.ItemClass, it.ItemType, it.OptionType);
                    Assert(pItem != NULL);
                    if (pItem->getItemClass() == Item::ITEM_CLASS_RESURRECT_ITEM)
                        countResurrectItem();

                    if (pItem->isUnique())
                        pItem->setGrade(6);
                    else
                        pItem->setGrade(ItemGradeManager::Instance().getRandomGrade());
                    

                    pItem->setDurability(computeMaxDurability(pItem));

                    if (!isHarvestFestivalItemAppeared ||
                        (isHarvestFestivalItemAppeared && pItem->getItemClass() == Item::ITEM_CLASS_SKULL))
                        pMonsterCorpse->addTreasure(pItem);

                    
                    if (isLottoZone && pItem->getItemClass() == Item::ITEM_CLASS_SKULL) {
                        int lottoSkullRatio = g_pVariableManager->getVariable(LOTTO_SKULL_RATIO);
                        if (lottoSkullRatio > 0) {
                            int value = rand() % 10000;
                            if (value < lottoSkullRatio) {
                                
                                for (int i = 0; i < 8; i++) {
                                    pItem = g_pItemFactoryManager->createItem(it.ItemClass, it.ItemType, it.OptionType);
                                    pMonsterCorpse->addTreasure(pItem);
                                    if (pItem->getItemClass() == Item::ITEM_CLASS_RESURRECT_ITEM)
                                        countResurrectItem();
                                }
                            }
                        }
                    }
                }
            } else {
                
                if (pTreasure->getRandomItem(&it, g_pVariableManager->getItemProbRatio() + itemBonusPercent)) {
                    // by sigi. 2002.10.21
                    // upgradeItemTypeByLuck(luckLevel, it);
#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)

                    it.ItemType = getItemTypeByItemLimit(it.ItemClass, it.ItemType);
#endif
                    pItem = g_pItemFactoryManager->createItem(it.ItemClass, it.ItemType, it.OptionType);
                    Assert(pItem != NULL);
                    if (pItem->getItemClass() == Item::ITEM_CLASS_RESURRECT_ITEM)
                        countResurrectItem();

                    if (pItem->isUnique())
                        pItem->setGrade(6);
                    else
                        pItem->setGrade(ItemGradeManager::Instance().getRandomGrade());
                    //				pItem->setGrade( ItemGradeManager::Instance().getRandomGrade() );
                    

                    pItem->setDurability(computeMaxDurability(pItem));

                    if (!isHarvestFestivalItemAppeared ||
                        (isHarvestFestivalItemAppeared && pItem->getItemClass() == Item::ITEM_CLASS_SKULL))
                        pMonsterCorpse->addTreasure(pItem);
                }
            }


            /////////////////////////////////////////////////////////////////////////
            
            
            int nBonusItem = 0;

            if (pItem != NULL && pItem->getItemClass() != Item::ITEM_CLASS_SKULL) {
                if (bChiefMonsterBonus)
                    nBonusItem = g_pVariableManager->getVariable(CHIEF_ITEM_BONUS_NUM);

                if (isLottoZone && isLottoWinning())
                    nBonusItem = g_pVariableManager->getVariable(LOTTO_ITEM_BONUS_NUM);

                if (pDeadMonster->getMonsterType() == 765)
                    nBonusItem = 8;
                // add by sonic 2006.10.30
                if (pDeadMonster->getZoneID() == 1013 && pDeadMonster->getMonsterType() != 765 && nBonusItem == 0)
                    nBonusItem = 2;
                // end by sonic 2006.10.31
            }

            if (nBonusItem > 0) {
                int i = 0;
                int j = 0;
                static int MaxTry = 30;
                while (i < nBonusItem && j < MaxTry) {
                    Treasure* pTreasure = (*itr);
                    ITEM_TEMPLATE it;

                    it.ItemClass = Item::ITEM_CLASS_MAX;
                    it.ItemType = 0;
                    // it.OptionType = 0;

                    int itemRatioBonus = 0;

                    if (bChiefMonsterBonus || pDeadMonster->getMonsterType() == 765 ||
                        pDeadMonster->getZoneID() == 1013) //  add by sonic 2006.101.
                    {
                        it.NextOptionRatio = g_pVariableManager->getChiefMonsterRareItemPercent();
                        itemRatioBonus = g_pVariableManager->getPremiumItemProbePercent();
                    } else {
                        it.NextOptionRatio = 0;
                    }

                    Item* pItem = NULL;

                    
                    Zone* pZone = pDeadMonster->getZone();
                    if (pZone->isPayPlay() || pZone->isPremiumZone()) {
                        
                        if (pTreasure->getRandomItem(&it, itemRatioBonus +
                                                              g_pVariableManager->getPremiumItemProbePercent() +
                                                              itemBonusPercent)) {
                            // by sigi. 2002.10.21
                            int upgradeLevel = upgradeItemTypeByLuck(luckLevel, ownerCreatureClass, it);

                            if (upgradeLevel != 0) {
                                GCAddEffectToTile gcAE;

                                if (upgradeLevel > 0)
                                    gcAE.setEffectID(Effect::EFFECT_CLASS_LUCKY);
                                else
                                    gcAE.setEffectID(Effect::EFFECT_CLASS_MISFORTUNE);

                                gcAE.setObjectID(0);
                                gcAE.setDuration(0);
                                gcAE.setXY(pDeadMonster->getX(), pDeadMonster->getY());

                                PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pItemOwnerCreature);
                                if (pPC != NULL) {
                                    if (canGiveEventItem(pPC, pDeadMonster) &&
                                        !GDRLairManager::Instance().isGDRLairZone(pPC->getZoneID())) {
                                        if (upgradeLevel > 0)
                                            addOlympicStat(pPC, 5);
                                        else
                                            addOlympicStat(pPC, 6);
                                    }
                                }

                                
                                // pDeadMonster->getY() << endl;

                                pZone->broadcastPacket(pDeadMonster->getX(), pDeadMonster->getY(), &gcAE);
                            }

                            
                            // by sigi. 2002.10.23
                            if (bChiefMonsterBonus
                                
                                
                                && rand() % 100 < g_pVariableManager->getChiefMonsterRareItemPercent() &&
                                isPossibleUpgradeItemType(it.ItemClass)) {
                                
                                int upgradeCount = 1;

                                it.ItemType = getUpgradeItemType(it.ItemClass, it.ItemType, upgradeCount);
                            }

                            if (!it.OptionType.empty()) {
                                // add by sonic 2006.10.31
                                if (pDeadMonster->getZoneID() == 1013 && pDeadMonster->getMonsterType() != 765) {
                                    upgradeOptionByLuck(1, ownerCreatureClass, it);
                                } else {
                                    upgradeOptionByLuck(luckLevel, ownerCreatureClass, it);
                                }
                                // end
                            }

#if defined(__THAILAND_SERVER__) || defined(__CHAINA_SERVER__)
                            it.ItemType = getItemTypeByItemLimit(it.ItemClass, it.ItemType);
#endif

                            pItem = g_pItemFactoryManager->createItem(it.ItemClass, it.ItemType, it.OptionType);
                            Assert(pItem != NULL);
                            if (pItem->getItemClass() == Item::ITEM_CLASS_RESURRECT_ITEM)
                                countResurrectItem();

                            if (pItem->isUnique())
                                pItem->setGrade(6);
                            else
                                pItem->setGrade(ItemGradeManager::Instance().getRandomGrade());
                            // pItem->setGrade( ItemGradeManager::Instance().getRandomGrade() );
                            

                            pItem->setDurability(computeMaxDurability(pItem));

                            if (!isHarvestFestivalItemAppeared ||
                                (isHarvestFestivalItemAppeared && pItem->getItemClass() == Item::ITEM_CLASS_SKULL))
                                pMonsterCorpse->addTreasure(pItem);
                        }
                    } else {
                        
                        if (pTreasure->getRandomItem(&it, g_pVariableManager->getItemProbRatio() + itemBonusPercent)) {
                            // by sigi. 2002.10.21
                            // upgradeItemTypeByLuck(luckLevel, it);
#if defined(__CHAINA_SERVER__) || defined(__THAILAND_SERVER__)
                            it.ItemType = getItemTypeByItemLimit(it.ItemClass, it.ItemType);
#endif
                            pItem = g_pItemFactoryManager->createItem(it.ItemClass, it.ItemType, it.OptionType);
                            Assert(pItem != NULL);
                            if (pItem->getItemClass() == Item::ITEM_CLASS_RESURRECT_ITEM)
                                countResurrectItem();

                            if (pItem->isUnique())
                                pItem->setGrade(6);
                            else
                                pItem->setGrade(ItemGradeManager::Instance().getRandomGrade());
                            // pItem->setGrade( ItemGradeManager::Instance().getRandomGrade() );
                            

                            pItem->setDurability(computeMaxDurability(pItem));

                            if (!isHarvestFestivalItemAppeared ||
                                (isHarvestFestivalItemAppeared && pItem->getItemClass() == Item::ITEM_CLASS_SKULL))
                                pMonsterCorpse->addTreasure(pItem);
                        }
                    }

                    if (pItem != NULL)
                        i++;

                    j++;
                }
            }
            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
        }
    }

    
    if (pMonsterInfo->getSkullType() != 0) {
        Item* pSkull = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_SKULL, pMonsterInfo->getSkullType(),
                                                         list<OptionType_t>());
        if (pSkull != NULL) {
            pMonsterCorpse->addTreasure(pSkull);
        }
    }

    //////////////////////////////////////////////////////////////////////
    
    
    
    
    
    
    /////////////////////////////////////////////////////////////////////
     


    //////////////////////////////////////////////////////////////////////
    
    
    
    //////////////////////////////////////////////////////////////////////
    // cout << "Monster Manager: star -> " << g_pVariable->getStar() << endl;
    // int star_percentage = g_pVariable->getStar();
     
    __END_CATCH
}

int MonsterManager::upgradeItemTypeByLuck(int luckLevel, Creature::CreatureClass ownerCreatureClass, ITEM_TEMPLATE& it)

{
    __BEGIN_TRY

    if (luckLevel == 0 || !isPossibleUpgradeItemType(it.ItemClass))
        return 0;

    luckLevel = luckLevel + (rand() % 20) - 10;
    luckLevel = min(MAX_LUCK_LEVEL, luckLevel);
    //	cout << "Apply luck : " << luckLevel << endl;

    int ratio;

    switch (ownerCreatureClass) {
    case Creature::CREATURE_CLASS_SLAYER: {
        if (luckLevel >= 0) {
            ratio = (int)(((float)luckLevel / (4.254 + (1.0 + it.ItemType) / 5.0)) * 100);
        } else {
            ratio = (int)(((float)luckLevel / (2.5 - (1.0 + it.ItemType) / 20.0)) * 100);
        }
    } break;
    case Creature::CREATURE_CLASS_VAMPIRE: {
        if (luckLevel >= 0) {
            ratio = (int)(((float)luckLevel / (6.03 + (1.0 + it.ItemType) / 5.0)) * 100);
        } else {
            ratio = (int)(((float)luckLevel / (4.14 - (1.0 + it.ItemType) / 20.0)) * 100);
        }
    } break;
    case Creature::CREATURE_CLASS_OUSTERS: {
        if (luckLevel >= 0) {
            ratio = (int)(((float)luckLevel / (4.936 + (1.0 + it.ItemType) / 5.0)) * 100);
        } else {
            ratio = (int)(((float)luckLevel / (3.05 - (1.0 + it.ItemType) / 20.0)) * 100);
        }
    } break;
    default:
        return 0;
    }

    int value = rand() % 10000;
    //	int value = 0;//rand()%10000;

    //	cout << "ratio : " << ratio << endl;
    //	cout << "value : " << value << endl;

    /*	const LuckInfo& luckInfo = g_pLuckInfoManager->getLuckInfo(luckLevel);

        int upgradeCount = luckInfo.getUpgradeItemTypeCount();

        if (upgradeCount==0)
            return;*/

    //	cout << "before : " << it.ItemClass << "/" << (int)it.ItemType << endl;
    if (ratio > 0 && value < ratio) {
        it.ItemType = getUpgradeItemType(it.ItemClass, it.ItemType, 1);
        //		cout << "after : " << it.ItemClass << "/" << (int)it.ItemType << endl;
        return 1;
    } else if (ratio < 0 && value < (-ratio)) {
        it.ItemType = getDowngradeItemType(it.ItemClass, it.ItemType);
        //		cout << "after : " << it.ItemClass << "/" << (int)it.ItemType << endl;
        return -1;
    }

    return 0;

    __END_CATCH
}

int MonsterManager::upgradeOptionByLuck(int luckLevel, Creature::CreatureClass ownerCreatureClass, ITEM_TEMPLATE& it) {
    __BEGIN_TRY

    if (it.OptionType.empty())
        return 0;

    OptionType_t optionType = it.OptionType.front();
    OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo(optionType);
    if (pOptionInfo == NULL)
        return 0;

    luckLevel = luckLevel + (rand() % 20) - 10;
    luckLevel = min(MAX_LUCK_LEVEL, luckLevel);
    //	cout << "Apply luck to option : " << luckLevel << endl;

    int grade = pOptionInfo->getGrade() + 1;
    //	cout << "Option Grade : " << grade << endl;

    int ratio;

    switch (ownerCreatureClass) {
    case Creature::CREATURE_CLASS_SLAYER: {
        if (luckLevel >= 0) {
            ratio = (int)(((float)luckLevel / (grade * 25.0 - 15.2)) * 100);
        } else {
            ratio = (int)(((float)luckLevel / (7.5 - grade / 2.0)) * 100);
        }
    } break;
    case Creature::CREATURE_CLASS_VAMPIRE: {
        if (luckLevel >= 0) {
            ratio = (int)(((float)luckLevel / (grade * 25.0 - 11.3)) * 100);
        } else {
            ratio = (int)(((float)luckLevel / (10.3 - grade / 2.0)) * 100);
        }
    } break;
    case Creature::CREATURE_CLASS_OUSTERS: {
        if (luckLevel >= 0) {
            ratio = (int)(((float)luckLevel / (grade / 25.0 - 13.7)) * 100);
        } else {
            ratio = (int)(((float)luckLevel / (7.9 - grade / 2.0)) * 100);
        }
    } break;
    default:
        return 0;
    }

    int value = rand() % 10000;
    //	int value = 0;//rand()%10000;

    //	cout << "ratio : " << ratio << endl;
    //	cout << "value : " << value << endl;

    //	cout << "before : " << pOptionInfo->getHName() << endl;

    if (ratio > 0 && value < ratio && pOptionInfo->getUpgradeType() != optionType && pOptionInfo->isUpgradePossible()) {
        (*it.OptionType.begin()) = pOptionInfo->getUpgradeType();
        //		cout << "after : " << g_pOptionInfoManager->getOptionInfo( it.OptionType.front() )->getHName() << endl;
        return 1;
    } else if (ratio < 0 && value < (-ratio) && pOptionInfo->getPreviousType() != optionType) {
        if (pOptionInfo->getPreviousType() != 0)
            (*it.OptionType.begin()) = pOptionInfo->getPreviousType();
        else
            it.OptionType.pop_front();
        //		if ( it.OptionType.front() == 0 ) it.OptionType.pop_front();
        //		cout << "after : " << g_pOptionInfoManager->getOptionInfo( it.OptionType.front() )->getHName() << endl;
        return -1;
    }

    return 0;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void MonsterManager::deleteAllMonsters(bool bDeleteFromZone)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    unordered_map<ObjectID_t, Creature*>::iterator current = m_Creatures.begin();

    while (current != m_Creatures.end()) {
        Creature* pCreature = current->second;

        Assert(pCreature != NULL);

        if (bDeleteFromZone) {
            try {
                Zone* pZone = pCreature->getZone();
                Assert(m_pZone == pZone);

                // Monster* pMonster = dynamic_cast<Monster*>(pCreature);

                ZoneCoord_t cx = pCreature->getX();
                ZoneCoord_t cy = pCreature->getY();

                
                Tile& tile = m_pZone->getTile(cx, cy);
                tile.deleteCreature(pCreature->getObjectID());

                
                GCDeleteObject gcDeleteObject(pCreature->getObjectID());
                pZone->broadcastPacket(cx, cy, &gcDeleteObject, pCreature);

            } catch (Throwable& t) {
                filelog("MonsterManagerBug.txt", "deleteAllCreatures: %s", t.toString().c_str());
            }
        }

        
        SAFE_DELETE(pCreature);

        current++;
    }

    
    m_Creatures.clear();
    m_Monsters.clear();


    __END_DEBUG
    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void MonsterManager::killAllMonsters(const unordered_map<ObjectID_t, ObjectID_t>& exceptCreatures)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    unordered_map<ObjectID_t, Creature*>::iterator current = m_Creatures.begin();

    while (current != m_Creatures.end()) {
        Creature* pCreature = current->second;

        Assert(pCreature != NULL);

        if (pCreature->isAlive()) {
            if (pCreature->isMonster()) {
                unordered_map<ObjectID_t, ObjectID_t>::const_iterator itr =
                    exceptCreatures.find(pCreature->getObjectID());

                if (itr == exceptCreatures.end()) {
                    Monster* pMonster = dynamic_cast<Monster*>(pCreature);
                    pMonster->setHP(0, ATTR_CURRENT);
                }
            } else
                Assert(false);
        }
    }

    __END_DEBUG
    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string MonsterManager::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "MonsterManager(" << CreatureManager::toString();

    unordered_map<SpriteType_t, MonsterCounter*>::const_iterator itr = m_Monsters.begin();
    for (; itr != m_Monsters.end(); itr++)
        msg << itr->second->toString();

    msg << ")";
    return msg.toString();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////

 

bool isLottoWinning() {
    int lottoItemRatio = g_pVariableManager->getVariable(LOTTO_ITEM_RATIO);
    if (lottoItemRatio > 0) {
        int value = rand() % 10000;
        if (value < lottoItemRatio) {
            return true;
        }
    }

    return false;
}
