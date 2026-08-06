//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectProminence.cpp
// Written by  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectProminence.h"

#include "DB.h"
#include "Effect.h"
#include "GCModifyInformation.h"
#include "GCStatusCurrentHP.h"
#include "GamePlayer.h"
#include "Monster.h"
#include "Ousters.h"
#include "SkillUtil.h"
#include "Slayer.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectProminence::EffectProminence(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY)

{
    __BEGIN_TRY

    m_pZone = pZone;
    m_X = zoneX;
    m_Y = zoneY;
    m_UserObjectID = 0;
    m_SendEffectClass = getEffectClass();
    m_bForce = false;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectProminence::affect()

{
    __BEGIN_TRY

    // cout << "EffectProminence" << "affect BEGIN" << endl;

    Assert(m_pZone != NULL);

    
    
    Creature* pCastCreature = m_pZone->getCreature(m_UserObjectID);

    if (pCastCreature == NULL && !isForce()) {
        setNextTime(m_Tick);

        return;
    }

    
    Tile& tile = m_pZone->getTile(m_X, m_Y);

    
    const forward_list<Object*>& oList = tile.getObjectList();
    forward_list<Object*>::const_iterator itr = oList.begin();
    for (; itr != oList.end(); itr++) {
        Assert(*itr != NULL);

        Object* pObject = *itr;
        Assert(pObject != NULL);

        if (pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE) {
            Creature* pCreature = dynamic_cast<Creature*>(pObject);
            Assert(pCreature != NULL);

            
            
            if (pCastCreature != NULL &&
                (!canAttack(pCastCreature, pCreature) || pCreature->isFlag(Effect::EFFECT_CLASS_COMA) ||
                 !canHit(pCastCreature, pCreature, SKILL_PROMINENCE, getLevel()))) {
                continue;
            }

            // 2003.1.10 by Sequoia
            
            if (!checkZoneLevelToHitTarget(pCreature))
                continue;

            if (pCreature->getMoveMode() != Creature::MOVE_MODE_FLYING) {
                GCModifyInformation gcAttackerMI;
                GCModifyInformation gcDefenderMI;

                if (pCreature->isSlayer()) {
                    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

                    ::setDamage(pSlayer, m_Damage, pCastCreature, SKILL_PROMINENCE, &gcDefenderMI, &gcAttackerMI, true,
                                false);

                    Player* pPlayer = pSlayer->getPlayer();
                    Assert(pPlayer != NULL);
                    pPlayer->sendPacket(&gcDefenderMI);
                } else if (pCreature->isVampire()) {
                    Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

                    ::setDamage(pVampire, m_Damage, pCastCreature, SKILL_PROMINENCE, &gcDefenderMI, &gcAttackerMI, true,
                                false);

                    Player* pPlayer = pVampire->getPlayer();
                    Assert(pPlayer != NULL);
                    pPlayer->sendPacket(&gcDefenderMI);
                } else if (pCreature->isMonster()) {
                    Monster* pMonster = dynamic_cast<Monster*>(pCreature);

                    ::setDamage(pMonster, m_Damage, pCastCreature, SKILL_PROMINENCE, NULL, &gcAttackerMI, true, false);
                } else if (pCreature->isOusters() && isForce()) {
                    Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

                    ::setDamage(pOusters, m_Damage, pCastCreature, SKILL_PROMINENCE, &gcDefenderMI, &gcAttackerMI, true,
                                false);

                    Player* pPlayer = pOusters->getPlayer();
                    Assert(pPlayer != NULL);
                    pPlayer->sendPacket(&gcDefenderMI);
                } else
                    continue; 

                
                if (pCastCreature != NULL) {
                    if (pCreature->isDead() && pCastCreature->isOusters()) {
                        Ousters* pCastOusters = dynamic_cast<Ousters*>(pCastCreature);
                        Assert(pCastOusters != NULL);

                        //						int exp = computeCreatureExp(pCreature, 100, pCastOusters);
                        int exp = computeCreatureExp(pCreature, 70, pCastOusters);
                        shareOustersExp(pCastOusters, exp, gcAttackerMI);
                    }
                }

                
                /*				if ( pCastCreature != NULL
                                    && pCastCreature->isPC()
                                    && pCreature->isPC()
                                )
                                {
                                    computeAlignmentChange( pCreature, m_Damage, pCastCreature, &gcDefenderMI,
                   &gcAttackerMI ); modifiedAttacker = true;
                                }*/

                if (gcAttackerMI.getShortCount() != 0 || gcAttackerMI.getLongCount() != 0)
                    pCastCreature->getPlayer()->sendPacket(&gcAttackerMI);
            }
        }
    }

    setNextTime(m_Tick);

    // cout << "EffectProminence" << "affect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectProminence::unaffect()

{
    __BEGIN_TRY

    // cout << "EffectProminence" << "unaffect BEGIN" << endl;

    Tile& tile = m_pZone->getTile(m_X, m_Y);
    tile.deleteEffect(m_ObjectID);

    // cout << "EffectProminence" << "unaffect END" << endl;

    __END_CATCH
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectProminence::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;

    msg << "EffectProminence("
        << "ObjectID:" << getObjectID() << ")";

    return msg.toString();

    __END_CATCH
}

void EffectProminenceLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pResult = pStmt->executeQuery("SELECT LeftX, TopY, RightX, BottomY, Value1, Value2, Value3 FROM ZoneEffectInfo "
                                      "WHERE ZoneID = %d AND EffectID = %d",
                                      pZone->getZoneID(), (int)Effect::EFFECT_CLASS_PROMINENCE_3);

        while (pResult->next()) {
            int count = 0;

            ZoneCoord_t left = pResult->getInt(++count);
            ZoneCoord_t top = pResult->getInt(++count);
            ZoneCoord_t right = pResult->getInt(++count);
            ZoneCoord_t bottom = pResult->getInt(++count);
            int value1 = pResult->getInt(++count);
            int value2 = pResult->getInt(++count);
            int value3 = pResult->getInt(++count);

            VSRect rect(0, 0, pZone->getWidth() - 1, pZone->getHeight() - 1);

            for (int X = left; X <= right; X++)
                for (int Y = top; Y <= bottom; Y++) {
                    if (rect.ptInRect(X, Y)) {
                        Tile& tile = pZone->getTile(X, Y);
                        if (tile.canAddEffect()) {
                            EffectProminence* pEffect = new EffectProminence(pZone, X, Y);
                            pEffect->setTick(value2);
                            pEffect->setDamage(value3);
                            pEffect->setNextTime(0);
                            pEffect->setForce(true);
                            pEffect->setSendEffectClass(Effect::EFFECT_CLASS_PROMINENCE_3);

                            
                            pZone->registerObject(pEffect);
                            // pZone->addEffect(pEffect);  // REMOVED: Don't add permanent tile effects to Zone
                            tile.addEffect(pEffect);
                        }
                    }
                }
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

EffectProminenceLoader* g_pEffectProminenceLoader = NULL;
