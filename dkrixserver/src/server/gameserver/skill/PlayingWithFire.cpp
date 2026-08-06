//////////////////////////////////////////////////////////////////////////////
// Filename    : PlayingWithFire.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "PlayingWithFire.h"
// #include "GCSkillToSelfOK1.h"
// #include "GCSkillToSelfOK2.h"
#include "SimpleTileMeleeSkill.h"


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
PlayingWithFire::PlayingWithFire() throw() {
    __BEGIN_TRY

    
    m_pPlayingWithFireMask[0].set(1, 1);
    m_pPlayingWithFireMask[1].set(-1, -1);
    m_pPlayingWithFireMask[2].set(0, -1);
    m_pPlayingWithFireMask[3].set(1, -1);
    m_pPlayingWithFireMask[4].set(-1, 0);
    m_pPlayingWithFireMask[5].set(1, 0);
    m_pPlayingWithFireMask[6].set(-1, 1);
    m_pPlayingWithFireMask[7].set(0, 1);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
 

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void PlayingWithFire::execute(Monster* pMonster)

{
    __BEGIN_TRY

    cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

    Assert(pMonster != NULL);

    try {
        Zone* pZone = pMonster->getZone();
        Assert(pZone != NULL);

        if (pMonster->isFlag(Effect::EFFECT_CLASS_HIDE)) {
            // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;
            return;
        }
        if (pMonster->isFlag(Effect::EFFECT_CLASS_INVISIBILITY)) {
            addVisibleCreature(pZone, pMonster, true);
        }

        // GCSkillToSelfOK2 _GCSkillToSelfOK2;

        ZoneCoord_t x = pMonster->getX();
        ZoneCoord_t y = pMonster->getY();

        bool bRangeCheck = checkZoneLevelToUseSkill(pMonster);
        bool bMoveModeCheck = pMonster->isWalking();

        if (bRangeCheck && bMoveModeCheck) {
            //--------------------------------------------------------
            
            //--------------------------------------------------------
            //_GCSkillToSelfOK2.setObjectID(pMonster->getObjectID());
            //_GCSkillToSelfOK2.setDuration(0);
            //_GCSkillToSelfOK2.setSkillType(getSkillType());
            // pZone->broadcastPacket(x, y, &_GCSkillToSelfOK2);


            //--------------------------------------------------------
            
            //--------------------------------------------------------
            SkillInput input(pMonster);
            SkillOutput output;
            computeOutput(input, output);

            SIMPLE_SKILL_INPUT param;
            param.SkillType = getSkillType();
            param.SkillDamage = output.Damage;
            param.Delay = output.Delay;
            param.ItemClass = Item::ITEM_CLASS_MAX;
            param.STRMultiplier = 0;
            param.DEXMultiplier = 0;
            param.INTMultiplier = 0;
            param.bMagicHitRoll = true;
            param.bMagicDamage = true;
            param.bAdd = false;

            SIMPLE_SKILL_OUTPUT result;

            /*	for (int i=0; i<8; i++)
                {
                    param.addMask(m_pPlayingWithFireMask[i].x, m_pPlayingWithFireMask[i].y, 100);
                }
            */
            for (int i = -10; i <= 10; ++i)
                for (int j = -10; j <= 10; ++j)
                    param.addMask(i, j, 100);


            
            bool bForceKnockback = true;
            g_SimpleTileMeleeSkill.execute(pMonster, x, y, param, result, 0, bForceKnockback);
        } else {
            executeSkillFailNormal(pMonster, getSkillType(), NULL);
        }
    } catch (Throwable& t) {
        executeSkillFailException(pMonster, getSkillType());
    }

    // cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

    __END_CATCH
}

PlayingWithFire g_PlayingWithFire;
