#include "GQuestGiveItemElement.h"

#include "GCCreateItem.h"
#include "GCSystemMessage.h"
#include "GQuestInventory.h"
#include "Inventory.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "PacketUtil.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "Treasure.h"
#include "Zone.h"

GQuestElement::ResultType GQuestGiveItemElement::checkCondition(PlayerCreature* pPC) const {
    //	cout << "GQuestGiveItemElement : " << (int)m_ItemClass << ", " << (int)m_ItemType << endl;
    Item* pItem = g_pItemFactoryManager->createItem(m_ItemClass, m_ItemType, m_Option);
    if (pItem == NULL)
        return FAIL;

    pItem->setNum(m_Num);
    Inventory* pInventory = pPC->getInventory();
    TPOINT pt;
    pPC->getZone()->registerObject(pItem);
    setItemGender(pItem, (pPC->getSex() == FEMALE) ? GENDER_FEMALE : GENDER_MALE);

    if (!pInventory->addItem(pItem, pt)) {
        pt = pPC->getZone()->addItem(pItem, pPC->getX(), pPC->getY());
        if (pt.x == -1) {
            SAFE_DELETE(pItem);
        } else {
            DWORD zoneID = pPC->getZone()->getZoneID();
            pItem->create("", STORAGE_ZONE, zoneID, pt.x, pt.y);
        }
    } else {
        pItem->create(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y);
        GCCreateItem gcCreateItem;
        makeGCCreateItem(&gcCreateItem, pItem, pt.x, pt.y);
        pPC->getPlayer()->sendPacket(&gcCreateItem);


        if (pItem->isTraceItem()) {
            remainTraceLog(pItem, "GQuest", pPC->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
            remainTraceLogNew(pItem, pPC->getName(), ITL_GET, ITLD_EVENTNPC, pPC->getZone()->getZoneID());
        }
    }

    GCSystemMessage gcSM;
    gcSM.setMessage(".");
    pPC->getPlayer()->sendPacket(&gcSM);

    return OK;
}

GQuestGiveItemElement* GQuestGiveItemElement::makeElement(XMLTree* pTree) {
    GQuestGiveItemElement* pRet = new GQuestGiveItemElement;

    // Hoisted out of Assert(): that macro is ((void)0) under NDEBUG, so a
    // Release build would skip both reads. `iClass` would stay empty and
    // `itemType` uninitialised, and this element hands the resulting item to a
    // player -- so the NDEBUG build would mint an arbitrary item type. Both
    // locals are initialised here as well.
    string iClass;
    bool bHasClass = pTree->GetAttribute("class", iClass);
    Assert(bHasClass);

    pRet->m_ItemClass = TreasureItemClass::getItemClassFromString(iClass);

    DWORD itemType = 0;
    bool bHasType = pTree->GetAttribute("type", itemType);
    Assert(bHasType);

    pRet->m_ItemType = (ItemType_t)itemType;

    string option;
    if (pTree->GetAttribute("option", option)) {
        makeOptionList(option, pRet->m_Option);
    }

    // m_Num is ItemNum_t, i.e. BYTE, so the (int&) cast this replaces let
    // GetAttribute's int& overload store four bytes through a one-byte member --
    // three bytes past it, into the object's tail padding. Same out-of-bounds
    // shape as GQuestTouchWayPointElement (18-AA); declaring any member after
    // m_Num in the header would turn it into silent corruption. Read into an int
    // and narrow explicitly. No Assert here, so nothing to hoist.
    int num = 0;
    if (pTree->GetAttribute("num", num))
        pRet->m_Num = (ItemNum_t)num;
    else
        pRet->m_Num = 1;

    return pRet;
}

GQuestGiveItemElement g_GiveItemElement;
