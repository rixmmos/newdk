//////////////////////////////////////////////////////////////////////////////
// Filename    : NPC.cpp

// Description :


//////////////////////////////////////////////////////////////////////////////

#include "NPC.h"

#include "CastleInfoManager.h"
#include "GCNPCAskDynamic.h"
#include "PlayerCreature.h"
#include "ShopRack.h"
#include "couple/PartnerWaitingManager.h"
#include "mission/QuestInfoManager.h"
#include "mission/RewardClassInfoManager.h"

//////////////////////////////////////////////////////////////////////////////
// class NPC member methods
//////////////////////////////////////////////////////////////////////////////

NPC::NPC()

{
    __BEGIN_TRY

    m_pInventory = NULL;

    
    getCurrentTime(m_NextTurn);
    m_NextTurn.tv_sec += rand() % 3;
    m_NextTurn.tv_usec += rand() % 1000000;
    if (m_NextTurn.tv_usec >= 1000000)
        m_NextTurn.tv_sec++;

    
    // m_pInventory = new Inventory(5,5);

    
    m_pRack = new ShopRack[SHOP_RACK_TYPE_MAX];
    Assert(m_pRack != NULL);

    
    m_MarketCondBuy = 25;
    m_MarketCondSell = 100;

    
    m_ShopType = SHOPTYPE_NORMAL;

    m_ClanType = 0;

    
    
    // by sigi. 2002.9.6
    m_Sight = 5;

    // m_pQuestBoard = NULL;

    m_pCoupleRegisterManager = NULL;
    m_pCoupleUnregisterManager = NULL;

    m_pQuestInfoManager = NULL;
    m_pRewardClassInfoManager = NULL;

    m_TaxingCastleZoneID = 0;

    __END_CATCH
}

NPC::NPC(const string& name)

{
    __BEGIN_TRY

    m_Name = name;
    m_pInventory = NULL;

    
    getCurrentTime(m_NextTurn);
    m_NextTurn.tv_sec += rand() % 3;
    m_NextTurn.tv_usec += rand() % 1000000;
    if (m_NextTurn.tv_usec >= 1000000)
        m_NextTurn.tv_sec++;

    
    // m_pInventory = new Inventory(5,5);

    
    m_pRack = new ShopRack[SHOP_RACK_TYPE_MAX];
    Assert(m_pRack != NULL);

    
    m_MarketCondBuy = 25;
    m_MarketCondSell = 100;

    m_pCoupleRegisterManager = NULL;
    m_pCoupleUnregisterManager = NULL;

    m_pQuestInfoManager = NULL;
    m_pRewardClassInfoManager = NULL;

    __END_CATCH
}

NPC::~NPC()

{
    __BEGIN_TRY

    SAFE_DELETE(m_pCoupleUnregisterManager);
    SAFE_DELETE(m_pCoupleRegisterManager);

    SAFE_DELETE(m_pQuestInfoManager);
    SAFE_DELETE(m_pRewardClassInfoManager);

    SAFE_DELETE(m_pInventory);
    SAFE_DELETE_ARRAY(m_pRack);

    __END_CATCH_NO_RETHROW
}

// registerObject()



void NPC::registerObject()

{
    __BEGIN_TRY

    Assert(getZone() != NULL);

    ObjectRegistry& OR = getZone()->getObjectRegistry();

    __ENTER_CRITICAL_SECTION(OR)

    OR.registerObject_NOLOCKED(this);

    __LEAVE_CRITICAL_SECTION(OR)

    __END_CATCH
}

// load()


bool NPC::load()

{
    __BEGIN_TRY

    
    m_TriggerManager.load(m_Name);

    
    if (m_TriggerManager.hasCondition(Condition::CONDITION_AT_FIRST)) {
        list<Trigger*>& triggers = m_TriggerManager.getTriggers();
        list<Trigger*>::iterator itr = triggers.begin();

        for (; itr != triggers.end(); itr++) {
            Trigger* pTrigger = *itr;
            if (pTrigger->hasCondition(Condition::CONDITION_AT_FIRST)) {
                pTrigger->activate(this);
                triggers.erase(itr);
                break;
            }
        }
    }

    return true;

    __END_CATCH
}

// init()

void NPC::init()

{
    __BEGIN_TRY

    load();
    // registerObject();

    __END_CATCH
}

// act()


void NPC::act(const Timeval& currentTime)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    
    
    if (currentTime < m_NextTurn)
        return;

    
    
    Timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = 750000 + rand() % 200000;
    m_NextTurn = m_NextTurn + delay;

    if (m_pCoupleRegisterManager != NULL)
        m_pCoupleRegisterManager->heartbeat();
    if (m_pCoupleUnregisterManager != NULL)
        m_pCoupleUnregisterManager->heartbeat();

    
    
    
    
    list<Trigger*>& triggers = m_TriggerManager.getTriggers();
    list<Trigger*>::iterator itr = triggers.begin();

    for (; itr != triggers.end(); itr++) {
        Trigger* pTrigger = *itr;
        if (pTrigger->isAllSatisfied(Trigger::ACTIVE_TRIGGER, this)) {
            pTrigger->activate(this);
            return;
        }
    }

    __END_DEBUG
    __END_CATCH
}

// The eleven accessors below index m_pRack[SHOP_RACK_TYPE_MAX] -- three
// elements -- and every one of them was guarded only by
// Assert(type < SHOP_RACK_TYPE_MAX). ShopRackType_t is a BYTE and `type`
// arrives raw off the wire: CGShopRequestBuyHandler takes it from
// pPacket->getShopType() and CGShopRequestListHandler from getRackType(),
// neither of which bounds it -- the `type == SHOP_RACK_MYSTERIOUS` tests in
// those handlers select a branch, they do not validate. insertShopItem and
// removeShopItem then write through the out-of-range ItemRack, so this is a
// heap write, not only a read.
//
// These throw rather than returning a sentinel because several callers
// dereference the result unchecked (CGShopRequestBuyHandler:106, :194, :466
// all do pItem->... with no NULL test), so a sentinel would trade an
// out-of-bounds read for a NULL dereference. Same reasoning as
// Inventory::getInventorySlot. Throwing also preserves today's behaviour
// exactly: Assert already throws in a Debug build, and both AssertionError and
// OutOfBoundException are caught by the same catch (Throwable&) in
// GamePlayer. GCShopVersion.h already guards its own copy of this index with
// a real `if`. A legitimate client sends 0..2 and never reaches any of this.
//
// getShopVersion()

ShopVersion_t NPC::getShopVersion(ShopRackType_t type) const {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    return m_pRack[type].getVersion();
}

// setShopVersion()

void NPC::setShopVersion(ShopRackType_t type, ShopVersion_t ver) {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    m_pRack[type].setVersion(ver);
}

// increaseShopVersion()

void NPC::increaseShopVersion(ShopRackType_t type) {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    m_pRack[type].increaseVersion();
}

// isExistShopItem()

bool NPC::isExistShopItem(ShopRackType_t type, BYTE index) const {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    return m_pRack[type].isExist(index);
}

// insertShopItem()

void NPC::insertShopItem(ShopRackType_t type, BYTE index, Item* pItem) {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    m_pRack[type].insert(index, pItem);
}

// removeShopItem()

void NPC::removeShopItem(ShopRackType_t type, BYTE index) {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    return m_pRack[type].remove(index);
}

// getShopItem()

Item* NPC::getShopItem(ShopRackType_t type, BYTE index) const {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    return m_pRack[type].get(index);
}

// clearShopItem()

void NPC::clearShopItem(void) {
    for (int i = 0; i < SHOP_RACK_TYPE_MAX; i++)
        m_pRack[i].clear();
}

// getFirstEmptySlot()

BYTE NPC::getFirstEmptySlot(ShopRackType_t type) const {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    return m_pRack[type].getFirstEmptySlot();
}

// getLastEmptySlot()

BYTE NPC::getLastEmptySlot(ShopRackType_t type) const {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    return m_pRack[type].getLastEmptySlot();
}

// isFull()

bool NPC::isFull(ShopRackType_t type) const {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    return m_pRack[type].isFull();
}

// isEmpty

bool NPC::isEmpty(ShopRackType_t type) const {
    if (type >= SHOP_RACK_TYPE_MAX)
        throw OutOfBoundException("NPC: shop rack type out of range");

    Assert(type < SHOP_RACK_TYPE_MAX);
    return m_pRack[type].isEmpty();
}

int NPC::getTaxRatio(PlayerCreature* pPC) const {
    return g_pCastleInfoManager->getItemTaxRatio(pPC, this);
}

// SimpleQuest by sigi. 2002.12.3
 
/*
void NPC::sendSimpleQuestListPacket(Creature* pCreature)

{
    __BEGIN_TRY

#ifdef __ACTIVE_QUEST__
    if (m_pQuestBoard!=NULL)
    {
        Assert(pCreature!=NULL);
        Assert(pCreature->isPC());

        GCNPCAskDynamic npcAskPacket;
        npcAskPacket.setObjectID( getObjectID() );
        m_pQuestBoard->makeNPCAskPacket(npcAskPacket);

        pCreature->getPlayer()->sendPacket( &npcAskPacket );
    }

#endif

    __END_CATCH
}
*/

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string NPC::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "NPC("
        << "ObjectID:" << (int)m_ObjectID << ",SpriteType:" << (int)m_SpriteType << ",Name:" << m_Name
        << ",MainColor:" << (int)m_MainColor << ",SubColor:" << (int)m_SubColor << ",X:" << (int)m_X
        << ",Y:" << (int)m_Y << ",Sight:" << (int)m_Sight << m_TriggerManager.toString() << ")";
    return msg.toString();

    __END_CATCH
}
