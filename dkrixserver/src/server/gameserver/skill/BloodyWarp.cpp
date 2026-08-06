//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyWarp.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "BloodyWarp.h"

#include "SkillHandlerManager.h"
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
POINT
BloodyWarp::getWarpPosition(int myX, int myY, int targetX, int targetY) throw() {
    __BEGIN_TRY

    POINT pt;

    
    /*	if (rand()%10==0)
        {
            pt.x = myX + rand()%9 - 4;
            pt.y = myY + rand()%9 - 4;
        }
        else*/
    {
        int stepX = targetX - myX;
        int stepY = targetY - myY;
        int signX = (stepX > 0 ? 1 : stepX < 0 ? -1 : 0);
        int signY = (stepY > 0 ? 1 : stepY < 0 ? -1 : 0);
        int cx = targetX + signX * (2 + rand() % 2);
        int cy = targetY + signY * (2 + rand() % 2);

        pt.x = cx;
        pt.y = cy;
    }

    return pt;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void BloodyWarp::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot,
                         CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

    Assert(pVampire != NULL);
    Assert(pVampireSkillSlot != NULL);

    try {
        Zone* pZone = pVampire->getZone();
        Assert(pZone != NULL);

        Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
        Assert(pTargetCreature != NULL);

        
        if (pTargetCreature == NULL) {
            executeSkillFailException(pVampire, getSkillType());

            return;
        }

        execute(pVampire, pTargetCreature->getX(), pTargetCreature->getY(), pVampireSkillSlot, CEffectID);
    } catch (Throwable& t) {
        executeSkillFailException(pVampire, getSkillType());
        // cout << t.toString() << endl;
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void BloodyWarp::execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot,
                         CEffectID_t CEffectID)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

    /*
    SkillInput input(pVampire);
    SkillOutput output;
    computeOutput(input, output);

    int myX = pVampire->getX();
    int myY = pVampire->getY();

    POINT& pt = getWarpPosition(myX, myY, X, Y );

    if (moveFastPC( pVampire, myX, myY, X, Y ))
    {
        decreaseMana(pVampire, RequiredMP, _GCSkillToObjectOK1);
    }
    else
    */
    {
        executeSkillFailNormal(pVampire, getSkillType(), NULL);
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void BloodyWarp::execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y)

{
    __BEGIN_TRY

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;
    Zone* pZone = pMonster->getZone();
    Assert(pZone != NULL);

    SkillInput input(pMonster);
    SkillOutput output;
    computeOutput(input, output);

    int myX = pMonster->getX();
    int myY = pMonster->getY();

    POINT pt = getWarpPosition(myX, myY, X, Y);

    
    SkillType_t SkillType = SKILL_BLOODY_WALL;

    if (pMonster->getMonsterType() >= 717)
        SkillType = SKILL_BLOODY_SNAKE;

    SkillHandler* pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
    Assert(pSkillHandler != NULL);

    pSkillHandler->execute(pMonster, X, Y);


    if (pZone->moveFastMonster(pMonster, myX, myY, pt.x, pt.y, getSkillType())) {
        
        SkillType = (pMonster->isMaster() ? SKILL_BLOODY_MASTER_WAVE : SKILL_BLOODY_WAVE);

        pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SkillType);
        Assert(pSkillHandler != NULL);

        pSkillHandler->execute(pMonster, X, Y);
    } else {
        executeSkillFailNormal(pMonster, getSkillType(), NULL);
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

    __END_CATCH
}

BloodyWarp g_BloodyWarp;
