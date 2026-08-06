//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGreenPoison.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectGreenPoison.h"

#include "DB.h"
#include "EffectPoison.h"
#include "GCAddEffect.h"
#include "GCRemoveEffect.h"
#include "SkillHandler.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectGreenPoison::EffectGreenPoison(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY)

{
    __BEGIN_TRY

    m_UserObjectID = 0;
    m_pZone = pZone;
    m_X = zoneX;
    m_Y = zoneY;
    m_bVampire = false;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
bool EffectGreenPoison::affectCreature(Creature* pTargetCreature, bool bAffectByMove)

{
    __BEGIN_TRY

    // cout << "EffectGreenPoison " << "affectCreature Begin " << endl;

    Assert(pTargetCreature != NULL);

    
    if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_POISON)) {
        // cout << "EffectGreenPoison " << "affectCreature End " << endl;
        return false;
    }

    
    // 2003.1.10 by bezz.Sequoia
    if (!checkZoneLevelToHitTarget(pTargetCreature)) {
        return false;
    }

    Zone* pZone = pTargetCreature->getZone();

    Creature* pAttacker = pZone->getCreature(m_UserObjectID);
    
    int PoisonDamage = computeMagicDamage(pTargetCreature, m_Damage, SKILL_GREEN_POISON, m_bVampire, pAttacker);

    if (PoisonDamage > 0) {
        
        EffectPoison* pEffectPoison = new EffectPoison(pTargetCreature);
        pEffectPoison->setLevel(m_Level);
        pEffectPoison->setPoint(PoisonDamage);
        pEffectPoison->setDeadline(m_Duration); 
        pEffectPoison->setTick(50);             
        pEffectPoison->setUserObjectID(m_UserObjectID);
        pEffectPoison->affect(pTargetCreature);
        pTargetCreature->addEffect(pEffectPoison);
        pTargetCreature->setFlag(Effect::EFFECT_CLASS_POISON);

        
        GCAddEffect gcAddEffect;
        gcAddEffect.setObjectID(pTargetCreature->getObjectID());
        gcAddEffect.setEffectID(Effect::EFFECT_CLASS_POISON);
        gcAddEffect.setDuration(m_Duration);
        pZone->broadcastPacket(pTargetCreature->getX(), pTargetCreature->getY(), &gcAddEffect);
    }

    // cout << "EffectGreenPoison " << "affectCreature End " << endl;

    return true;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGreenPoison::affect(Creature* pTargetCreature)

{
    __BEGIN_TRY

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGreenPoison::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGreenPoison::unaffect()

{
    __BEGIN_TRY

    // cout << "EffectGreenPoison " << "unaffect BEGIN" << endl;

    Tile& tile = m_pZone->getTile(m_X, m_Y);
    tile.deleteEffect(m_ObjectID);

    // cout << "EffectGreenPoison " << "unaffect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGreenPoison::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

    {__BEGIN_TRY __END_CATCH}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectGreenPoison::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectGreenPoison("
        << "DayTime:" << m_Deadline.tv_sec << ")";
    return msg.toString();

    __END_CATCH
}

void EffectGreenPoisonLoader::load(Zone* pZone)

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pResult = pStmt->executeQuery("SELECT LeftX, TopY, RightX, BottomY, Value1, Value2, Value3 FROM ZoneEffectInfo "
                                      "WHERE ZoneID = %d AND EffectID = %d",
                                      pZone->getZoneID(), (int)Effect::EFFECT_CLASS_GREEN_POISON);

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
                            EffectGreenPoison* pEffect = new EffectGreenPoison(pZone, X, Y);
                            pEffect->setDuration(value1);
                            pEffect->setNextTime(value2);
                            pEffect->setDamage(value3);

                            
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

EffectGreenPoisonLoader* g_pEffectGreenPoisonLoader = NULL;
