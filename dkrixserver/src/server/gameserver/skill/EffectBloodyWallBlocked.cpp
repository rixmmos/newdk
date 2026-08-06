//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBloodyWallBlocked.cpp
// Written by  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectBloodyWallBlocked.h"

#include "GCModifyInformation.h"
#include "GCStatusCurrentHP.h"
#include "GamePlayer.h"
#include "Monster.h"
#include "SkillUtil.h"
#include "Slayer.h"
#include "Vampire.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBloodyWallBlocked::EffectBloodyWallBlocked(Zone* pZone, ZoneCoord_t zoneX, ZoneCoord_t zoneY)

{
    __BEGIN_TRY

    m_pZone = pZone;
    m_X = zoneX;
    m_Y = zoneY;
    m_CasterName = "";
    m_PartyID = 0;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodyWallBlocked::affect()

{
    __BEGIN_TRY

    // cout << "EffectBloodyWallBlocked" << "affect BEGIN" << endl;

    Assert(m_pZone != NULL);

    
    Tile& tile = m_pZone->getTile(m_X, m_Y);

    HP_t CurrentHP = 0;
    HP_t RemainHP = 0;

    
    const forward_list<Object*>& oList = tile.getObjectList();
    forward_list<Object*>::const_iterator itr = oList.begin();
    for (; itr != oList.end(); itr++) {
        Assert(*itr != NULL);

        Object* pObject = *itr;
        Assert(pObject != NULL);

        if (pObject->getObjectClass() == Object::OBJECT_CLASS_CREATURE) {
            Creature* pCreature = dynamic_cast<Creature*>(pObject);
            Assert(pCreature != NULL);

            
            
            if (!canAttack(NULL, pCreature) || pCreature->isFlag(Effect::EFFECT_CLASS_IMMUNE_TO_BLOOD_DRAIN) ||
                pCreature->isFlag(Effect::EFFECT_CLASS_COMA) || pCreature->isDead()) {
                continue;
            }

            int AcidDamage = computeMagicDamage(pCreature, m_Damage, SKILL_BLOODY_WALL);

            if (pCreature->getMoveMode() != Creature::MOVE_MODE_FLYING) {
                if (pCreature->isSlayer()) {
                    Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

                    CurrentHP = pSlayer->getHP(ATTR_CURRENT);
                    RemainHP = max(0, CurrentHP - (int)AcidDamage);

                    pSlayer->setHP(RemainHP, ATTR_CURRENT);

                    GCModifyInformation gcMI;
                    gcMI.addShortData(MODIFY_CURRENT_HP, pSlayer->getHP(ATTR_CURRENT));

                    Player* pPlayer = pSlayer->getPlayer();
                    Assert(pPlayer != NULL);
                    pPlayer->sendPacket(&gcMI);

                    
                    GCStatusCurrentHP pkt;
                    pkt.setObjectID(pSlayer->getObjectID());
                    pkt.setCurrentHP(RemainHP);
                    m_pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &pkt);
                }
                 
                else if (pCreature->isOusters()) {
                    Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

                    CurrentHP = pOusters->getHP(ATTR_CURRENT);
                    RemainHP = max(0, CurrentHP - (int)AcidDamage);

                    pOusters->setHP(RemainHP, ATTR_CURRENT);

                    GCModifyInformation gcMI;
                    gcMI.addShortData(MODIFY_CURRENT_HP, pOusters->getHP(ATTR_CURRENT));

                    Player* pPlayer = pOusters->getPlayer();
                    Assert(pPlayer != NULL);
                    pPlayer->sendPacket(&gcMI);

                    
                    GCStatusCurrentHP pkt;
                    pkt.setObjectID(pOusters->getObjectID());
                    pkt.setCurrentHP(RemainHP);
                    m_pZone->broadcastPacket(pOusters->getX(), pOusters->getY(), &pkt);
                } else if (pCreature->isMonster()) {
                    Monster* pMonster = dynamic_cast<Monster*>(pCreature);

                    CurrentHP = pMonster->getHP(ATTR_CURRENT);
                    RemainHP = max(0, CurrentHP - (int)AcidDamage);

                    pMonster->setHP(RemainHP, ATTR_CURRENT);

                    if (m_CasterName != "") {
                        
                        
                        
                        pMonster->addPrecedence(m_CasterName, m_PartyID, AcidDamage);
                        pMonster->setLastHitCreatureClass(Creature::CREATURE_CLASS_VAMPIRE);
                    }

                    
                    GCStatusCurrentHP pkt;
                    pkt.setObjectID(pMonster->getObjectID());
                    pkt.setCurrentHP(RemainHP);
                    m_pZone->broadcastPacket(pMonster->getX(), pMonster->getY(), &pkt);
                }


                
                // by sigi. 2002.8.31
                if (pCreature->isDead()) {
                    Creature* pAttacker = m_pZone->getCreature(m_CasterName);

                    if (pAttacker != NULL) {
                        affectKillCount(pAttacker, pCreature);
                    }
                }
            }
        }
    }

    
    // setNextTime(m_Tick);

    // cout << "EffectBloodyWallBlocked" << "affect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodyWallBlocked::affect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodyWallBlocked::affect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodyWallBlocked::unaffect(Creature* pCreature)

{
    __BEGIN_TRY
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodyWallBlocked::unaffect()

{
    __BEGIN_TRY

    // cout << "EffectBloodyWallBlocked" << "unaffect BEGIN" << endl;

    Tile& tile = m_pZone->getTile(m_X, m_Y);
    tile.deleteEffect(m_ObjectID);

    // cout << "EffectBloodyWallBlocked" << "unaffect END" << endl;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodyWallBlocked::unaffect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pObject)

    {__BEGIN_TRY __END_CATCH}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBloodyWallBlocked::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;

    msg << "EffectBloodyWallBlocked("
        << "ObjectID:" << getObjectID() << ")";

    return msg.toString();

    __END_CATCH
}

EffectBloodyWallBlockedLoader* g_pEffectBloodyWallBlockedLoader = NULL;
