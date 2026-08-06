//////////////////////////////////////////////////////////////////////////////
// Filename    : MonsterAI.h

// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __MONSTERAI_H__
#define __MONSTERAI_H__

#include "Assert.h"
#include "Creature.h"
#include "Directive.h"
#include "Timeval.h"
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
enum {
    SKILL_FAILED_RANGE = 1,
    SKILL_FAILED_RATIO = 2,
    SKILL_FAILED_STATUS = 3,

    SKILL_FAILED_MAX
};

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
enum {
    LAST_ACTION_NONE,
    LAST_ACTION_MOVE,
    LAST_ACTION_SKILL,

    LAST_ACTION_SKILL_MAX
};

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
enum MoveRule {
    MOVE_RULE_NORMAL,
    MOVE_RULE_LEFTWALL,
    MOVE_RULE_RIGHTWALL,

    MOVE_RULE_MAX,
};

//////////////////////////////////////////////////////////////////////////////
// class MonsterAI
//////////////////////////////////////////////////////////////////////////////

class Monster;

typedef bool (*ConditionCheckFunction)(Monster* pMonster, Creature* pEnemy);

class MonsterAI {
public:
public:
    MonsterAI(Monster* pMonster, uint type);
    virtual ~MonsterAI();

public:
    
    bool getDamaged(void) const {
        return m_bDamaged;
    }
    void setDamaged(bool value) {
        m_bDamaged = value;
    }

    
    void addDirective(Directive* pDirective);

    
    bool checkCondition(int condition, Creature* pEnemy);
    bool checkDirective(Directive* pDirective, Creature* pEnemy);

    
    bool move(ZoneCoord_t ex, ZoneCoord_t ey);
    bool move(Creature* pEnemy, bool bRetreat = false);
    bool approach(Creature* pEnemy) {
        return move(pEnemy, false);
    }
    bool flee(Creature* pEnemy);
    int useSkill(Creature* pEnemy, SkillType_t SkillType, int ratio);

    
    bool moveNormal(ZoneCoord_t ex, ZoneCoord_t ey, ZoneCoord_t& nx, ZoneCoord_t& ny, Dir_t& ndir);
    bool moveWall(ZoneCoord_t ex, ZoneCoord_t ey, ZoneCoord_t& nx, ZoneCoord_t& ny, Dir_t& ndir, bool bLeft);
    void setMoveRule(MoveRule mr) {
        m_MoveRule = mr;
        m_WallCount = 0;
    }

    
    MoveRule getMoveRule() const {
        return m_MoveRule;
    }
    int getWallCount() const {
        return m_WallCount;
    }

    
    void deal(Creature* pEnemy, const Timeval& currentTime);
    void actDeadAction(Creature* pEnemy);

    
    void setDelay(const Timeval& currentTime);
    void setAttackDelay(const Timeval& currentTime);

    bool canAttackAir() const {
        return m_pDirectiveSet->canAttackAir();
    }
    bool canSeeSafeZone() const {
        return m_pDirectiveSet->canSeeSafeZone();
    }

private:
    
    Monster* m_pBody;

    
    uint m_AIType;

    
    DirectiveSet* m_pDirectiveSet;

    
    int m_Panic;

    
    int m_PanicMax;

    
    
    int m_Courage;

    
    int m_CourageMax;

    
    bool m_bDamaged;

    
    int m_LastAction;

    
    MoveRule m_MoveRule;

    
    Dir_t m_BlockedDir;

    
    int m_WallCount;

    
    ConditionCheckFunction m_pCondChecker[DIRECTIVE_COND_MAX];
};

bool checkEnemyRangeMelee(Monster* pMonster, Creature* pEnemy);
bool checkEnemyRangeMissile(Monster* pMonster, Creature* pEnemy);
bool checkEnemyRangeClose(Monster* pMonster, Creature* pEnemy);
bool checkEnemyRangeOutOfSight(Monster* pMonster, Creature* pEnemy);
bool checkEnemyDying(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotBloodDrained(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotGreenPoisoned(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotYellowPoisoned(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotDarkbluePoisoned(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotGreenStalkered(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotParalyzed(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotDoomed(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotBlinded(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotInDarkness(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotSeduction(Monster* pMonster, Creature* pEnemy);
bool checkImOK(Monster* pMonster, Creature* pEnemy);
bool checkImDying(Monster* pMonster, Creature* pEnemy);
bool checkImDamaged(Monster* pMonster, Creature* pEnemy);
bool checkImHiding(Monster* pMonster, Creature* pEnemy);
bool checkImWolf(Monster* pMonster, Creature* pEnemy);
bool checkImBat(Monster* pMonster, Creature* pEnemy);
bool checkImInvisible(Monster* pMonster, Creature* pEnemy);
bool checkImWalkingWall(Monster* pMonster, Creature* pEnemy);
bool checkTimingBloodDrain(Monster* pMonster, Creature* pEnemy);
// by sigi. 2002.9.4
bool checkMasterSummonTiming(Monster* pMonster, Creature* pEnemy);
bool checkMasterNotReady(Monster* pMonster, Creature* pEnemy);
bool checkImInBadPosition(Monster* pMonster, Creature* pEnemy);
bool checkFindWeakEnemy(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotDeath(Monster* pMonster, Creature* pEnemy);
bool checkEnemyNotHallucination(Monster* pMonster, Creature* pEnemy);

// by sigi. 2002.9.16
bool checkTimingMasterBloodDrain(Monster* pMonster, Creature* pEnemy);

// by sigi. 2002.9.23
bool checkTimingDuplicateSelf(Monster* pMonster, Creature* pEnemy);

// by sigi. 2002.9.24
bool checkEnemyRangeInMissile(Monster* pMonster, Creature* pEnemy);

// by sigi. 2002.10.1
bool checkPossibleSummonMonsters(Monster* pMonster, Creature* pEnemy);

// by Sequoia. 2003.4.14
bool checkEnemyTileNotAcidSwamp(Monster* pMonster, Creature* pEnemy);

bool checkEnemyOnAir(Monster* pMonster, Creature* pEnemy);
bool checkEnemyOnSafeZone(Monster* pMonster, Creature* pEnemy);
bool checkCanAttackThrowingAxe(Monster* pMonster, Creature* pEnemy);

#endif // __MONSTER_H__
