//////////////////////////////////////////////////////////////////////////////
// Filename    : Monster.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Monster.h"

#include "AbilityBalance.h"
#include "CreatureUtil.h"
#include "Effect.h"
#include "EffectRegenerate.h"
#include "GCAddMonster.h"
#include "GCMove.h"
#include "GCSay.h"
#include "GCStatusCurrentHP.h"
#include "GDRLairManager.h"
#include "MonsterAI.h"
#include "MonsterFunctionObject.h"
#include "MonsterInfo.h"
#include "MonsterManager.h"
#include "MonsterNameManager.h"
#include "Ousters.h"
#include "PrecedenceTable.h"
#include "Profile.h"
#include "SiegeManager.h"
#include "SkillHandler.h"
#include "SkillUtil.h"
#include "Slayer.h"
#include "StringPool.h"
#include "Vampire.h"
#include "VariableManager.h"
#include "Zone.h"
#include "skill/EffectDoom.h"
#include "skill/EffectParalyze.h"
#include "skill/EffectPeace.h"
#include "skill/EffectTransformToBat.h"
#include "skill/EffectTransformToWolf.h"
#include "skill/SkillHandlerManager.h"
#include "skill/SummonGroundElemental.h"

// #define __PROFILE_MONSTER__

#ifdef __PROFILE_MONSTER__
#define __BEGIN_PROFILE_MONSTER(name) beginProfileEx(name);
#define __END_PROFILE_MONSTER(name) endProfileEx(name);
#else
#define __BEGIN_PROFILE_MONSTER(name) ((void)0);
#define __END_PROFILE_MONSTER(name) ((void)0);
#endif

// #ifdef __XMAS_EVENT_CODE__

const string SantaSpeech[] = {" ,    !!!",
                              "!!!",
                              "    !!!",
                              "~ O.o o.O",
                              " !!",
                              "     !!!",
                              "   !!!",
                              "     ???",
                              "!!!   !!!",
                              "     !!!",
                              "~~~  !!!",
                              "~ .  .",
                              " ..."};

const string RudolfSpeech[] = {
    "~ ~", "~", "~",       "~",     "~",
    "",        "~", "~ ~", "~ ~", "~ ~ O.O",
};

const int SantaSpeechMax = 13;
const int RudolfSpeechMax = 10;

// #endif


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
bool Monster::isRealEnemy(Creature* pEnemy)

{
    __BEGIN_TRY

    Assert(pEnemy != NULL);

    if (m_pBrain == NULL)
        return false;
    Zone* pZone = pEnemy->getZone();
    if (pEnemy->getObjectID() == m_OwnerObjectID)
        return false;

    if (m_OwnerObjectID != 0) {
        Creature* pOwner = m_pZone->getCreature(m_OwnerObjectID);
        if (pOwner != NULL && pOwner->getCreatureClass() == pEnemy->getCreatureClass() && canAttack(pOwner, pEnemy)) {
            //			cout << __PRETTY_FUNCTION__ << pEnemy->getName() << "is same class as owner" <<
            // pOwner->getName() << endl;
            return false;
        }

        if (GDRLairManager::Instance().isGDRLairZone(getZoneID()) && pEnemy->isPC())
            return false;
    }

    
    if (pEnemy->isFlag(Effect::EFFECT_CLASS_GHOST)
        
        || pEnemy->isFlag(Effect::EFFECT_CLASS_COMA)
        
        || (!m_pBrain->canAttackAir() && pEnemy->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT))
        
        || !pEnemy->isFlag(Effect::EFFECT_CLASS_PARALYZE) 
               && !pZone->isMasterLair()                  
               && (!isFlag(Effect::EFFECT_CLASS_OBSERVING_EYE) && pEnemy->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE)
                   
                   || !isFlag(Effect::EFFECT_CLASS_DETECT_INVISIBILITY) &&
                          pEnemy->isFlag(Effect::EFFECT_CLASS_INVISIBILITY))
        
        || pEnemy->isFlag(Effect::EFFECT_CLASS_ARMAGEDDON)
        
        || (!m_pBrain->canSeeSafeZone() && (pZone->getZoneLevel(pEnemy->getX(), pEnemy->getY()) & SAFE_ZONE))) {
        return false;
    }

    if (SiegeManager::Instance().isSiegeZone(pZone->getZoneID())) {
        if (pEnemy->isFlag(Effect::EFFECT_CLASS_SIEGE_DEFENDER) || pEnemy->isFlag(Effect::EFFECT_CLASS_SIEGE_REINFORCE))
            return false;
    }

    return true;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
Monster::Monster(MonsterType_t monsterType) : m_MonsterType(monsterType) {
    __BEGIN_TRY

    try {
        const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(m_MonsterType);

        
        m_MoveMode = pMonsterInfo->getMoveMode();

        
        m_STR = pMonsterInfo->getSTR();
        m_DEX = pMonsterInfo->getDEX();
        m_INT = pMonsterInfo->getINT();
        m_Exp = pMonsterInfo->getExp();
        m_Moral = pMonsterInfo->getMoral();
        m_Delay = pMonsterInfo->getDelay();
        m_AttackDelay = pMonsterInfo->getAttackDelay();
        m_Sight = pMonsterInfo->getSight();
        m_MeleeRange = pMonsterInfo->getMeleeRange();
        m_MissileRange = pMonsterInfo->getMissileRange();
        m_AttackOrder = pMonsterInfo->getAttackOrder();

        // by sigi. 2002.9.2
        m_bMaster = pMonsterInfo->isMaster();
        m_bTreasure = pMonsterInfo->hasTreasure();
        m_MonsterSummonStep = 0;

        Creature::CreatureClass CClass = getCreatureClass();
        BASIC_ATTR attr;

        attr.nSTR = m_STR;
        attr.nDEX = m_DEX;
        attr.nINT = m_INT;
        attr.nLevel = pMonsterInfo->getLevel();

        m_HP[ATTR_MAX] = computeHP(CClass, &attr, pMonsterInfo->getEnhanceHP());
        m_ToHit = computeToHit(CClass, &attr, pMonsterInfo->getEnhanceToHit());
        m_Defense = computeDefense(CClass, &attr, pMonsterInfo->getEnhanceDefense());
        m_Protection = computeProtection(CClass, &attr, pMonsterInfo->getEnhanceProtection());
        m_Damage[ATTR_CURRENT] = computeMinDamage(CClass, &attr, pMonsterInfo->getEnhanceMinDamage());
        m_Damage[ATTR_MAX] = computeMaxDamage(CClass, &attr, pMonsterInfo->getEnhanceMaxDamage());

        if (monsterType == 717)
            m_HP[ATTR_MAX] = 40000;
        if (monsterType == 723)
            m_HP[ATTR_MAX] = 60000;
        if (monsterType == 724)
            m_HP[ATTR_MAX] = 30000;
        if (monsterType == 725)
            m_HP[ATTR_MAX] = 30000;
        if (monsterType == 764)
            m_HP[ATTR_MAX] = 55000;
        if (monsterType == 765)
            m_HP[ATTR_MAX] = 60000;

        if (m_HP[ATTR_MAX] > 20000) {
            if (monsterType >= 717)
                cout << pMonsterInfo->getEName() << " HP : " << m_HP[ATTR_MAX] << endl;
            else
                m_HP[ATTR_MAX] = 20000;
        }

        
        if (monsterType == 358 || monsterType == 359 || monsterType == 360 || monsterType == 361)
            m_HP[ATTR_MAX] = m_HP[ATTR_MAX] * 10;

        if (monsterType >= 371 && monsterType <= 375) {
            m_HP[ATTR_MAX] = 500; 
            m_Defense = 50;
            m_Protection = 20;
        }

        m_HP[ATTR_CURRENT] = m_HP[ATTR_MAX];
        m_SilverDamage = 0;

        
        getCurrentTime(m_NextTurn);
        m_NextTurn.tv_sec += rand() % 3;
        m_NextTurn.tv_usec += rand() % 1000000;
        if (m_NextTurn.tv_usec >= 1000000)
            m_NextTurn.tv_sec++;

        clearAccuDelay();

        if (monsterType != 722) {
            
            uint aitype = pMonsterInfo->getAIType();
            if (aitype != 65535)
                m_pBrain = new MonsterAI(this, aitype);
            else {
                cout << pMonsterInfo->getEName() << "  ." << endl;
                m_pBrain = NULL;
            }
        } else {
            m_pBrain = NULL;
        }

        
        switch (pMonsterInfo->selectRegenType()) {
        case REGENTYPE_HIDE:
            setFlag(Effect::EFFECT_CLASS_HIDE);
            m_MoveMode = MOVE_MODE_BURROWING;
            break;

        case REGENTYPE_PORTAL:
            // by sigi. 2002.9.6
            setFlag(Effect::EFFECT_CLASS_VAMPIRE_PORTAL);
            break;

        case REGENTYPE_INVISIBLE:
            setFlag(Effect::EFFECT_CLASS_INVISIBILITY);
            break;

        case REGENTYPE_BAT:
            setFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT);
            m_MoveMode = MOVE_MODE_FLYING;
            break;

        default:
            break;
        }

        // Set the name.
        if (pMonsterInfo->getEName().length() < 20) {
            m_Name = pMonsterInfo->getEName();
        } else {
            m_Name = g_pMonsterNameManager->getRandomName(this);
        }

        getCurrentTime(m_LastSpeechTime);

        switch (monsterType) {
        case 358:
        case 359:
            m_Name = g_pStringPool->getString(STRID_EVENT_SANTA_NAME);
            break;
        case 360:
        case 361:
            m_Name = g_pStringPool->getString(STRID_EVENT_WOLF_NAME);
            break;
        case 371:
        case 372:
            m_Name = g_pStringPool->getString(STRID_SLAYER_RELIC_SHRINE);
            break;
        case 374:
        case 375:
            m_Name = g_pStringPool->getString(STRID_VAMPIRE_RELIC_SHRINE);
            break;
        default:
            break;
        }

        if (monsterType >= 660 && monsterType <= 669)
            m_Name = g_pStringPool->getString(STRID_ORE);

        
        // by sigi. 2002.10.8
        m_ClanType = pMonsterInfo->getClanType(); // CLAN_VAMPIRE_MONSTER;
        m_OwnerObjectID = 0;

        m_HostName = "";
        m_HostPartyID = 0;

        
        
        
        if (monsterType >= 371 && monsterType <= 375) {
            m_Resist[MAGIC_DOMAIN_NO_DOMAIN] = 0;
            m_Resist[MAGIC_DOMAIN_POISON] = 70;
            m_Resist[MAGIC_DOMAIN_ACID] = 60;
            m_Resist[MAGIC_DOMAIN_CURSE] = 100;
            m_Resist[MAGIC_DOMAIN_BLOOD] = 50;
        } else {
            m_Resist[MAGIC_DOMAIN_NO_DOMAIN] = 0;
            m_Resist[MAGIC_DOMAIN_POISON] = 50;
            m_Resist[MAGIC_DOMAIN_ACID] = 0;
            m_Resist[MAGIC_DOMAIN_CURSE] = 0;
            m_Resist[MAGIC_DOMAIN_BLOOD] = 0;
        }

        
        pMonsterInfo->addDefaultEffects(this);

        m_LastHitCreatureClass = CREATURE_CLASS_MAX;
        m_isEventMonster = false;

        
        m_RelicIndex = -1;

        
        if (monsterType >= 660 && monsterType <= 669) {
            m_RelicIndex = monsterType;
        }

        m_bScanEnemy = false;
        m_NextScanTurn.tv_sec = 0;
        m_NextScanTurn.tv_usec = 0;

        m_pOldEnemy = NULL;
        m_bEnemyLimit = false;

        // 2002.10.14
        if (pMonsterInfo->isNormalRegen()) {
            m_EventMonsterIndex = 0xFFFF;
        } else {
            
            
            m_EventMonsterIndex = 0xF000 + rand() % 0x0FFF;
        }

        setChief(pMonsterInfo->isChief());

    } catch (Throwable& t) {
        filelog("monsterCreateBug.txt", "%s", t.toString().c_str());
        throw;
    }

    m_pQuestItem = NULL;

#ifdef __UNDERWORLD__
    m_bUnderWorld = false;
#endif

    m_LastKiller = 0;
    getCurrentTime(m_NextRegenTime);

    switch (m_MonsterType) {
    case 724:
    case 725:
        m_HP[ATTR_CURRENT] = m_HP[ATTR_MAX] = 30000;
        break;
    default:
        break;
    }

    switch (m_MonsterType) {
    case 717:
    case 723:
    case 724:
    case 725: {
        m_RegenAmount = 100;
    } break;

    case 721: {
        m_RegenAmount = 50;
    } break;

    default: {
        m_RegenAmount = 0;
    }
    }

    if (m_MonsterType == 764) {
        EffectRegenerate* pEffect = new EffectRegenerate(this);
        pEffect->setTick(30);
        pEffect->setPoint(200);
        pEffect->setNextTime(0);
        addEffect(pEffect);
    } else if (m_MonsterType == 765) {
        EffectRegenerate* pEffect = new EffectRegenerate(this);
        pEffect->setTick(30);
        pEffect->setPoint(300);
        pEffect->setNextTime(0);
        addEffect(pEffect);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
Monster::~Monster()

{
    __BEGIN_TRY

    SAFE_DELETE(m_pBrain);

    __END_CATCH_NO_RETHROW
}

SpriteType_t Monster::getSpriteType() const

{
    __BEGIN_TRY

    return g_pMonsterInfoManager->getMonsterInfo(m_MonsterType)->getSpriteType();

    __END_CATCH
}

Level_t Monster::getLevel() const

{
    __BEGIN_TRY

    return g_pMonsterInfoManager->getMonsterInfo(m_MonsterType)->getLevel();

    __END_CATCH
}

uint Monster::getBodySize() const

{
    __BEGIN_TRY

    return g_pMonsterInfoManager->getMonsterInfo(m_MonsterType)->getBodySize();

    __END_CATCH
}

Color_t Monster::getMainColor() const

{
    __BEGIN_TRY

    return g_pMonsterInfoManager->getMonsterInfo(m_MonsterType)->getMainColor();

    __END_CATCH
}

Color_t Monster::getSubColor() const

{
    __BEGIN_TRY

    return g_pMonsterInfoManager->getMonsterInfo(m_MonsterType)->getSubColor();

    __END_CATCH
}

MAlignment Monster::getAlignment() const

{
    __BEGIN_TRY

    return g_pMonsterInfoManager->getMonsterInfo(m_MonsterType)->getAlignment();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// registerObject
//////////////////////////////////////////////////////////////////////////////
void Monster::registerObject()

{
    __BEGIN_TRY

    Assert(getZone() != NULL);

    ObjectRegistry& OR = getZone()->getObjectRegistry();

    __ENTER_CRITICAL_SECTION(OR)

    OR.registerObject_NOLOCKED(this);

    __LEAVE_CRITICAL_SECTION(OR)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void Monster::act(const Timeval& currentTime)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    if (m_RegenAmount != 0 && currentTime > m_NextRegenTime) {
        if ((HP_t)(m_HP[ATTR_MAX] * 0.3) > m_HP[ATTR_CURRENT]) {
            switch (m_MonsterType) {
            case 724:
            case 725: {
                m_NextRegenTime.tv_sec += 1;
                m_RegenAmount = 100;
            } break;
            case 717: {
                m_NextRegenTime.tv_sec += 1;
                m_RegenAmount = 200;
            } break;
            case 723: {
                m_NextRegenTime.tv_sec += 1;
                m_RegenAmount = 300;
            } break;
            default:
                m_NextRegenTime.tv_sec += 3;
            }
        } else {
            switch (m_MonsterType) {
            case 724:
            case 725:
            case 717:
            case 723: {
                m_NextRegenTime.tv_sec += 2;
            } break;

            case 721:
            default: {
                m_NextRegenTime.tv_sec += 7;
            } break;
            }
        }

        m_HP[ATTR_CURRENT] += min((int)m_RegenAmount, m_HP[ATTR_MAX] - m_HP[ATTR_CURRENT]);

        GCStatusCurrentHP gcHP;
        gcHP.setObjectID(getObjectID());
        gcHP.setCurrentHP(m_HP[ATTR_CURRENT]);

        if (getZone() != NULL)
            getZone()->broadcastPacket(getX(), getY(), &gcHP);
    }

    
    
    if (currentTime < m_NextTurn)
        return;

    
    if (m_AccuDelay.tv_sec != 0 || m_AccuDelay.tv_usec != 0) {
        m_NextTurn.tv_sec += m_AccuDelay.tv_sec;
        m_NextTurn.tv_usec += m_AccuDelay.tv_usec;

        clearAccuDelay();
        return;
    }

    __BEGIN_PROFILE_MONSTER("M_VERIFY_ENEMIES");

    
    
    verifyEnemies();

    __END_PROFILE_MONSTER("M_VERIFY_ENEMIES");

    
    if (m_pBrain == NULL) {
        
        Timeval delay;
        delay.tv_sec = 1;
        delay.tv_usec = 500000;
        addAccuDelay(delay);

        return;
    }

    
    if (isFlag(Effect::EFFECT_CLASS_PARALYZE) || isFlag(Effect::EFFECT_CLASS_COMA) ||
        isFlag(Effect::EFFECT_CLASS_CAUSE_CRITICAL_WOUNDS) || isFlag(Effect::EFFECT_CLASS_SLEEP) ||
        isFlag(Effect::EFFECT_CLASS_ARMAGEDDON) || isFlag(Effect::EFFECT_CLASS_TRAPPED) ||
        isFlag(Effect::EFFECT_CLASS_EXPLOSION_WATER)) {
        m_pBrain->setDelay(currentTime);
        return;
    }
    /*
    //#ifdef __XMAS_EVENT_CODE__

        if (m_LastSpeechTime < currentTime && isEventMonster(this))
        {
            GCSay gcSay;
            gcSay.setObjectID(getObjectID());

            if (m_MonsterType == 358 || m_MonsterType == 359)
            {
                gcSay.setMessage(SantaSpeech[rand()%SantaSpeechMax]);
            }
            else if (m_MonsterType == 360 || m_MonsterType == 361)
            {
                gcSay.setMessage(RudolfSpeech[rand()%RudolfSpeechMax]);
            }
            else
            {
                Assert(false);
            }

            m_pZone->broadcastPacket(m_X, m_Y, &gcSay);

            m_LastSpeechTime.tv_sec = currentTime.tv_sec + 30;
            m_LastSpeechTime.tv_usec = currentTime.tv_usec;
        }

    //#endif
    */
    
    if (hasEnemy()) {
        Creature* pEnemy = getPrimaryEnemy();

        if (pEnemy != NULL) {
            __BEGIN_PROFILE_MONSTER("M_AI_DEAL");
            m_pBrain->deal(pEnemy, currentTime);
            __END_PROFILE_MONSTER("M_AI_DEAL");
        }
    } else {
        
        if (isFlag(Effect::EFFECT_CLASS_HIDE)) {
            __BEGIN_PROFILE_MONSTER("M_UNBURROW");
            const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(m_MonsterType);

            if ((rand() & 0x0000007F) < pMonsterInfo->getUnburrowChance()) {
                SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_UN_BURROW);
                Assert(pSkillHandler != NULL);

                pSkillHandler->execute(this);

                
                Timeval delay;
                delay.tv_sec = 1;
                delay.tv_usec = 500000;
                addAccuDelay(delay);
            } else {
                
                m_pBrain->setDelay(currentTime);
            }
            __END_PROFILE_MONSTER("M_UNBURROW");
        }
        
        
        
        else if (!isMaster() && m_pZone->getZoneID() != 72) {
            
            if (m_RelicIndex == -1) {
                __BEGIN_PROFILE_MONSTER("M_MOVE_CENTER");
                POINT pt(getX(), getY());

                VSRect* pOuterRect = m_pZone->getOuterRect();
                VSRect* pInnerRect = m_pZone->getInnerRect();
                VSRect* pCoreRect = m_pZone->getCoreRect();

                
                if (pCoreRect->ptInRect(pt) || pInnerRect->ptInRect(pt)) {
                    
                    uint diceResult = rand() & 0x0000007F; //%100;
                    if (diceResult < 6)                    // 5
                    {
                        uint direction = rand() & 0x00000007; //% 8;
                        ZoneCoord_t nx = pt.x + dirMoveMask[direction].x;
                        ZoneCoord_t ny = pt.y + dirMoveMask[direction].y;

                        
                        
                        if (canMove(nx, ny) && !(m_pZone->getZoneLevel(nx, ny) & SAFE_ZONE)) {
                            m_pZone->moveCreature(this, nx, ny, direction);
                        }
                    }
                } else if (pOuterRect->ptInRect(pt)) {
                    
                    m_pBrain->move(m_pZone->getWidth() >> 1, m_pZone->getHeight() >> 1);
                }

                if ((m_bScanEnemy || isFlag(Effect::EFFECT_CLASS_HALLUCINATION)) && currentTime > m_NextScanTurn) {
                    m_pZone->monsterScan(this, m_X, m_Y, m_Dir);

                    
                    m_NextScanTurn.tv_sec = currentTime.tv_sec + 2;
                    m_NextScanTurn.tv_usec = currentTime.tv_usec;
                }
                __END_PROFILE_MONSTER("M_MOVE_CENTER");
            }

            
            
            m_pBrain->setDelay(currentTime);
        }
    }

    //----------------------------------------------------------------------
    
    // fucking
    //----------------------------------------------------------------------
    __BEGIN_PROFILE_MONSTER("M_REGEN_HP");
    int statSum = m_STR + m_DEX + m_INT;
    if (rand() % 600 < statSum && isAlive())
        m_HP[ATTR_CURRENT] = (HP_t)min((int)m_HP[ATTR_CURRENT] + 1, (int)m_HP[ATTR_MAX]);

    if (m_HP[ATTR_CURRENT] == m_HP[ATTR_MAX] && isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN)) {
        removeFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
    }
    __END_PROFILE_MONSTER("M_REGEN_HP");

    __END_DEBUG
    __END_CATCH
}

void Monster::actDeadAction(void)

{
    __BEGIN_TRY

    
    
    verifyEnemies();

    
    if (m_pBrain != NULL) {
        if (hasEnemy()) {
            Creature* pEnemy = getPrimaryEnemy();
            // Assert(pEnemy != NULL);
            if (pEnemy != NULL)
                m_pBrain->actDeadAction(pEnemy);
        }
    } else if (getMonsterType() == GROUND_ELEMENTAL_TYPE) {
        int oX, oY;
        int X = getX();
        int Y = getY();

        for (oX = X - 2; oX <= X + 2; ++oX)
            for (oY = Y - 2; oY <= Y + 2; ++oY) {
                if (!isValidZoneCoord(getZone(), oX, oY))
                    continue;
                Effect* pEffect = getZone()->getTile(oX, oY).getEffect(Effect::EFFECT_CLASS_GROUND_ELEMENTAL_AURA);
                if (pEffect != NULL) {
                    getZone()->deleteEffect(pEffect->getObjectID());
                }
            }
    } else if (getMonsterType() >= 726 && getMonsterType() <= 729) {
        SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_SUMMON_MONSTERS);
        pSkillHandler->execute(this);
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//

//




//////////////////////////////////////////////////////////////////////////////
void Monster::addEnemy(Creature* pCreature)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Assert(pCreature != NULL);

    
    if (this == pCreature || !isRealEnemy(pCreature))
        return;

    
    
    if (isFlag(Effect::EFFECT_CLASS_PEACE)) {
        Effect* pEffect = m_pEffectManager->findEffect(Effect::EFFECT_CLASS_PEACE);

        EffectPeace* pEffectPeace = dynamic_cast<EffectPeace*>(pEffect);

        
        if (pCreature->getObjectID() == pEffectPeace->getPeaceCreatureID()) {
            pEffect->unaffect(this);
            m_pEffectManager->deleteEffect(Effect::EFFECT_CLASS_PEACE);
        }
    }

    try {
        switch (getAttackOrder()) {
        
        
        case ATTACK_FIRST: {
            
            
            list<ObjectID_t>::iterator itr = find(m_Enemies.begin(), m_Enemies.end(), pCreature->getObjectID());
            if (itr == m_Enemies.end()) {
                m_Enemies.push_back(pCreature->getObjectID());
                if (m_Enemies.size() > getMaxEnemies()) {
                    m_Enemies.pop_back();
                }
            }
        } break;

        
        
        case ATTACK_LAST: {
            
            
            list<ObjectID_t>::iterator itr = find(m_Enemies.begin(), m_Enemies.end(), pCreature->getObjectID());
            if (itr == m_Enemies.end()) {
                m_Enemies.push_front(pCreature->getObjectID());

                
                if (m_Enemies.size() > getMaxEnemies()) {
                    m_Enemies.pop_back();
                }
            }
        } break;

        
        case ATTACK_WEAKEST: {
            
            list<ObjectID_t>::iterator itr = find(m_Enemies.begin(), m_Enemies.end(), pCreature->getObjectID());
            if (itr == m_Enemies.end()) {
                if (pCreature->isSlayer()) {
                    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

                    itr = find_if(m_Enemies.begin(), m_Enemies.end(),
                                  StrongerSlayer(m_pZone, pSlayer->getSkillDomainLevelSum()));

                    if (itr == m_Enemies.end())
                        m_Enemies.push_front(pSlayer->getObjectID());
                    else
                        m_Enemies.insert(itr, pSlayer->getObjectID());

                } else if (pCreature->isVampire()) {
                    Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

                    itr = find_if(m_Enemies.begin(), m_Enemies.end(), StrongerVampire(m_pZone, pVampire->getLevel()));

                    if (itr == m_Enemies.end())
                        m_Enemies.push_back(pVampire->getObjectID());
                    else
                        m_Enemies.insert(itr, pVampire->getObjectID());

                } else if (pCreature->isOusters()) {
                    Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

                    itr = find_if(m_Enemies.begin(), m_Enemies.end(), StrongerOusters(m_pZone, pOusters->getLevel()));

                    if (itr == m_Enemies.end())
                        m_Enemies.push_back(pOusters->getObjectID());
                    else
                        m_Enemies.insert(itr, pOusters->getObjectID());

                }
                
                else if (pCreature->isMonster()) {
                    Monster* pMonster = dynamic_cast<Monster*>(pCreature);

                    itr = find_if(m_Enemies.begin(), m_Enemies.end(), StrongerMonster(m_pZone, pMonster->getLevel()));

                    if (itr == m_Enemies.end())
                        m_Enemies.push_back(pMonster->getObjectID());
                    else
                        m_Enemies.insert(itr, pMonster->getObjectID());

                    
                    
                    // m_pZone->getMonsterManager()->addPotentialEnemy(this, pCreature);
                }

                
                if (!m_Enemies.empty() && m_Enemies.size() > getMaxEnemies())
                    m_Enemies.pop_back();
            }

        } break;

        
        case ATTACK_STRONGEST: {
            
            list<ObjectID_t>::iterator itr = find(m_Enemies.begin(), m_Enemies.end(), pCreature->getObjectID());

            if (itr == m_Enemies.end()) {
                if (pCreature->isSlayer()) {
                    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

                    itr = find_if(m_Enemies.begin(), m_Enemies.end(),
                                  WeakerSlayer(m_pZone, pSlayer->getSkillDomainLevelSum()));

                    if (itr == m_Enemies.end())
                        m_Enemies.push_front(pSlayer->getObjectID());
                    else
                        m_Enemies.insert(itr, pSlayer->getObjectID());
                } else if (pCreature->isVampire()) {
                    Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

                    itr = find_if(m_Enemies.begin(), m_Enemies.end(), WeakerVampire(m_pZone, pVampire->getLevel()));

                    if (itr == m_Enemies.end())
                        m_Enemies.push_back(pVampire->getObjectID());
                    else
                        m_Enemies.insert(itr, pVampire->getObjectID());
                } else if (pCreature->isOusters()) {
                    Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

                    itr = find_if(m_Enemies.begin(), m_Enemies.end(), WeakerOusters(m_pZone, pOusters->getLevel()));

                    if (itr == m_Enemies.end())
                        m_Enemies.push_back(pOusters->getObjectID());
                    else
                        m_Enemies.insert(itr, pOusters->getObjectID());
                }
                
                else if (pCreature->isMonster()) {
                    Monster* pMonster = dynamic_cast<Monster*>(pCreature);

                    itr = find_if(m_Enemies.begin(), m_Enemies.end(), WeakerMonster(m_pZone, pMonster->getLevel()));

                    if (itr == m_Enemies.end())
                        m_Enemies.push_back(pMonster->getObjectID());
                    else
                        m_Enemies.insert(itr, pMonster->getObjectID());

                    
                    
                    // m_pZone->getMonsterManager()->addPotentialEnemy(this, pCreature);
                }

                
                if (!m_Enemies.empty() && m_Enemies.size() > getMaxEnemies())
                    m_Enemies.pop_back();
            }
        } break;

        
        
        
        
        case ATTACK_CLOSEST: {
            
            list<ObjectID_t>::iterator itr = find(m_Enemies.begin(), m_Enemies.end(), pCreature->getObjectID());
            if (itr == m_Enemies.end()) {
                
                m_Enemies.push_back(pCreature->getObjectID());

                
                if (m_Enemies.size() > getMaxEnemies()) {
                    
                    list<ObjectID_t>::iterator farest = m_Enemies.end();
                    Distance_t dist = 0;

                    for (itr = m_Enemies.begin(); itr != m_Enemies.end(); itr++) {
                        Creature* pEnemy = NULL;

                        /*
                        try
                        {
                            pEnemy = m_pZone->getCreature(*itr);
                        }
                        catch (NoSuchElementException& nsee)
                        {
                            //cerr << nsee.toString() << endl;
                            pEnemy = NULL;
                        }
                        */

                        
                        pEnemy = m_pZone->getCreature(*itr);

                        
                        if (pEnemy == NULL) {
                            dist = 255;
                            farest = itr;

                            
                            break;
                        } else {
                            Distance_t curDist = getDistance(pEnemy->getX(), pEnemy->getY());
                            if (dist < curDist) {
                                dist = curDist;
                                farest = itr;
                            }
                        }
                    }

                    
                    // Assert(farest != m_Enemies.end());

                    
                    if (farest != m_Enemies.end())
                        m_Enemies.erase(farest);
                }
            }
        } break;

        
        case ATTACK_FAREST: {
            
            list<ObjectID_t>::iterator itr = find(m_Enemies.begin(), m_Enemies.end(), pCreature->getObjectID());
            if (itr == m_Enemies.end()) {
                
                m_Enemies.push_back(pCreature->getObjectID());

                
                if (m_Enemies.size() > getMaxEnemies()) {
                    
                    list<ObjectID_t>::iterator closest = m_Enemies.end();
                    Distance_t dist = 255;

                    for (itr = m_Enemies.begin(); itr != m_Enemies.end(); itr++) {
                        Creature* pEnemy = NULL;

                        /*
                        try
                        {
                            pEnemy = m_pZone->getCreature(*itr);
                        }
                        catch (NoSuchElementException& nsee)
                        {
                            //cerr << nsee.toString() << endl;
                            pEnemy = NULL;
                        }
                        */

                        
                        pEnemy = m_pZone->getCreature(*itr);

                        
                        if (pEnemy == NULL) {
                            dist = 0;
                            closest = itr;

                            
                            break;
                        } else {
                            Distance_t curDist = getDistance(pEnemy->getX(), pEnemy->getY());
                            if (dist > curDist) {
                                dist = curDist;
                                closest = itr;
                            }
                        }
                    }

                    
                    if (closest != m_Enemies.end())
                        m_Enemies.erase(closest);
                }
            }
        } break;

        
        case ATTACK_FIGHTER: {
            throw UnsupportedError(__PRETTY_FUNCTION__);

            
            m_Enemies.push_back(pCreature->getObjectID());

            
            if (m_Enemies.size() > getMaxEnemies()) {
                
                for (list<ObjectID_t>::iterator itr = m_Enemies.begin(); itr != m_Enemies.end(); itr++) {
                    // Creature* pCreature = m_pZone->getCreature(*itr);
                }
            }
        } break;

        
        case ATTACK_PRIEST: {
            throw UnsupportedError(__PRETTY_FUNCTION__);

            
            m_Enemies.push_back(pCreature->getObjectID());

            
            if (m_Enemies.size() > getMaxEnemies()) {
                
                for (list<ObjectID_t>::iterator itr = m_Enemies.begin(); itr != m_Enemies.end(); itr++) {
                    // Creature* pCreature = m_pZone->getCreature(*itr);
                }
            }
        } break;

        
        case ATTACK_GUNNER: {
            throw UnsupportedError(__PRETTY_FUNCTION__);

            
            m_Enemies.push_back(pCreature->getObjectID());

            
            if (m_Enemies.size() > getMaxEnemies()) {
                
                for (list<ObjectID_t>::iterator itr = m_Enemies.begin(); itr != m_Enemies.end(); itr++) {
                    // Creature* pCreature = m_pZone->getCreature(*itr);
                }
            }
        } break;

        
        default:

        {
            
            
            list<ObjectID_t>::iterator itr = find(m_Enemies.begin(), m_Enemies.end(), pCreature->getObjectID());
            if (itr == m_Enemies.end()) {
                m_Enemies.push_back(pCreature->getObjectID());
                if (m_Enemies.size() > getMaxEnemies()) {
                    m_Enemies.pop_back();
                }
            }
        } break;
            // throw Error("illegal attack order");
        }
    } catch (Throwable& t) {
        // cerr << t.toString() << endl;
    }

    __END_DEBUG
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
void Monster::addPotentialEnemy(Creature* pCreature)

{
    __BEGIN_TRY

    Assert(pCreature != NULL);

    
    if (this == pCreature || !isRealEnemy(pCreature))
        return;
    
    if (!isFlag(Effect::EFFECT_CLASS_HALLUCINATION) && pCreature->getClanType() == m_ClanType)
        return;

    
    if (isFlag(Effect::EFFECT_CLASS_PEACE)) {
        //		Effect* pEffect = pMonster->getEffectManager()->findEffect(EFFECT_CLASS_PEACE);
        Effect* pEffect = m_pEffectManager->findEffect(Effect::EFFECT_CLASS_PEACE);

        EffectPeace* pEffectPeace = dynamic_cast<EffectPeace*>(pEffect);

        
        if (pCreature->getObjectID() == pEffectPeace->getPeaceCreatureID()) {
            return;
        }
    }

    
    list<ObjectID_t>::iterator itr = find(m_Enemies.begin(), m_Enemies.end(), pCreature->getObjectID());
    if (itr == m_Enemies.end() && m_Enemies.size() < getMaxEnemies()) {
        m_Enemies.push_back(pCreature->getObjectID());

        
        
        
        // m_pZone->getMonsterManager()->addPotentialEnemy(this, pCreature);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void Monster::deleteEnemy(ObjectID_t enemyID) {
    __BEGIN_TRY

    list<ObjectID_t>::iterator itr = find(m_Enemies.begin(), m_Enemies.end(), enemyID);

    if (itr == m_Enemies.end()) {
        // cerr << "Monster::deleteEnemy() : NoSuchElementException" << endl;
        // throw NoSuchElementException();

        
        return;
    }

    m_Enemies.erase(itr);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void Monster::verifyEnemies()

{
    __BEGIN_TRY

    list<ObjectID_t>::iterator before = m_Enemies.end();
    list<ObjectID_t>::iterator current = m_Enemies.begin();
    Creature* pEnemy = NULL;

    while (current != m_Enemies.end()) {
        
        pEnemy = m_pZone->getCreature(*current);

        if (pEnemy != NULL) {
            if (!isRealEnemy(pEnemy)) {
                m_Enemies.erase(current);

                if (before == m_Enemies.end()) // first enemy
                {
                    current = m_Enemies.begin();
                } else // !first enemy
                {
                    current = before;
                    current++;
                }
            } else {
                
                before = current++;
            }
        } else {
            
            m_Enemies.erase(current);

            if (before == m_Enemies.end()) // first enemy
            {
                current = m_Enemies.begin();
            } else // !first enemy
            {
                current = before;
                current++;
            }
        }
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
Creature* Monster::getPrimaryEnemy() const

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    // by sigi. 2002.10.7
    if (m_Enemies.empty())
        return NULL;

    Creature* pEnemy = NULL;
    Distance_t dist = 255;
    Creature* pCreature = NULL;
    Distance_t newDist = 255;

    list<ObjectID_t>::const_iterator itr;

    switch (getAttackOrder()) {
    
    
    
    case ATTACK_FIRST:
    case ATTACK_LAST:
    case ATTACK_STRONGEST:
    case ATTACK_WEAKEST:
        /*
        try
        {
            pEnemy = m_pZone->getCreature(m_Enemies.front());
        }
        catch (NoSuchElementException& nsee)
        {
            //cerr << nsee.toString() << endl;
            pEnemy = NULL;
        }
        */

        
        pEnemy = m_pZone->getCreature(m_Enemies.front());

        break;

    
    
    case ATTACK_CLOSEST:
        dist = 255;
        for (itr = m_Enemies.begin(); itr != m_Enemies.end(); itr++) {
            /*
            try
            {
                pCreature = m_pZone->getCreature(*itr);
            }
            catch (NoSuchElementException& nsee)
            {
                //cerr << nsee.toString() << endl;
                pCreature = NULL;
            }
            */

            
            pCreature = m_pZone->getCreature(*itr);

            if (pCreature != NULL) {
                newDist = pCreature->getDistance(m_X, m_Y);

                if (newDist < dist) 
                {
                    dist = newDist;
                    pEnemy = pCreature;
                }
            }
        }
        break;

    case ATTACK_FAREST:
        dist = 0;
        for (itr = m_Enemies.begin(); itr != m_Enemies.end(); itr++) {
            /*
            try
            {
                pCreature = m_pZone->getCreature(*itr);
            }
            catch (NoSuchElementException& nsee)
            {
                //cerr << nsee.toString() << endl;
                pCreature = NULL;
            }
            */

            
            pCreature = m_pZone->getCreature(*itr);

            if (pCreature != NULL) {
                newDist = pCreature->getDistance(m_X, m_Y);

                if (newDist > dist) 
                {
                    dist = newDist;
                    pEnemy = pCreature;
                }
            }
        }
        break;
    case ATTACK_FIGHTER:
    case ATTACK_PRIEST:
    case ATTACK_GUNNER:
    default:
        throw Error("invalid attack order");
    }

    if (pEnemy == NULL) {
        // cerr << "<ENEMY NULL ERROR>" << endl;
        // cerr << "Monster's AttackOrder : " << AttackOrder2String[getAttackOrder()] << endl;
        // cerr << "Enemy's ObjectID : " << m_Enemies.front();
    }

    return pEnemy;

    __END_DEBUG
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
ObjectID_t Monster::getEnemy(EnemyPriority enemyPriority) const {
    __BEGIN_TRY

    EnemyPriority ep = ENEMY_PRIMARY;
    list<ObjectID_t>::const_iterator itr = m_Enemies.begin();

    for (; itr != m_Enemies.end(); itr++) {
        if (enemyPriority == ep)
            return *itr;
        ep = EnemyPriority(ep + 1);
    }

    cerr << "Monster::getEnemy() : NoSuchElementException" << endl;
    throw NoSuchElementException();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//
// *CAUTION*
//


//
//   0 -  30     : ENEMY_PRIORITY
//  30 -  60     : ENEMY_SECONDARY
//  60 -  90     : ENEMY_THIRD
//  90 - 120     : ENEMY_FOURTH
// 120 - 150     : ENEMY_FIFTH
// 150 - 180     : ENEMY_SIXTH
// 180 - 210     : ENEMY_SEVENTH
// 210 - maxAttr : ENEMY_EIGHTH
//////////////////////////////////////////////////////////////////////////////
uint Monster::getMaxEnemies() const

{
    return (m_INT < 210) ? (m_INT / 30 + 1) : 8;
}

//////////////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////////////
string Monster::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    __BEGIN_DEBUG

    msg << "Monster("
        << "ObjectID:" << (int)getObjectID() << ",MonsterType:" << (int)m_MonsterType
        << ",SpriteType:" << (int)getSpriteType() << ",Name:" << getName() << ",Level:" << (int)getLevel()
        << ",STR:" << (int)m_STR << ",DEX:" << (int)m_DEX << ",INT:" << (int)m_INT << ",HP:" << (int)m_HP[ATTR_CURRENT]
        << "/" << (int)m_HP[ATTR_MAX] << ",MainColor:" << (int)getMainColor() << ",SubColor:" << (int)getSubColor()
        << ",Alignment:" << MAlignment2String[getAlignment()] << ",AttackOrder:" << AttackOrder2String[getAttackOrder()]
        << ",Moral:" << (int)m_Moral << ",Delay:" << (int)m_Delay << ",Enemies(";

    for (list<ObjectID_t>::const_iterator itr = m_Enemies.begin(); itr != m_Enemies.end(); itr++)
        msg << (int)(*itr) << " ";

    msg << ")"
        << ",X:" << (int)m_X << ",Y:" << (int)m_Y << ",Sight :" << (int)m_Sight << ")";

    __END_DEBUG

    return msg.toString();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void Monster::setDamaged(bool value)

{
    __BEGIN_TRY

    
    if (m_pBrain != NULL) {
        m_pBrain->setDamaged(value);
    }

    __END_CATCH
}

void Monster::addPrecedence(const string& Name, int PartyID, int damage) {
    m_PrecedenceTable.addPrecedence(Name, PartyID, damage);
}

int Monster::getOustersExp(Ousters* pOusters) {
    int total = computeCreatureExp(this, 100);
    double percent = m_PrecedenceTable.getDamagePercent(pOusters->getName(), pOusters->getPartyID());

    //	return (int)((double)total * percent) * 1.4;
    return (int)((double)total * percent);
}

/*
void Monster::addPrecedence(Creature* pCreature, int damage)
{
    Assert(pCreature != NULL);
    m_PrecedenceTable.addPrecedence(pCreature, damage);
}
*/
void Monster::clearEnemyLimitTime() {
    getCurrentTime(m_EnemyLimitTime);

    m_EnemyLimitTime.tv_sec += g_pVariableManager->getEnemyLimitTime();
    m_EnemyLimitTime.tv_usec = 0;
}

bool Monster::hasNextMonsterSummonInfo()

{
    __BEGIN_TRY

    const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(m_MonsterType);
    Assert(pMonsterInfo != NULL);

    if (pMonsterInfo->hasNextMonsterSummonInfo(m_MonsterSummonStep)) {
        return true;
    }

    __END_CATCH

    return false;
}

bool Monster::getMonsterSummonInfo(SUMMON_INFO2& summonInfo)

{
    __BEGIN_TRY

    const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(m_MonsterType);
    Assert(pMonsterInfo != NULL);

    if (pMonsterInfo->getMonsterSummonInfo(m_MonsterSummonStep, summonInfo)) {
        
        m_MonsterSummonStep++;

        return true;
    }

    __END_CATCH

    return false;
}

////////////////////////////////////////////////////////////////////////////////
// isEnemyToAttack
////////////////////////////////////////////////////////////////////////////////

//

////////////////////////////////////////////////////////////////////////////////
bool Monster::isEnemyToAttack(Creature* pCreature) const {
    if (pCreature->isFlag(Effect::EFFECT_CLASS_GHOST))
        return false;

    if (pCreature->getObjectID() == m_OwnerObjectID)
        return false;

    if (m_OwnerObjectID != 0) {
        Creature* pOwner = m_pZone->getCreature(m_OwnerObjectID);
        if (pOwner != NULL && pOwner->getCreatureClass() == pCreature->getCreatureClass() &&
            canAttack(pOwner, pCreature)) {
            //			cout << __PRETTY_FUNCTION__ << pCreature->getName() << "is same race as owner " <<
            // pOwner->getName() << endl;
            return false;
        }

        if (GDRLairManager::Instance().isGDRLairZone(getZoneID()) && pCreature->isPC())
            return false;
    }

    if (m_Flag.test(Effect::EFFECT_CLASS_HALLUCINATION))
        return true;

    if (pCreature->isDead() || pCreature->isFlag(Effect::EFFECT_CLASS_COMA))
        return false;

    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
        return isEnemyToAttack(pSlayer);
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
        return isEnemyToAttack(pVampire);
    } else if (pCreature->isOusters()) {
        
        return true;
    } else if (pCreature->isMonster()) {
        Monster* pMonster = dynamic_cast<Monster*>(pCreature);
        return isEnemyToAttack(pMonster);
    }

    return false;
}


bool Monster::isEnemyToAttack(Slayer* pSlayer) const {
    if (pSlayer->isFlag(Effect::EFFECT_CLASS_GHOST))
        return false;

    if (m_Flag.test(Effect::EFFECT_CLASS_HALLUCINATION))
        return true;

    if (pSlayer->isDead() || pSlayer->isFlag(Effect::EFFECT_CLASS_COMA))
        return false;

    
    return true;
}


bool Monster::isEnemyToAttack(Vampire* pVampire) const {
    if (pVampire->isFlag(Effect::EFFECT_CLASS_GHOST))
        return false;

    if (m_Flag.test(Effect::EFFECT_CLASS_HALLUCINATION))
        return true;

    if (pVampire->isDead() || pVampire->isFlag(Effect::EFFECT_CLASS_COMA))
        return false;

    
    return true;
}


bool Monster::isEnemyToAttack(Monster* pMonster) const {
    if (m_Flag.test(Effect::EFFECT_CLASS_HALLUCINATION))
        return true;

    
    
    
    
    if (pMonster->isDead() || pMonster->isFlag(Effect::EFFECT_CLASS_COMA) ||
        (!pMonster->isFlag(Effect::EFFECT_CLASS_HALLUCINATION) && pMonster->m_ClanType == m_ClanType))
        return false;

    return true;
}

bool Monster::canMove(ZoneCoord_t nx, ZoneCoord_t ny) const {
    return (Creature::canMove(nx, ny) && !(m_pZone->getZoneLevel(nx, ny) & SAFE_ZONE));
}


void Monster::setBrain(MonsterAI* pBrain) {
    removeBrain();
    m_pBrain = pBrain;
}

void Monster::removeBrain() {
    SAFE_DELETE(m_pBrain);
}

void Monster::deleteAllEnemy()

{
    m_Enemies.clear();
}

Race_t Monster::getRace() const {
    
    return RACE_VAMPIRE;
}
