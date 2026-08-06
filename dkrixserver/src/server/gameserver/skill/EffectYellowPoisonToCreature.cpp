//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectYellowPoisonToCreature.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectYellowPoisonToCreature.h"

#include "DB.h"
#include "DarkLightInfo.h"
#include "GCChangeDarkLight.h"
#include "GCModifyInformation.h"
#include "GCRemoveEffect.h"
#include "Player.h"
#include "Slayer.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectYellowPoisonToCreature::EffectYellowPoisonToCreature(Creature* pCreature)

{
    __BEGIN_TRY
    m_Level = 0;
    setTarget(pCreature);
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoisonToCreature::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoisonToCreature::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoisonToCreature::unaffect(Creature* pCreature)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    //	Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

    if (pCreature != NULL) {
        Zone* pZone = pCreature->getZone();
        Player* pPlayer = pCreature->getPlayer();

        
        
        Sight_t NewSight = 13;
        pCreature->setSight(NewSight);

        
        GCModifyInformation _GCModifyInformation;
        _GCModifyInformation.addShortData(MODIFY_VISION, NewSight);
        pPlayer->sendPacket(&_GCModifyInformation);

        pCreature->removeFlag(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);


        pZone->updateScan(pCreature, YELLOW_POISON_SIGHT, NewSight);

        
        //	DarkLightInfo* pDIInfo    = g_pDarkLightInfoManager->getCurrentDarkLightInfo( pZone );
        // DarkLevel_t    darkLevel  = pDIInfo->getDarkLevel();
        // LightLevel_t   lightLevel = pDIInfo->getLightLevel();

        GCChangeDarkLight gcChangeDarkLight;
        gcChangeDarkLight.setDarkLevel(pZone->getDarkLevel());
        gcChangeDarkLight.setLightLevel(pZone->getLightLevel());

        pPlayer->sendPacket(&gcChangeDarkLight);

         

        
        // pSlayer->tinysave("Sight = 13");

        
        GCRemoveEffect gcRemoveEffect;
        gcRemoveEffect.setObjectID(pCreature->getObjectID());
        gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
        pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

        // destroy(pSlayer->getName());
    }

    __END_DEBUG
    __END_CATCH
}
void EffectYellowPoisonToCreature::unaffect()

{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoisonToCreature::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoisonToCreature::create(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt;

    Timeval currentTime;

    getCurrentTime(currentTime);

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        StringStream sql;

        Turn_t currentYearTime;
        getCurrentYearTime(currentYearTime);

        sql << "INSERT INTO EffectYellowPoisonToCreature"
            << "(OwnerID , YearTime, DayTime, Level, OldSight)"
            << " VALUES('" << ownerID << "' , " << currentYearTime << "," << m_Deadline.tv_sec << "," << (int)m_Level
            << "," << (int)m_OldSight << ")";

        pStmt->executeQueryString(sql.toString());

        delete pStmt;
    }
    END_DB(pStmt)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoisonToCreature::destroy(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        StringStream sql;

        sql << "DELETE FROM EffectYellowPoisonToCreature WHERE OwnerID = '" << ownerID << "'";

        pStmt->executeQueryString(sql.toString());

        delete pStmt;
    }
    END_DB(pStmt)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoisonToCreature::save(const string& ownerID)

{
    __BEGIN_TRY

    Statement* pStmt;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        StringStream sql;

        Turn_t currentYearTime;
        getCurrentYearTime(currentYearTime);

        sql << "UPDATE EffectYellowPoisonToCreature SET "
            << "YearTime = " << currentYearTime << ", DayTime = " << m_Deadline.tv_sec << ", Level = " << (int)m_Level
            << ", OldSight = " << (int)m_OldSight << " WHERE OwnerID = '" << ownerID << "'";

        pStmt->executeQueryString(sql.toString());

        delete pStmt;
    }
    END_DB(pStmt)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectYellowPoisonToCreature::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;

    msg << "EffectYellowPoisonToCreature("
        << "ObjectID:" << getObjectID() << ")";

    return msg.toString();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectYellowPoisonToCreatureLoader::load(Creature* pCreature)

{
    __BEGIN_TRY

    // Assert(pCreature != NULL);
    if (pCreature == NULL) {
        
        return;
    }


    Statement* pStmt;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        StringStream sql;

        sql << "SELECT YearTime, DayTime, Level, OldSight FROM EffectYellowPoisonToCreature"
            << " WHERE OwnerID = '" << pCreature->getName() << "'";

        Result* pResult = pStmt->executeQueryString(sql.toString());

        while (pResult->next()) {
            if (pCreature->isSlayer() || pCreature->isOusters()) {
                uint i = 0;

                Turn_t YearTime = pResult->getDWORD(++i);
                int DayTime = pResult->getDWORD(++i);

                Turn_t currentYearTime;

                Timeval currentTime;

                getCurrentYearTime(currentYearTime);

                getCurrentTime(currentTime);

                int leftTime = ((YearTime - currentYearTime) * 24 * 60 * 60 + (DayTime - currentTime.tv_sec)) * 10;
                EffectYellowPoisonToCreature* pEffect = new EffectYellowPoisonToCreature(pCreature);

                if (leftTime > 0) {
                    pEffect->setDeadline(leftTime);
                } else {
                    pEffect->setDeadline(10);
                    // pEffect->destroy(pCreature->getName());
                    // delete pEffect;
                }

                pEffect->setLevel(pResult->getInt(++i));

                pEffect->setOldSight(13);
                pCreature->setFlag(Effect::EFFECT_CLASS_YELLOW_POISON_TO_CREATURE);
                pCreature->addEffect(pEffect);
            }
        }

        delete pStmt;
    }
    END_DB(pStmt)

    __END_CATCH
}

EffectYellowPoisonToCreatureLoader* g_pEffectYellowPoisonToCreatureLoader = NULL;
