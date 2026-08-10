////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionTeachSkill.cpp
// Written By  :
// Description :




////////////////////////////////////////////////////////////////////////////////

#include "ActionTeachSkill.h"

#include "Creature.h"
#include "GCNPCResponse.h"
#include "GCTeachSkillInfo.h"
#include "GamePlayer.h"
#include "NPC.h"
#include "SkillInfo.h"
#include "Slayer.h"
#include "Vampire.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ActionTeachSkill::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY

    try {
        
        string domainType = propertyBuffer.getProperty("DomainType");

        if (domainType == "BLADE")
            m_DomainType = SKILL_DOMAIN_BLADE;
        else if (domainType == "SWORD")
            m_DomainType = SKILL_DOMAIN_SWORD;
        else if (domainType == "GUN")
            m_DomainType = SKILL_DOMAIN_GUN;
        // else if (domainType == "RIFLE")   m_DomainType = SKILL_DOMAIN_RIFLE;
        else if (domainType == "ENCHANT")
            m_DomainType = SKILL_DOMAIN_ENCHANT;
        else if (domainType == "HEAL")
            m_DomainType = SKILL_DOMAIN_HEAL;
        else if (domainType == "ETC")
            m_DomainType = SKILL_DOMAIN_ETC;
        else if (domainType == "VAMPIRE")
            m_DomainType = SKILL_DOMAIN_VAMPIRE;
        else
            throw Error("TeachSkill::read() :   .");
    } catch (NoSuchElementException& nsee) {
        throw Error(nsee.toString());
    }

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionTeachSkill::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature2 != NULL);
    Assert(pCreature1->isNPC());
    Assert(pCreature2->isPC());

    
    GCNPCResponse okpkt;
    Player* pPlayer = pCreature2->getPlayer();
    Assert(pPlayer != NULL);
    pPlayer->sendPacket(&okpkt);

    if (pCreature2->isSlayer())
        executeSlayer(pCreature1, pCreature2);
    else if (pCreature2->isVampire())
        executeVampire(pCreature1, pCreature2);
    else
        throw("ActionTeachSkill::execute() :     !");

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionTeachSkill::executeSlayer(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature2);
    Player* pPlayer = pCreature2->getPlayer();
    GCTeachSkillInfo teachinfo;

    Assert(pPlayer != NULL);

    // execute() dispatches on the *player's* race, not the NPC's, so a Slayer
    // who talks to a Vampire trainer arrives here with m_DomainType ==
    // SKILL_DOMAIN_VAMPIRE -- live data: triggers 672 (Kaim) and 727
    // (Bricolacas) both carry `DomainType : VAMPIRE`. That indexed one past the
    // end of the Slayer's six-entry m_GoalExp and m_SkillDomainLevels below.
    // A Slayer has no Vampire domain, so answer with the "nothing to teach"
    // reply this function already sends when the domain is mid-advancement,
    // rather than letting the bounded accessors throw at a legitimate NPC.
    if (m_DomainType >= SKILL_DOMAIN_VAMPIRE) {
        teachinfo.setDomainType(m_DomainType);
        teachinfo.setTargetLevel(0);
        pPlayer->sendPacket(&teachinfo);
        return;
    }


    if (pSlayer->getGoalExp(m_DomainType) != 0) {
        
        teachinfo.setDomainType(m_DomainType);
        teachinfo.setTargetLevel(0);
        pPlayer->sendPacket(&teachinfo);
        return;
    }

    Level_t DomainLevel = pSlayer->getSkillDomainLevel(m_DomainType);
    SkillType_t SkillType = g_pSkillInfoManager->getSkillTypeByLevel(m_DomainType, DomainLevel);

     

    
    teachinfo.setDomainType(m_DomainType);
    teachinfo.setTargetLevel(SkillType);

    pPlayer->sendPacket(&teachinfo);

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionTeachSkill::executeVampire(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Vampire* pVampire = dynamic_cast<Vampire*>(pCreature2);

     

    Level_t DomainLevel = pVampire->getLevel();

    SkillType_t SkillType = g_pSkillInfoManager->getSkillTypeByLevel(m_DomainType, DomainLevel);


    
    GCTeachSkillInfo teachinfo;
    teachinfo.setDomainType(m_DomainType);
    teachinfo.setTargetLevel(SkillType);

    
    Player* pPlayer = pCreature2->getPlayer();
    Assert(pPlayer != NULL);
    pPlayer->sendPacket(&teachinfo);

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionTeachSkill::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionTeachSkill(" << ")";
    return msg.toString();

    __END_CATCH
}
