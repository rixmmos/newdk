////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionTradeGiftBox.cpp

// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionTradeGiftBox.h"

#include <list>

#include "DB.h"
#include "FlagSet.h"
#include "GCCreateItem.h"
#include "GCDeleteInventoryItem.h"
#include "GCNPCResponse.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "Item.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "OptionInfo.h"
#include "PacketUtil.h"
#include "PlayerCreature.h"
#include "PriceManager.h"
#include "Slayer.h"
#include "Thread.h"
#include "Vampire.h"
#include "Zone.h"

////////////////////////////////////////////////////////////////////////////////
// ActionTradeGiftBox
////////////////////////////////////////////////////////////////////////////////
ActionTradeGiftBox::ActionTradeGiftBox() : m_pLuaState(NULL), m_pLuaSlayerItem(NULL), m_pLuaVampireItem(NULL) {
    m_pLuaState = new LuaState;
    m_pLuaState->init();

    m_pLuaSlayerItem = new LuaTradeEventSlayerItem(m_pLuaState);
    m_pLuaVampireItem = new LuaTradeEventVampireItem(m_pLuaState);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
ActionTradeGiftBox::~ActionTradeGiftBox() {
    __BEGIN_TRY

    SAFE_DELETE(m_pLuaState);
    SAFE_DELETE(m_pLuaSlayerItem);
    SAFE_DELETE(m_pLuaVampireItem);

    __END_CATCH_NO_RETHROW
}

////////////////////////////////////////////////////////////////////////////////
//  load
////////////////////////////////////////////////////////////////////////////////
void ActionTradeGiftBox::load()

{
    __BEGIN_TRY

    Assert(m_pLuaState != NULL);

    int result = m_pLuaState->dofile(m_CommonFilename);
    LuaState::logError(result);

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ActionTradeGiftBox::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY

    try {
        // read script id
        m_CommonFilename = propertyBuffer.getProperty("CommonFilename");
        m_SlayerFilename = propertyBuffer.getProperty("SlayerFilename");
        m_VampireFilename = propertyBuffer.getProperty("VampireFilename");

        load();
    } catch (NoSuchElementException& nsee) {
        throw Error(nsee.toString());
    }

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ActionTradeGiftBox::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature2 != NULL);
    Assert(pCreature1->isNPC());
    Assert(pCreature2->isPC());

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
    Assert(pPC != NULL);

    Player* pPlayer = pPC->getPlayer();
    Assert(pPlayer != NULL);

    Inventory* pInventory = pPC->getInventory();
    Assert(pInventory != NULL);

    Zone* pZone = pPC->getZone();
    Assert(pZone != NULL);

    FlagSet* pFlagSet = pPC->getFlagSet();

    Item::ItemClass ItemClass;
    ItemType_t ItemType;
    OptionType_t OptionType;

    Item* pItem;
    Item* pGiftBoxItem;

    
    if (pFlagSet->isOn(FLAGSET_TRADE_GIFT_BOX_2002_12)) {
        GCNPCResponse response;
        response.setCode(NPC_RESPONSE_TRADE_GIFT_BOX_ALREADY_TRADE);
        pPlayer->sendPacket(&response);

        GCNPCResponse quit;
        quit.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        pPlayer->sendPacket(&quit);

        return;
    }

    
    CoordInven_t X, Y;
    pGiftBoxItem = pInventory->findItem(Item::ITEM_CLASS_EVENT_GIFT_BOX, 1, X, Y);
    if (pGiftBoxItem == NULL) {
        GCNPCResponse response;
        response.setCode(NPC_RESPONSE_TRADE_GIFT_BOX_NO_ITEM);
        pPlayer->sendPacket(&response);

        GCNPCResponse quit;
        quit.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        pPlayer->sendPacket(&quit);

        return;
    }

    LuaSelectItem* pLuaSelectItem = NULL;
    string luaFileName;

    if (pPC->isSlayer()) {
        
        Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
        Assert(pSlayer != NULL);

        Attr_t sum = pSlayer->getSTR(ATTR_BASIC) + pSlayer->getDEX(ATTR_BASIC) + pSlayer->getINT(ATTR_BASIC);

        m_pLuaSlayerItem->setSum(sum);
        pLuaSelectItem = m_pLuaSlayerItem;
        luaFileName = m_SlayerFilename;

    } else if (pPC->isVampire()) {
        
        Vampire* pVampire = dynamic_cast<Vampire*>(pPC);
        Assert(pVampire != NULL);

        int level = pVampire->getLevel();
        m_pLuaVampireItem->setLevel(level);
        pLuaSelectItem = m_pLuaVampireItem;
        luaFileName = m_VampireFilename;
    }

    // Same shape as ActionGiveEventItem: there is no Ousters branch, no Ousters
    // LuaSelectItem and no OustersFilename in the trigger data, so an Ousters
    // reached prepare() through a NULL pointer and crashed the gameserver. Fail
    // safe -- end the dialogue without consuming the gift box, exactly like the
    // invalid-ItemClass path below.
    if (pLuaSelectItem == NULL) {
        GCNPCResponse quit;
        quit.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        pPlayer->sendPacket(&quit);

        return;
    }

    pLuaSelectItem->prepare();

    int result = pLuaSelectItem->executeFile(luaFileName);
    LuaState::logError(result);

    ItemClass = pLuaSelectItem->getItemClass();
    ItemType = pLuaSelectItem->getItemType();
    OptionType = pLuaSelectItem->getOptionType();

    pLuaSelectItem->clear();

    if (ItemClass >= Item::ITEM_CLASS_MAX)
    //||  ItemType  >= ITEM_TYPE_MAX || ItemType  < 0
    //	|| OptionType == 0)
    {
        filelog("XMasEventError.txt", "[ ItemInfo Error ] : ItemClass = %d , ItemType = %d , OptionType = %d",
                ItemClass, ItemType, OptionType);

        GCNPCResponse quit;
        quit.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
        pPlayer->sendPacket(&quit);

        return;
    }

    
    GCDeleteInventoryItem gcDeleteInventoryItem;
    gcDeleteInventoryItem.setObjectID(pGiftBoxItem->getObjectID());
    pPlayer->sendPacket(&gcDeleteInventoryItem);

    
    pInventory->deleteItem(X, Y);
    
    if (pGiftBoxItem != NULL && pGiftBoxItem->isTraceItem()) {
        remainTraceLog(pGiftBoxItem, pCreature2->getName(), pCreature1->getName(), ITEM_LOG_DELETE, DETAIL_EVENTNPC);
    }
    pGiftBoxItem->destroy();
    SAFE_DELETE(pGiftBoxItem);


    
    list<OptionType_t> optionTypeList;
    if (OptionType != 0)
        optionTypeList.push_back(OptionType);

    pItem = g_pItemFactoryManager->createItem(ItemClass, ItemType, optionTypeList);
    Assert(pItem != NULL);


    
    pZone->getObjectRegistry().registerObject(pItem);
    pInventory->addItem(X, Y, pItem);
    pItem->create(pPC->getName(), STORAGE_INVENTORY, 0, X, Y);

    
    if (pItem != NULL && pItem->isTraceItem()) {
        remainTraceLog(pItem, pCreature1->getName(), pCreature2->getName(), ITEM_LOG_CREATE, DETAIL_EVENTNPC);
    }

    
    GCCreateItem gcCreateItem;
    makeGCCreateItem(&gcCreateItem, pItem, X, Y);
    pPlayer->sendPacket(&gcCreateItem);

    
    pFlagSet->turnOn(FLAGSET_TRADE_GIFT_BOX_2002_12);

    
    pFlagSet->save(pPC->getName());

    
    GCNPCResponse response;
    response.setCode(NPC_RESPONSE_TRADE_GIFT_BOX_OK);
    pPlayer->sendPacket(&response);

    GCNPCResponse quit;
    quit.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
    pPlayer->sendPacket(&quit);

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionTradeGiftBox::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "ActionTradeGiftBox("
        << ")";
    return msg.toString();

    __END_CATCH
}
