//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillProperty.cpp
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "SkillPropertyManager.h"

#include "Skill.h"

//////////////////////////////////////////////////////////////////////////////
// class SkillProperty member methods
//////////////////////////////////////////////////////////////////////////////

const int DefaultSkillMax = 20;
const int DefaultSkill[DefaultSkillMax][4] = {
    // SkillType, bMelee, bMagic, bPhysic
    {SKILL_ATTACK_MELEE, 1, 0, 1},
    {SKILL_ATTACK_ARMS, 0, 0, 1},
    {SKILL_SELF, 0, 0, 0},
    {SKILL_TILE, 0, 0, 0},
    {SKILL_OBJECT, 0, 0, 0},
    {SKILL_UN_BURROW, 0, 0, 0},
    {SKILL_UN_TRANSFORM, 0, 0, 0},
    {SKILL_UN_INVISIBILITY, 0, 0, 0},
    {SKILL_THROW_HOLY_WATER, 0, 0, 0},
    {SKILL_SUMMON_BAT, 0, 0, 0},
    {SKILL_CHARM, 0, 0, 0},
    {SKILL_POLYMORPH, 0, 0, 0},
    {SKILL_HYPNOSIS, 0, 0, 0},
    {SKILL_CURE_EFFECT, 0, 0, 0},
    {SKILL_CRITICAL_EFFECT, 0, 0, 0},
    {SKILL_REFLECTION_EFFECT, 0, 0, 0},
    {SKILL_TEMP, 0, 0, 0},
    {SKILL_DARKNESS_WIDE, 0, 1, 0},
    {SKILL_POISON_STORM_WIDE, 0, 1, 0},
    {SKILL_ACID_STORM_WIDE, 0, 1, 0},
};

SkillProperty::SkillProperty(){__BEGIN_TRY __END_CATCH}

SkillProperty::~SkillProperty(){__BEGIN_TRY __END_CATCH_NO_RETHROW}

string SkillProperty::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "(" << (int)m_SkillType << ":";
    for (int i = 0; i < SKILL_PROPERTY_TYPE_MAX; i++) {
        msg << (m_PropertyType.test(i) ? "1" : "0");
    }
    msg << ")";
    return msg.toString();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// class SkillPropertyManager member methods
//////////////////////////////////////////////////////////////////////////////

SkillPropertyManager::SkillPropertyManager()

    {__BEGIN_TRY

         __END_CATCH}

SkillPropertyManager::~SkillPropertyManager()

{
    __BEGIN_TRY

    clear();

    __END_CATCH_NO_RETHROW
}

void SkillPropertyManager::init()

{
    __BEGIN_TRY

    // reserve() only allocates capacity -- size() stays 0, so every
    // m_SkillProperties[i] below (and in clear(), getSkillProperty() and
    // addSkillProperty()) indexed past the end into unconstructed storage.
    // assign() sizes the vector and NULL-fills it in one step, and is correct
    // if init() is called again after clear().
    m_SkillProperties.assign((size_t)SKILL_MAX, (SkillProperty*)NULL);

    initDefaultSkillProperty();

    __END_CATCH
}

void SkillPropertyManager::clear()

{
    __BEGIN_TRY

    // Bound on the vector's own size, not on SKILL_MAX: clear() also runs from
    // the destructor, which is reachable without init() having sized it.
    for (size_t i = 0; i < m_SkillProperties.size(); i++) {
        SAFE_DELETE(m_SkillProperties[i]);
    }

    m_SkillProperties.clear();

    __END_CATCH
}

void SkillPropertyManager::initDefaultSkillProperty()

{
    __BEGIN_TRY

    for (int i = 0; i < DefaultSkillMax; i++) {
        SkillProperty* pSkillProperty = new SkillProperty();
        pSkillProperty->setType(DefaultSkill[i][0]);
        pSkillProperty->setMelee(DefaultSkill[i][1]);
        pSkillProperty->setMagic(DefaultSkill[i][2]);
        pSkillProperty->setPhysic(DefaultSkill[i][3]);

        addSkillProperty(pSkillProperty);
    }

    __END_CATCH
}

SkillProperty* SkillPropertyManager::getSkillProperty(SkillType_t SkillType) const {
    __BEGIN_TRY

    if (SkillType >= SKILL_MAX) {
        cerr << "SkillPropertyManager::getSkillProperty() : out of bounds" << endl;
        throw OutOfBoundException();
    }

    if (m_SkillProperties[SkillType] == NULL) {
        cerr << "SkillPropertyManager::getSkillProperty() : no such element" << endl;
        throw NoSuchElementException();
    }

    return m_SkillProperties[SkillType];

    __END_CATCH
}

void SkillPropertyManager::addSkillProperty(SkillProperty* pSkillProperty) {
    __BEGIN_TRY

    if (pSkillProperty->getType() >= SKILL_MAX) {
        cerr << "SkillPropertyManager::getSkillProperty() : out of bounds" << endl;
        throw OutOfBoundException();
    }

    if (m_SkillProperties[pSkillProperty->getType()] != NULL) {
        cerr << "SkillPropertyManager::getSkillProperty() : Dup element" << endl;
        throw DuplicatedException();
    }

    m_SkillProperties[pSkillProperty->getType()] = pSkillProperty;

    __END_CATCH
}

// SkillPropertyManager:: toString()
string SkillPropertyManager::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "SkillPropertyManager(\n";

    // Most slots are legitimately NULL -- only the skill types that
    // initDefaultSkillProperty()/addSkillProperty() populate are set.
    for (size_t i = 0; i < m_SkillProperties.size(); i++) {
        if (m_SkillProperties[i] != NULL)
            msg << m_SkillProperties[i]->toString() << ",";
    }

    msg << "\n)";

    return msg.toString();

    __END_CATCH
}

// Global Variable definition
SkillPropertyManager* g_pSkillPropertyManager = NULL;
