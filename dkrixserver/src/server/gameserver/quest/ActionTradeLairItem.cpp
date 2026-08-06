////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionTradeLairItem.cpp

// Description :




//
//
// History.
//
//  Date        Writer         Description
// ---------- ----------- ------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////

#include "ActionTradeLairItem.h"

#include <list>

#include "DB.h"
#include "GCCreateItem.h"
#include "GCDeleteInventoryItem.h"
#include "GCNPCResponse.h"
#include "GCSystemMessage.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemFactoryManager.h"
#include "ItemGradeManager.h"
#include "ItemMineInfo.h"
#include "ItemUtil.h"
#include "MonsterInfo.h"
#include "Ousters.h"
#include "PacketUtil.h"
#include "PlayerCreature.h"
#include "QuestItem.h"
#include "Slayer.h"
#include "StringPool.h"
#include "SystemAvailabilitiesManager.h"
#include "Thread.h"
#include "Treasure.h"
#include "Vampire.h"
#include "Zone.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ActionTradeLairItem::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY

    try {
        
        m_Type = (ItemType_t)propertyBuffer.getPropertyInt("Type");
    } catch (NoSuchElementException& nsee) {
        throw Error(nsee.toString());
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionTradeLairItem::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature2 != NULL);
    Assert(pCreature1->isNPC());
    Assert(pCreature2->isPC());

    if (m_Type <= 5) {
        SYSTEM_RETURN_IF_NOT(SYSTEM_MASTER_LAIR);
    }

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
    Assert(pPC != NULL);

    Player* pPlayer = pPC->getPlayer();
    Assert(pPlayer != NULL);

    GCNPCResponse okpkt;
    pPlayer->sendPacket(&okpkt);
    StringStream message;

     


    // cout << "ActionTradeLairItem" << ":" << m_Type;

    Inventory* pInventory = pPC->getInventory();

    
    Item* pMasterItem = NULL;
    MonsterType_t MonsterType = 0;

    Item* pItem1 = NULL;
    bool bUpgrade = false;

    
    
    
    

    if (m_Type == 0) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 0);
        MonsterType = BATORI_TYPE;

        if (pMasterItem == NULL) {
            pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 2);
            MonsterType = TEPEZ_TYPE;
            if (pMasterItem == NULL) {
                pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 8); 
                MonsterType = GDR_TYPE;
                bUpgrade = true;
            }
        }
    } else if (m_Type == 1) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 1);
        MonsterType = BATORI_TYPE;

        if (pMasterItem == NULL) {
            pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 3);
            MonsterType = TEPEZ_TYPE;
            if (pMasterItem == NULL) {
                pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 9); 
                MonsterType = GDR_TYPE;
                bUpgrade = true;
            }
        }
    } else if (m_Type == 2) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 2);
        
        
        MonsterType = TEPEZ_TYPE;
    } else if (m_Type == 3) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 3);
        
        
        MonsterType = TEPEZ_TYPE;
    } else if (m_Type == 4) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 0);
        
        
        MonsterType = BATORI_TYPE;
    } else if (m_Type == 5) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 1);
        
        
        MonsterType = BATORI_TYPE;
    } else if (m_Type == 10) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 8);
        bUpgrade = true;
        MonsterType = GDR_TYPE;
    } else if (m_Type == 11) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 9);
        bUpgrade = true;
        MonsterType = GDR_TYPE;
    } else if (m_Type == 6) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 4);
        MonsterType = BATORI_TYPE;
    } else if (m_Type == 7) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 5);
        ItemMineInfo* pItemMineInfo;

        if (pPC->isSlayer()) {
            Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
            Assert(pSlayer != NULL);

            Attr_t totalAttr = pSlayer->getTotalAttr(ATTR_BASIC);

            if (totalAttr <= 130) 
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(0);
            else if (totalAttr <= 210)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(1);
            else if (totalAttr <= 270)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(2);
            else if (totalAttr <= 300)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(3);
            else
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(4);
        } else if (pPC->isVampire()) {
            Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
            Assert(pVampire != NULL);

            Level_t level = pVampire->getLevel();

            if (level <= 20)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(5);
            else if (level <= 40)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(6);
            else if (level <= 60)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(7);
            else if (level <= 90)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(8);
            else
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(9);
        } else if (pPC->isOusters()) {
            Ousters* pOusters = dynamic_cast<Ousters*>(pPC);
            Assert(pOusters != NULL);

            Level_t level = pOusters->getLevel();

            if (level <= 20)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(10);
            else if (level <= 40)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(11);
            else if (level <= 60)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(12);
            else if (level <= 90)
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(13);
            else
                pItemMineInfo = g_pItemMineInfoManager->getItemMineInfo(14);
        } else
            Assert(false);

        pItem1 = pItemMineInfo->getItem();
        setItemGender(pItem1, (pPC->getSex() == FEMALE) ? GENDER_FEMALE : GENDER_MALE);
    } else if (m_Type == 8) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 6);

        if (pPC->isSlayer()) {
            pItem1 = g_pItemMineInfoManager->getRandomItem(15, 32);
        } else if (pPC->isVampire()) {
            pItem1 = g_pItemMineInfoManager->getRandomItem(33, 45);
        } else if (pPC->isOusters()) {
            pItem1 = g_pItemMineInfoManager->getRandomItem(46, 61);
        }

        setItemGender(pItem1, (pPC->getSex() == FEMALE) ? GENDER_FEMALE : GENDER_MALE);
    } else if (m_Type == 9) 
    {
        pMasterItem = pInventory->findItem(Item::ITEM_CLASS_QUEST_ITEM, 7);

        if (pPC->isSlayer()) {
            pItem1 = g_pItemMineInfoManager->getRandomItem(62, 81);
        } else if (pPC->isVampire()) {
            pItem1 = g_pItemMineInfoManager->getRandomItem(82, 96);
        } else if (pPC->isOusters()) {
            pItem1 = g_pItemMineInfoManager->getRandomItem(97, 112);
        }
    } else {
        
        
        GCSystemMessage gcSystemMessage;
        gcSystemMessage.setMessage(g_pStringPool->getString(STRID_NPC_ERROR));
        pPlayer->sendPacket(&gcSystemMessage);

        GCNPCResponse response;
        response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        pPlayer->sendPacket(&response);
        return;
    }

    if (pMasterItem == NULL) {
        GCSystemMessage gcSystemMessage;
        gcSystemMessage.setMessage(g_pStringPool->getString(STRID_NO_LAIR_ITEM));
        pPlayer->sendPacket(&gcSystemMessage);

        GCNPCResponse response;
        response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        pPlayer->sendPacket(&response);

        return;
    }

    Zone* pZone = pPC->getZone();

    
    if (pItem1 == NULL) {
        QuestItemInfo* pItemInfo =
            dynamic_cast<QuestItemInfo*>(g_pQuestItemInfoManager->getItemInfo(pMasterItem->getItemType()));
        Assert(pItemInfo != NULL);

        ///////////////////////////////////////////////////////////////////////////////
        
        
        
        //////////////////////////////////////////////////////////////////////////////
        const MonsterInfo* pMonsterInfo = g_pMonsterInfoManager->getMonsterInfo(MonsterType);
        TreasureList* pTreasureList = NULL;

        
        if (pCreature2->isSlayer())
            pTreasureList = pMonsterInfo->getSlayerTreasureList();
        else if (pCreature2->isVampire())
            pTreasureList = pMonsterInfo->getVampireTreasureList();
        else if (pCreature2->isOusters())
            pTreasureList = pMonsterInfo->getOustersTreasureList();

        const list<Treasure*>& treasures = pTreasureList->getTreasures();

        list<Treasure*>::const_iterator itr = treasures.begin();

        ITEM_TEMPLATE it;

        for (; itr != treasures.end(); itr++) {
            Treasure* pTreasure = (*itr);

            it.ItemClass = Item::ITEM_CLASS_MAX;
            it.ItemType = 0;

            
            it.NextOptionRatio = pItemInfo->getBonusRatio();

            // cout << "TradeLairItem: BonusRatio = " << it.NextOptionRatio << endl;

            if (pTreasure->getRandomItem(&it)) {
                /*				if ( bUpgrade && isPossibleUpgradeItemType( it.ItemClass ) )
                                {
                                    it.ItemType = getUpgradeItemType( it.ItemClass, it.ItemType, 1 );
                                }
                */
                pItem1 = g_pItemFactoryManager->createItem(it.ItemClass, it.ItemType, it.OptionType);
                Assert(pItem1 != NULL);
            }
        }

        if (pItem1 == NULL) {
            StringStream msg;
            msg << "ActionTradeLairItem: " << (int)it.ItemClass << ", " << (int)it.ItemType << ", "
                << (int)it.bCreateOption << ", " << getOptionTypeToString(it.OptionType);

            filelog("tradeLairItemBUG.txt", "%s", msg.toString().c_str());

            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(g_pStringPool->getString(STRID_ITEM_CREATE_ERROR));
            pPlayer->sendPacket(&gcSystemMessage);

            GCNPCResponse response;
            response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
            pPlayer->sendPacket(&response);

            return;
        }
    }

    TPOINT pt;

    pItem1->setGrade(min(7, ItemGradeManager::Instance().getRandomBeadGrade()));
    pZone->registerObject(pItem1);

    
    if (pInventory->addItem(pItem1, pt)) {
        pItem1->create(pPC->getName(), STORAGE_INVENTORY, 0, pt.x, pt.y);

        GCCreateItem gcCreateItem;
        /*		gcCreateItem.setObjectID(pItem1->getObjectID());
                gcCreateItem.setItemClass(pItem1->getItemClass());
                gcCreateItem.setItemType(pItem1->getItemType());
                gcCreateItem.setOptionType(pItem1->getOptionTypeList());
                gcCreateItem.setDurability(pItem1->getDurability());
                gcCreateItem.setItemNum(pItem1->getNum());
                gcCreateItem.setInvenX(pt.x);
                gcCreateItem.setInvenY(pt.y);
                gcCreateItem.setGrade( pItem1->getGrade() );*/

        makeGCCreateItem(&gcCreateItem, pItem1, pt.x, pt.y);

        pPlayer->sendPacket(&gcCreateItem);

        
        if (pItem1 != NULL && pItem1->isTraceItem()) {
            remainTraceLog(pItem1, pCreature1->getName(), pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
        }

        
        GCDeleteInventoryItem gcDeleteInventoryItem;
        gcDeleteInventoryItem.setObjectID(pMasterItem->getObjectID());
        pPlayer->sendPacket(&gcDeleteInventoryItem);

        
        pInventory->deleteItem(pMasterItem->getObjectID()); 
        
        if (pMasterItem != NULL && pMasterItem->isTraceItem()) {
            remainTraceLog(pMasterItem, pCreature2->getName(), pCreature1->getName(), ITEM_LOG_DELETE, DETAIL_EVENTNPC);
        }
        pMasterItem->destroy();
        SAFE_DELETE(pMasterItem);

        
        //		StringStream message;
        
        GCSystemMessage gcSystemMessage;
        gcSystemMessage.setMessage(g_pStringPool->getString(STRID_TRADE_SUCCESS));
        pPlayer->sendPacket(&gcSystemMessage);
    } else {
        //		StringStream buf;
        

        GCSystemMessage gcSystemMessage;
        gcSystemMessage.setMessage(g_pStringPool->getString(STRID_NOT_ENOUGH_INVENTORY_SPACE));
        pPlayer->sendPacket(&gcSystemMessage);
    }

    GCNPCResponse response;
    response.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
    pPlayer->sendPacket(&response);

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionTradeLairItem::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionTradeLairItem("
        << ")";
    return msg.toString();

    __END_CATCH
}
