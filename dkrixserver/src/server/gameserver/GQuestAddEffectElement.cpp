#include "GQuestAddEffectElement.h"

#include "CreatureUtil.h"
#include "PlayerCreature.h"
#include "Zone.h"

GQuestElement::ResultType GQuestAddEffectElement::checkCondition(PlayerCreature* pPC) const {
    if (!pPC->isFlag(m_EffectClass)) {
        addSimpleCreatureEffect(pPC, m_EffectClass);
    }

    return OK;
}

GQuestAddEffectElement* GQuestAddEffectElement::makeElement(XMLTree* pTree) {
    GQuestAddEffectElement* pRet = new GQuestAddEffectElement;

    // (int&) on an enum member is not out of bounds -- Effect::EffectClass has
    // no negative enumerator, so GCC gives it an unsigned int underlying type
    // and the widths match -- but it is still a write through a reference of the
    // wrong type, and it hides the narrowing. Read into an int and convert the
    // value instead of aliasing the member.
    //
    // The call also must not live inside Assert(), which is ((void)0) under
    // NDEBUG: a Release build would skip the parse and leave m_EffectClass at
    // the ctor's EFFECT_CLASS_MAX. Guarded so that stays true when the attribute
    // really is missing.
    int effectClass = 0;
    bool bHasEffectClass = pTree->GetAttribute("effectclass", effectClass);
    Assert(bHasEffectClass);
    if (bHasEffectClass)
        pRet->m_EffectClass = (Effect::EffectClass)effectClass;

    return pRet;
}

GQuestAddEffectElement g_AddEffectElement;
