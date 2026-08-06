////////////////////////////////////////////////////////////////////////////////
// Filename    : MasterLairManager.h

// Description :
////////////////////////////////////////////////////////////////////////////////


#include "MasterLairManager.h"

#include <stdio.h>

#include "Assert.h"
#include "EffectContinualGroundAttack.h"
#include "EffectMasterLairPass.h"
#include "GCAddEffect.h"
#include "GCCreateItem.h"
#include "GCNoticeEvent.h"
#include "GCSay.h"
#include "GCSystemMessage.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "MasterLairInfoManager.h"
#include "Monster.h"
#include "MonsterCorpse.h"
#include "MonsterManager.h"
#include "PCManager.h"
#include "PacketUtil.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "StringPool.h"
#include "Timeval.h"
#include "VariableManager.h"
#include "Zone.h"
#include "ZoneGroupManager.h"
#include "ZoneInfoManager.h"

////////////////////////////////////////////////////////////////////////////////
//
// constructor
//
////////////////////////////////////////////////////////////////////////////////
MasterLairManager::MasterLairManager(Zone* pZone)

{
    __BEGIN_TRY

    Assert(pZone != NULL);
    m_pZone = pZone;

    MasterLairInfo* pInfo = g_pMasterLairInfoManager->getMasterLairInfo(m_pZone->getZoneID());
    Assert(pInfo != NULL);

    m_MasterID = 0; 
    m_MasterX = 0;
    m_MasterY = 0;

    m_bMasterReady = false; 

    
    // m_nSummonedMonster = 0;

    m_nMaxPassPlayer = pInfo->getMaxPassPlayer(); 
    m_nPassPlayer = 0;

    m_Event = EVENT_WAITING_REGEN;
    m_EventValue = 0;

    Timeval currentTime;
    getCurrentTime(currentTime);

    
    m_EventTime.tv_sec = currentTime.tv_sec + pInfo->getFirstRegenDelay();
    m_EventTime.tv_usec = 0;

    m_RegenTime.tv_sec = currentTime.tv_sec + pInfo->getFirstRegenDelay();
    m_RegenTime.tv_usec = 0;

    m_Mutex.setName("MasterLairManager");

    // cout << "Init MasterLairManager: zoneID=" << (int)m_pZone->getZoneID() << endl;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// destructor
//
////////////////////////////////////////////////////////////////////////////////
MasterLairManager::~MasterLairManager()

{
    __BEGIN_TRY

    __END_CATCH_NO_RETHROW
}

////////////////////////////////////////////////////////////////////////////////
//
// enterCreature ( Creature* )
//
////////////////////////////////////////////////////////////////////////////////
//


//

//   - EVENT_WAITING_PLAYER,
//     EVENT_MINION_COMBAT,




//



//
////////////////////////////////////////////////////////////////////////////////
bool MasterLairManager::enterCreature(Creature* pCreature)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    if (pCreature->isDM() || pCreature->isGOD()) {
        m_Mutex.unlock();

        goto ENTER_OK;
    }

    if (m_Event != EVENT_WAITING_PLAYER && m_Event != EVENT_MINION_COMBAT && m_Event != EVENT_MASTER_COMBAT) {
        
        m_Mutex.unlock();
        return false;
    }

    EffectMasterLairPass* pPassEffect = NULL;

    
    if (pCreature->isFlag(Effect::EFFECT_CLASS_MASTER_LAIR_PASS)) {
        if (g_pVariableManager->isRetryMasterLair()) {
            Effect* pEffect = pCreature->getEffectManager()->findEffect(Effect::EFFECT_CLASS_MASTER_LAIR_PASS);
            Assert(pEffect != NULL);

            pPassEffect = dynamic_cast<EffectMasterLairPass*>(pEffect);

            if (pPassEffect->getZoneID() == m_pZone->getZoneID()) {
                // cout << "[" << (int)m_pZone->getZoneID() << "] MasterLairManager: " << pCreature->getName().c_str()
                // << " has EffectPass" << endl;
                m_Mutex.unlock();

                goto ENTER_OK;
            }

            
            // cout << "[" << (int)m_pZone->getZoneID() << "] MMasterLairManager: " << pCreature->getName().c_str() << "
            // has Wrong EffectPass" << endl;
        } else {
            // cout << "[" << (int)m_pZone->getZoneID() << "] MMasterLairManager: " << pCreature->getName().c_str() << "
            // can't enter more" << endl;

            m_Mutex.unlock();
            return false;
        }
    }

    
    if (m_Event != EVENT_WAITING_PLAYER) {
        // cout << "[" << (int)m_pZone->getZoneID() << "] MasterLairManager: Not WAITING_PLAYER: "
        //	<< m_pZone->getPCManager()->getSize() << " / " << m_nPassPlayer << "/" << m_nMaxPassPlayer << endl;

        m_Mutex.unlock();
        return false;
    }

    // if (m_nPassPlayer >= m_nMaxPassPlayer)
    if (m_nPassPlayer >= g_pVariableManager->getVariable(MASTER_LAIR_PLAYER_NUM)) // by sigi. 2002.12.31
    {
        // cout << "[" << (int)m_pZone->getZoneID() << "] MasterLairManager: Already Maximum Players: "
        //<< m_pZone->getPCManager()->getSize() << " / " << m_nPassPlayer << "/" << m_nMaxPassPlayer << endl;

        m_Mutex.unlock();
        return false;
    }

    
    m_nPassPlayer++;

    if (pPassEffect == NULL) {
        pPassEffect = new EffectMasterLairPass(pCreature, m_pZone->getZoneID());

        // cout << "[" << (int)m_pZone->getZoneID() << "] MasterLairManager: " << pCreature->getName().c_str() << "
        // received EffectPass: "
        //	<< m_pZone->getPCManager()->getSize() << " / " << m_nPassPlayer << "/" << m_nMaxPassPlayer << endl;
    } else {
        pPassEffect->setZoneID(m_pZone->getZoneID());
    }

    pCreature->getEffectManager()->addEffect(pPassEffect);
    pCreature->setFlag(Effect::EFFECT_CLASS_MASTER_LAIR_PASS);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH

ENTER_OK:

     

    if (m_Event == EVENT_MINION_COMBAT || m_Event == EVENT_MASTER_COMBAT) {
        Timeval currentTime;
        getCurrentTime(currentTime);

        int timeGap = m_EventTime.tv_sec - currentTime.tv_sec;

        GCNoticeEvent gcNoticeEvent;
        gcNoticeEvent.setCode(NOTICE_EVENT_MASTER_COMBAT_TIME);
        gcNoticeEvent.setParameter(timeGap);
        // m_pZone->broadcastPacket( &gcNoticeEvent );
        pCreature->getPlayer()->sendPacket(&gcNoticeEvent);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// leaveCreature ( Creature* )
//
////////////////////////////////////////////////////////////////////////////////
//

//

//
////////////////////////////////////////////////////////////////////////////////
bool MasterLairManager::leaveCreature(Creature* pCreature)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    if (pCreature->isDM() || pCreature->isGOD()) {
        m_Mutex.unlock();
        return true;
    }

    
    if (m_Event == EVENT_WAITING_PLAYER) {
        if (m_nPassPlayer > 0)
            m_nPassPlayer--;
    }

    
    
    if (!g_pVariableManager->isRetryMasterLair()) {
        if (pCreature->isFlag(Effect::EFFECT_CLASS_MASTER_LAIR_PASS)) {
            pCreature->getEffectManager()->deleteEffect(Effect::EFFECT_CLASS_MASTER_LAIR_PASS);
            pCreature->removeFlag(Effect::EFFECT_CLASS_MASTER_LAIR_PASS);
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH

    // cout << "[" << (int)m_pZone->getZoneID() << "] MasterLairManager: " << pCreature->getName().c_str() << " leaved:
    // "
    //		<< m_pZone->getPCManager()->getSize() << " / " << m_nPassPlayer << "/" << m_nMaxPassPlayer << endl;
    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
// heartbeat
//
////////////////////////////////////////////////////////////////////////////////
bool MasterLairManager::heartbeat()

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    switch (m_Event) {
    case EVENT_WAITING_PLAYER:
        processEventWaitingPlayer();
        break;

    case EVENT_MINION_COMBAT:
        processEventMinionCombat();
        break;

    case EVENT_MASTER_COMBAT:
        processEventMasterCombat();
        break;

    case EVENT_WAITING_KICK_OUT:
        processEventWaitingKickOut();
        break;

    case EVENT_WAITING_REGEN:
        processEventWaitingRegen();
        break;

    default:
        break;
    };

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// process EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::processEventWaitingPlayer()

{
    __BEGIN_TRY

    Timeval currentTime;
    getCurrentTime(currentTime);

    
    
    if (currentTime >= m_EventTime) {
        
        //		ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( m_pZone->getZoneID() );
        //		Assert(pZoneInfo!=NULL);

        //		StringStream msg;
        

        //        char msg[50];
        //       sprintf( msg, g_pStringPool->c_str( STRID_MASTER_LAIR_CLOSED ),
        //                      pZoneInfo->getFullName().c_str() );
        //
        //       string sMsg( msg );
        //
        //		GCSystemMessage gcSystemMessage;
        //		gcSystemMessage.setType(SYSTEM_MESSAGE_MASTER_LAIR);
        //		gcSystemMessage.setMessage( sMsg );
        //		g_pZoneGroupManager->broadcast( &gcSystemMessage );

        GCNoticeEvent gcNoticeEvent;

        gcNoticeEvent.setCode(NOTICE_EVENT_MASTER_LAIR_CLOSED);
        gcNoticeEvent.setParameter(m_pZone->getZoneID());

        g_pZoneGroupManager->broadcast(&gcNoticeEvent);

        
        activeEventMinionCombat();
    } else {
        int remainSec = m_EventTime.tv_sec - currentTime.tv_sec;

        
        if (remainSec != m_EventValue && remainSec != 0 && remainSec % 60 == 0) {
            
            //			ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( m_pZone->getZoneID() );
            //			Assert(pZoneInfo!=NULL);

            //			StringStream msg;
            
            

            //            char msg[100];
            //           sprintf( msg, g_pStringPool->c_str( STRID_MASTER_LAIR_OPENING_COUNT_DOWN ),
            //                          pZoneInfo->getFullName().c_str(),
            //                         (int)(remainSec/60) );
            //
            //           string sMsg( msg );
            //
            //			GCSystemMessage gcSystemMessage;
            //			gcSystemMessage.setType(SYSTEM_MESSAGE_MASTER_LAIR);
            //			gcSystemMessage.setMessage( sMsg );
            //			g_pZoneGroupManager->broadcast( &gcSystemMessage );

            GCNoticeEvent gcNoticeEvent;

            gcNoticeEvent.setCode(NOTICE_EVENT_MASTER_LAIR_COUNT);

            int remainMin = remainSec / 60;
            uint param = (remainMin << 16) | ((int)m_pZone->getZoneID());
            gcNoticeEvent.setParameter(param);

            g_pZoneGroupManager->broadcast(&gcNoticeEvent);


            m_EventValue = remainSec;
        }
    }


    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// process EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::processEventMinionCombat()

{
    __BEGIN_TRY

    Timeval currentTime;
    getCurrentTime(currentTime);

    
    
    
    if (currentTime >= m_EventTime) {
        GCNoticeEvent gcNoticeEvent;
        gcNoticeEvent.setCode(NOTICE_EVENT_MASTER_COMBAT_END);
        m_pZone->broadcastPacket(&gcNoticeEvent);

        activeEventWaitingKickOut();
    }

    
    
    // if (m_nSummonedMonster >= m_nMaxSummonMonster
    if (m_bMasterReady
        
        && m_pZone->getMonsterManager()->getSize() == 1) {
        activeEventMasterCombat();
    }

    
    if (m_pZone->getPCManager()->getSize() == 0) {
        activeEventWaitingRegen();
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// process EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::processEventMasterCombat()

{
    __BEGIN_TRY

    Timeval currentTime;
    getCurrentTime(currentTime);

    Creature* pMaster = m_pZone->getMonsterManager()->getCreature(m_MasterID);

    if (pMaster == NULL) {
        
        StringStream msg;
        msg << " . zoneID = " << (int)m_pZone->getZoneID();

        filelog("masterLairBug.txt", "%s", msg.toString().c_str());

        // throw Error(msg.toString());
    } else {
        
        m_MasterX = pMaster->getX();
        m_MasterY = pMaster->getY();
    }

    
    
    
    if (pMaster == NULL || pMaster->isDead()) {
        killAllMonsters();
        giveKillingReward();
        activeEventWaitingKickOut();
    }

    else if (currentTime >= m_EventTime) {
        activeEventWaitingKickOut();
    }

    
    if (m_pZone->getPCManager()->getSize() == 0) {
        activeEventWaitingRegen();
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// process EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::processEventWaitingKickOut()

{
    __BEGIN_TRY

    Timeval currentTime;
    getCurrentTime(currentTime);

    
    
    
    if (currentTime >= m_EventTime) {
        kickOutPlayers();
        activeEventWaitingRegen();
    }

    __END_CATCH
}
////////////////////////////////////////////////////////////////////////////////
//
// process EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::processEventWaitingRegen()

{
    __BEGIN_TRY

    Timeval currentTime;
    getCurrentTime(currentTime);

    
    
    if (currentTime >= m_RegenTime) {
        if (g_pVariableManager->isActiveMasterLair()) {
            activeEventWaitingPlayer();
        } else {
            
            MasterLairInfo* pInfo = g_pMasterLairInfoManager->getMasterLairInfo(m_pZone->getZoneID());
            Assert(pInfo != NULL);

            m_RegenTime.tv_sec += pInfo->getRegenDelay();
        }
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// active EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::activeEventWaitingPlayer()

{
    __BEGIN_TRY

    MasterLairInfo* pInfo = g_pMasterLairInfoManager->getMasterLairInfo(m_pZone->getZoneID());
    Assert(pInfo != NULL);

    deleteAllMonsters();

    m_bMasterReady = false;
    // m_nSummonedMonster = 0;

    m_nPassPlayer = 0;

    
    getCurrentTime(m_RegenTime);
    m_EventTime.tv_sec = m_RegenTime.tv_sec + pInfo->getStartDelay();
    m_EventTime.tv_usec = m_RegenTime.tv_usec;
    m_EventValue = 0;

    
    
    int lairAttackTick = pInfo->getLairAttackTick();
    int lairAttackMinNumber = pInfo->getLairAttackMinNumber();
    int lairAttackMaxNumber = pInfo->getLairAttackMaxNumber();

    // cout << "EffectCon: " << (int)m_pZone->getZoneID() << ", " << lairAttackTick << ", " << lairAttackMinNumber << ",
    // " << lairAttackMaxNumber << endl;

    if (lairAttackMinNumber > 0 && lairAttackMaxNumber > 0) {
        
        for (int i = 0; i < 10; i++) 
        {
            Effect* pOldEffect = m_pZone->findEffect(Effect::EFFECT_CLASS_CONTINUAL_GROUND_ATTACK);
            if (pOldEffect == NULL)
                break;
            m_pZone->deleteEffect(pOldEffect->getObjectID());
        }

        EffectContinualGroundAttack* pEffect =
            new EffectContinualGroundAttack(m_pZone, Effect::EFFECT_CLASS_GROUND_ATTACK, lairAttackTick);
        // EffectContinualGroundAttack* pEffect = new EffectContinualGroundAttack(m_pZone,
        // Effect::EFFECT_CLASS_METEOR_STRIKE, lairAttackTick);
        pEffect->setDeadline(pInfo->getStartDelay() * 10);
        pEffect->setNumber(lairAttackMinNumber, lairAttackMaxNumber);

        ObjectRegistry& objectregister = m_pZone->getObjectRegistry();
        objectregister.registerObject(pEffect);

        
        m_pZone->addEffect(pEffect);

        
        GCNoticeEvent gcNoticeEvent;
        gcNoticeEvent.setCode(NOTICE_EVENT_CONTINUAL_GROUND_ATTACK);
        gcNoticeEvent.setParameter(pInfo->getStartDelay()); 

        m_pZone->broadcastPacket(&gcNoticeEvent);
    }

    
    //	ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( m_pZone->getZoneID() );
    //	Assert(pZoneInfo!=NULL);

    //	StringStream msg;
    

    //    char msg[50];
    //   sprintf( msg, g_pStringPool->c_str( STRID_MASTER_LAIR_OPENED ),
    //                  pZoneInfo->getFullName().c_str() );
    //
    //   string sMsg( msg );
    //
    //	GCSystemMessage gcSystemMessage;
    //	gcSystemMessage.setType(SYSTEM_MESSAGE_MASTER_LAIR);
    //	gcSystemMessage.setMessage( sMsg );
    //	g_pZoneGroupManager->broadcast( &gcSystemMessage );

    GCNoticeEvent gcNoticeEvent;

    gcNoticeEvent.setCode(NOTICE_EVENT_MASTER_LAIR_OPEN);
    gcNoticeEvent.setParameter(m_pZone->getZoneID());

    g_pZoneGroupManager->broadcast(&gcNoticeEvent);

    
    m_RegenTime.tv_sec += pInfo->getRegenDelay();

    m_Event = EVENT_WAITING_PLAYER;

    // cout << "[" << (int)m_pZone->getZoneID() << "] MasterLairManager::activeEventWaitingPlayer" << endl;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// active EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::activeEventMinionCombat()

{
    __BEGIN_TRY

    MasterLairInfo* pInfo = g_pMasterLairInfoManager->getMasterLairInfo(m_pZone->getZoneID());
    Assert(pInfo != NULL);

    
    GCNoticeEvent gcNoticeEvent;
    gcNoticeEvent.setCode(NOTICE_EVENT_CONTINUAL_GROUND_ATTACK_END);
    m_pZone->broadcastPacket(&gcNoticeEvent);

    gcNoticeEvent.setCode(NOTICE_EVENT_MASTER_COMBAT_TIME);
    gcNoticeEvent.setParameter(pInfo->getEndDelay());
    m_pZone->broadcastPacket(&gcNoticeEvent);


    
    deleteAllMonsters();

    
    Monster* pMaster = new Monster(pInfo->getMasterNotReadyMonsterType());
    Assert(pMaster != NULL);

    
    pMaster->setTreasure(false);

    
    pMaster->setFlag(Effect::EFFECT_CLASS_NO_DAMAGE);

    
    

    try {
        m_pZone->addCreature(pMaster, pInfo->getMasterX(), pInfo->getMasterY(), pInfo->getMasterDir());

        
        m_MasterID = pMaster->getObjectID();
    } catch (EmptyTileNotExistException&) {
        
        SAFE_DELETE(pMaster);
    }

    

    m_Event = EVENT_MINION_COMBAT;
    m_EventValue = 0;

    
    getCurrentTime(m_EventTime);
    m_EventTime.tv_sec += pInfo->getEndDelay();

    // cout << "[" << (int)m_pZone->getZoneID() << "] MasterLairManager::activeEventMinionCombat" << endl;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// active EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::activeEventMasterCombat()

{
    __BEGIN_TRY

    Creature* pMaster = m_pZone->getMonsterManager()->getCreature(m_MasterID);
    

    if (pMaster != NULL) {
        MasterLairInfo* pInfo = g_pMasterLairInfoManager->getMasterLairInfo(m_pZone->getZoneID());
        Assert(pInfo != NULL);

        Monster* pMasterMonster = dynamic_cast<Monster*>(pMaster);

        
        if (pInfo->getMasterMonsterType() != pMasterMonster->getMonsterType()) {
            
            Monster* pNewMaster = new Monster(pInfo->getMasterMonsterType());
            Assert(pNewMaster != NULL);

            
            pNewMaster->setTreasure(false);

            try {
                m_pZone->addCreature(pNewMaster, pInfo->getSummonX(), pInfo->getSummonY(), pMaster->getDir());

                
                m_MasterID = pNewMaster->getObjectID();
            } catch (EmptyTileNotExistException&) {
                m_MasterID = 0;

                
                SAFE_DELETE(pNewMaster);
            }

            
            if (pInfo->isMasterRemainNotReady()) {
                ZoneCoord_t cx = pMasterMonster->getX();
                ZoneCoord_t cy = pMasterMonster->getY();

                
                GCAddEffect gcAddEffect;
                gcAddEffect.setObjectID(pMasterMonster->getObjectID());
                gcAddEffect.setEffectID(Effect::EFFECT_CLASS_COMA);
                gcAddEffect.setDuration(0);
                m_pZone->broadcastPacket(cx, cy, &gcAddEffect);

                
                pMasterMonster->removeBrain();

                 
            } else {
                m_pZone->deleteCreature(pMaster, pMaster->getX(), pMaster->getY());

                SAFE_DELETE(pMaster);
            }
        } else {
            
            
            
            
            
            
            pMaster->removeFlag(Effect::EFFECT_CLASS_NO_DAMAGE);
        }
    }

    m_Event = EVENT_MASTER_COMBAT;
    m_EventValue = 0;

    // cout << "[" << (int)m_pZone->getZoneID() << "[ MasterLairManager::activeEventMasterCombat" << endl;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// active EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::activeEventWaitingKickOut()

{
    __BEGIN_TRY

    MasterLairInfo* pInfo = g_pMasterLairInfoManager->getMasterLairInfo(m_pZone->getZoneID());
    Assert(pInfo != NULL);

    
    Creature* pMaster = m_pZone->getMonsterManager()->getCreature(m_MasterID);

    if (pMaster != NULL && pMaster->isAlive()) {
        GCSay gcSay;
        gcSay.setObjectID(pMaster->getObjectID());
        gcSay.setColor(MASTER_SAY_COLOR);
        gcSay.setMessage(pInfo->getRandomMasterNotDeadSay());
        if (!gcSay.getMessage().empty())
            m_pZone->broadcastPacket(pMaster->getX(), pMaster->getY(), &gcSay);
    }

    
    m_Event = EVENT_WAITING_KICK_OUT;
    m_EventValue = 0;

    getCurrentTime(m_EventTime);
    m_EventTime.tv_sec += pInfo->getKickOutDelay();

    
    GCNoticeEvent gcNoticeEvent;
    gcNoticeEvent.setCode(NOTICE_EVENT_KICK_OUT_FROM_ZONE);
    gcNoticeEvent.setParameter(pInfo->getKickOutDelay());

    m_pZone->broadcastPacket(&gcNoticeEvent);

    // cout << "[" << (int)m_pZone->getZoneID() << "] MasterLairManager::activeEventKickOut" << endl;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// active EventWaitingPlayer
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::activeEventWaitingRegen()

{
    __BEGIN_TRY

    deleteAllMonsters();

    

    // m_nSummonedMonster = 0;
    m_nPassPlayer = 0;
    m_Event = EVENT_WAITING_REGEN;
    m_EventValue = 0;

    m_bMasterReady = false;

    // cout << "[" << (int)m_pZone->getZoneID() << "] MasterLairManager::activeEventWaitingRegen" << endl;


    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// delete All Monsters
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::deleteAllMonsters()

{
    __BEGIN_TRY

    
    // m_pZone->getMonsterManager()->deleteCreature( m_pMaster->getObjectID() );
    // SAFE_DELETE(m_pMaster);
    bool bDeleteFromZone = true;
    m_pZone->getMonsterManager()->deleteAllMonsters(bDeleteFromZone);

    m_MasterID = 0;
    m_MasterX = 0;
    m_MasterY = 0;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// kill All Monsters
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::killAllMonsters()

{
    __BEGIN_TRY

    

     

    __END_CATCH
}
////////////////////////////////////////////////////////////////////////////////
//
// increase SummonedMonster Number
//
////////////////////////////////////////////////////////////////////////////////
/*
void MasterLairManager::increaseSummonedMonsterNumber(int num)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    m_nSummonedMonster += num;

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}
*/
////////////////////////////////////////////////////////////////////////////////
//
// start Event
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::startEvent()

{
    __BEGIN_TRY

    activeEventWaitingPlayer();

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// start Event
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::stopEvent()

{
    __BEGIN_TRY

    kickOutPlayers();
    activeEventWaitingRegen();

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// kickOut Players
//
////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::kickOutPlayers()

{
    __BEGIN_TRY

    MasterLairInfo* pInfo = g_pMasterLairInfoManager->getMasterLairInfo(m_pZone->getZoneID());
    Assert(pInfo != NULL);

     


    
    int lairAttackTick = pInfo->getLairAttackTick();
    int lairAttackMinNumber = pInfo->getLairAttackMinNumber();
    int lairAttackMaxNumber = pInfo->getLairAttackMaxNumber();

    EffectContinualGroundAttack* pEffect =
        new EffectContinualGroundAttack(m_pZone, Effect::EFFECT_CLASS_METEOR_STRIKE, lairAttackTick);
    pEffect->setDeadline(pInfo->getStartDelay() * 10);
    pEffect->setNumber(lairAttackMinNumber, lairAttackMaxNumber);

    ObjectRegistry& objectregister = m_pZone->getObjectRegistry();
    objectregister.registerObject(pEffect);

    
    m_pZone->addEffect(pEffect);

    
    GCNoticeEvent gcNoticeEvent;
    gcNoticeEvent.setCode(NOTICE_EVENT_CONTINUAL_GROUND_ATTACK);
    gcNoticeEvent.setParameter(pInfo->getStartDelay()); 

    m_pZone->broadcastPacket(&gcNoticeEvent);

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
// give Killing Reward
//
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
void MasterLairManager::giveKillingReward()

{
    __BEGIN_TRY

    const PCManager* pPCManager = m_pZone->getPCManager();
    const unordered_map<ObjectID_t, Creature*>& creatures = pPCManager->getCreatures();
    unordered_map<ObjectID_t, Creature*>::const_iterator itr;

    if (creatures.empty())
        return;

    int goodOneIndex = rand() % creatures.size(); 

    ItemType_t itemType;
    int i;
    for (i = 0, itr = creatures.begin(); itr != creatures.end(); i++, itr++) {
        Creature* pCreature = itr->second;

        if (pCreature->isPC()) {
            PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
            Inventory* pInventory = pPC->getInventory();

            //------------------------------------------------------------
            
            //------------------------------------------------------------
            
            //
            if (pPC->getDistance(m_MasterX, m_MasterY) <= 7) {
                pPC->increaseRankExp(MASTER_KILL_RANK_EXP);
            }

            //------------------------------------------------------------
            
            //------------------------------------------------------------
            
            switch (m_pZone->getZoneID()) {
            
            case 1104:
            case 1106:
                itemType = ((goodOneIndex == i) ? 1 : 0);
                break;

            
            case 1114:
            case 1115:
                itemType = ((goodOneIndex == i) ? 3 : 2);
                break;

            default:
                filelog("MasterLairBUG.txt", "ZoneID ");
                return;
            }

            list<OptionType_t> nullList;
            Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_QUEST_ITEM, itemType, nullList);

            (m_pZone->getObjectRegistry()).registerObject(pItem);

            
            _TPOINT p;
            if (pInventory->getEmptySlot(pItem, p)) {
                
                pInventory->addItem(p.x, p.y, pItem);

                pItem->create(pCreature->getName(), STORAGE_INVENTORY, 0, p.x, p.y);

                
                if (pItem != NULL && pItem->isTraceItem()) {
                    remainTraceLog(pItem, "LairMaster", pCreature->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
                    remainTraceLogNew(pItem, pCreature->getName(), ITL_GET, ITLD_EVENTNPC, m_pZone->getZoneID());
                }

                
                GCCreateItem gcCreateItem;

                makeGCCreateItem(&gcCreateItem, pItem, p.x, p.y);

                pCreature->getPlayer()->sendPacket(&gcCreateItem);
            } else {
                

                TPOINT p = m_pZone->addItem(pItem, pCreature->getX(), pCreature->getY());
                if (p.x != -1) {
                    pItem->create("", STORAGE_ZONE, m_pZone->getZoneID(), p.x, p.y);

                    
                    if (pItem != NULL && pItem->isTraceItem()) {
                        char zoneName[15];
                        sprintf(zoneName, "%4d%3d%3d", m_pZone->getZoneID(), p.x, p.y);
                        remainTraceLog(pItem, "LairMaster", zoneName, ITEM_LOG_CREATE, DETAIL_EVENTNPC);
                        remainTraceLogNew(pItem, zoneName, ITL_GET, ITLD_EVENTNPC, m_pZone->getZoneID(), p.x, p.y);
                    }
                } else {
                    SAFE_DELETE(pItem);
                }
            }
        } else {
            throw Error("PCManager PC  -_-");
        }
    }

    __END_CATCH
}

string MasterLairManager::toString() const

{
    StringStream msg;

    int eventSec = m_EventTime.tv_sec;

    switch (m_Event) {
    case EVENT_WAITING_PLAYER: 
        msg << "WAITING_PLAYER, ";
        break;

    case EVENT_MINION_COMBAT: 
        msg << "MINION_COMBAT, ";
        break;

    case EVENT_MASTER_COMBAT:
        msg << "MASTER_COMBAT, ";
        break;

    case EVENT_WAITING_KICK_OUT: 
        msg << "WAITING_KICK_OUT, ";
        break;

    case EVENT_WAITING_REGEN: 
        msg << "WAITING_REGEN, ";

        eventSec = m_RegenTime.tv_sec;
        break;

    default:
        break;
    }

    Timeval currentTime;
    getCurrentTime(currentTime);

    int timeGap = eventSec - currentTime.tv_sec;

    msg << timeGap << " sec remain, " << (int)m_pZone->getPCManager()->getSize() << " players";

    return msg.toString();
}
