//////////////////////////////////////////////////////////////////////////////
// Filename    : CGResurrectHandler.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGResurrect.h"

#ifdef __GAME_SERVER__
#include "Effect.h"
#include "EffectComa.h"
#include "EffectManager.h"
#include "GamePlayer.h"
#include "Ousters.h"
#include "Slayer.h"
#include "Vampire.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGResurrectHandler::execute(CGResurrect* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    Creature* pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    if (pCreature->findEffect(Effect::EFFECT_CLASS_ETERNITY))
        return;

    
    if (pCreature->isFlag(Effect::EFFECT_CLASS_COMA)) {
        

        
        EffectManager* pEffectManager = pCreature->getEffectManager();
        Assert(pEffectManager != NULL);

        EffectComa* pEffectComa = (EffectComa*)(pEffectManager->findEffect(Effect::EFFECT_CLASS_COMA));
        Assert(pEffectComa != NULL);

        
        if (pEffectComa->canResurrect()) {
            

             

            
            
            pEffectComa->setDeadline(0);
        } else {
            
        }
    } else {
        

        
        
        
        
        if (pCreature->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
            pSlayer->setHP(0, ATTR_CURRENT);
            pSlayer->deleteEffect(Effect::EFFECT_CLASS_COMA);

            EffectComa* pEffectComa = new EffectComa(pSlayer);
            pEffectComa->setStartTime();
            pEffectComa->setDeadline(0);
            pSlayer->addEffect(pEffectComa);
            pSlayer->setFlag(Effect::EFFECT_CLASS_COMA);
        } else if (pCreature->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
            pVampire->setHP(0, ATTR_CURRENT);
            pVampire->deleteEffect(Effect::EFFECT_CLASS_COMA);

            EffectComa* pEffectComa = new EffectComa(pVampire);
            pEffectComa->setStartTime();
            pEffectComa->setDeadline(0);
            pVampire->addEffect(pEffectComa);
            pVampire->setFlag(Effect::EFFECT_CLASS_COMA);
        } else if (pCreature->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
            pOusters->setHP(0, ATTR_CURRENT);
            pOusters->deleteEffect(Effect::EFFECT_CLASS_COMA);

            EffectComa* pEffectComa = new EffectComa(pOusters);
            pEffectComa->setStartTime();
            pEffectComa->setDeadline(0);
            pOusters->addEffect(pEffectComa);
            pOusters->setFlag(Effect::EFFECT_CLASS_COMA);
        } else {
            Assert(false);
        }
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
