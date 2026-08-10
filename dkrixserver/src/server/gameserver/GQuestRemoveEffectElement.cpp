#include "GQuestRemoveEffectElement.h"

#include "PlayerCreature.h"
#include "Zone.h"

GQuestElement::ResultType GQuestRemoveEffectElement::checkCondition(PlayerCreature* pPC) const {
    if (pPC->isFlag(m_EffectClass)) {
        Effect* pEffect = pPC->findEffect(m_EffectClass);
        if (pEffect != NULL) {
            pEffect->setDeadline(0);
        }
    }

    return OK;
}

GQuestRemoveEffectElement* GQuestRemoveEffectElement::makeElement(XMLTree* pTree) {
    GQuestRemoveEffectElement* pRet = new GQuestRemoveEffectElement;

    // Same two fixes as GQuestAddEffectElement: no write through an (int&)
    // aliasing the enum member, and the parse call hoisted out of Assert(),
    // which is ((void)0) under NDEBUG. Guarded so a missing attribute still
    // leaves m_EffectClass at the ctor's EFFECT_CLASS_MAX.
    int effectClass = 0;
    bool bHasEffectClass = pTree->GetAttribute("effectclass", effectClass);
    Assert(bHasEffectClass);
    if (bHasEffectClass)
        pRet->m_EffectClass = (Effect::EffectClass)effectClass;

    return pRet;
}

GQuestRemoveEffectElement g_RemoveEffectElement;
