//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectYellowPoison.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectYellowPoison.h"

#include "DB.h"
#include "EffectLight.h"
#include "EffectProtectionFromPoison.h"
#include "EffectYellowPoisonToCreature.h"
#include "GCAddEffect.h"
#include "GCChangeDarkLight.h"
#include "GCModifyInformation.h"
#include "GCRemoveEffect.h"
#include "HitRoll.h"
#include "Player.h"
#include "SkillUtil.h"
#include "Slayer.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectYellowPoison::EffectYellowPoison(Zone* pZone, ZoneCoord_t ZoneX, ZoneCoord_t ZoneY)

{
    __BEGIN_TRY

    m_pZone = pZone;
    m_X = ZoneX;
    m_Y = ZoneY;

    m_bVampire = false;
    m_bForce = false;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoison::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool EffectYellowPoison::affectCreature(Creature* pTargetCreature, bool bAffectByMove)

{
    __BEGIN_TRY

    Assert(pTargetCreature != NULL);

    
    if (!pTargetCreature->isSlayer() && !pTargetCreature->isOusters() && !isForce())
        return false;
    if (pTargetCreature->getCompetence() != 3)
        return false;

    
    // 2003.1.10 by bezz.Sequoia
    if (!checkZoneLevelToHitTarget(pTargetCreature)) {
        return false;
    }

    Player* pPlayer = pTargetCreature->getPlayer();
    Assert(pPlayer != NULL);

    
    Resist_t resist = pTargetCreature->getResist(MAGIC_DOMAIN_POISON);

    
    if (m_bVampire) {
        if (!HitRoll::isSuccessVampireCurse(m_Level, resist))
            return false;
    } else {
        if (!HitRoll::isSuccessCurse(m_Level, resist))
            return false;
    }

    
    
    
    if (!pTargetCreature->isFlag(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE)) {
        Zone* pZone = pTargetCreature->getZone();

        
        
        if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE)) {
            pTargetCreature->deleteEffect(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
        }

        Sight_t CurrentSight = pTargetCreature->getSight();
        Sight_t oldSight = CurrentSight;

        
        EffectYellowPoisonToCreature* pEffect = new EffectYellowPoisonToCreature(pTargetCreature);
        pEffect->setDeadline(m_Duration);
        pEffect->setOldSight(CurrentSight);
        pEffect->setLevel(m_Level);

        
        pTargetCreature->setFlag(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
        pTargetCreature->addEffect(pEffect);

        
        // pEffect->create(pTargetCreature->getName());

        
        
        
        GCChangeDarkLight gcChangeDarkLight;
        gcChangeDarkLight.setDarkLevel(15);
        gcChangeDarkLight.setLightLevel(1);
        pPlayer->sendPacket(&gcChangeDarkLight);

        pTargetCreature->setSight(pTargetCreature->getEffectedSight());
        GCModifyInformation gcMI;

        
        if (oldSight != pTargetCreature->getSight()) {
            pZone->updateScan(pTargetCreature, oldSight, pTargetCreature->getSight());
            gcMI.addShortData(MODIFY_VISION, pTargetCreature->getSight());
        }

        // send GCModifyInformation for sight change
        gcMI.addShortData(MODIFY_EFFECT_STAT, Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
        gcMI.addLongData(MODIFY_DURATION, m_Duration);
        pPlayer->sendPacket(&gcMI);

        GCAddEffect gcAddEffect;
        gcAddEffect.setObjectID(pTargetCreature->getObjectID());
        gcAddEffect.setEffectID(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
        gcAddEffect.setDuration(m_Duration);

        pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect, pTargetCreature);

        return true;
    }

    return false;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoison::affect(Creature* pTargetCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoison::unaffect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoison::unaffect()

{
    __BEGIN_TRY

    Tile& tile = m_pZone->getTile(m_X, m_Y);
    tile.deleteEffect(m_ObjectID);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoison::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

    {__BEGIN_TRY __END_CATCH}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectYellowPoison::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectYellowPoison("
        << "ObjectID:" << getObjectID() << ")";
    return msg.toString();

    __END_CATCH
}

void EffectYellowPoisonLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pResult = pStmt->executeQuery("SELECT LeftX, TopY, RightX, BottomY, Value1, Value2, Value3 FROM ZoneEffectInfo "
                                      "WHERE ZoneID = %d AND EffectID = %d",
                                      pZone->getZoneID(), (int)Effect::EFFECT_CLASS_YELLOW_POISON);

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
                            EffectYellowPoison* pEffect = new EffectYellowPoison(pZone, X, Y);
                            pEffect->setForce(true);
                            pEffect->setDuration(value1);
                            pEffect->setLevel(100);

                            
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

EffectYellowPoisonLoader* g_pEffectYellowPoisonLoader = NULL;
