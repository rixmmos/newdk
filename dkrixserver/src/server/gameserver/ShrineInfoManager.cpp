#include "ShrineInfoManager.h"

#include <stdio.h>

#include "BloodBible.h"
#include "BloodBibleBonusManager.h"
#include "CastleInfoManager.h"
#include "ClientManager.h"
#include "CreatureUtil.h"
#include "DB.h"
#include "EffectHasRelic.h"
#include "EffectShrineGuard.h"
#include "EffectShrineHoly.h"
#include "EffectShrineShield.h"
#include "EventRefreshHolyLandPlayer.h"
#include "GCAddEffect.h"
#include "GCAddEffectToTile.h"
#include "GCBloodBibleStatus.h"
#include "GCDeleteInventoryItem.h"
#include "GCRemoveEffect.h"
#include "GCSystemMessage.h"
#include "GlobalItemPosition.h"
#include "GlobalItemPositionLoader.h"
#include "HolyLandManager.h"
#include "Item.h"
#include "ItemFactoryManager.h"
#include "MonsterCorpse.h"
#include "Player.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"
#include "RelicUtil.h"
#include "StringPool.h"
#include "StringStream.h"
#include "War.h"
#include "WarSystem.h"
#include "Zone.h"
#include "ZoneGroupManager.h"
#include "ZoneItemPosition.h"
#include "ZoneUtil.h"

bool AddBible[] = {
    true,  // SHRINE_ARMEGA,      // 0
    false, // SHRINE_MIHOLE,      // 1
    false, // SHRINE_KIRO,        // 2
    false, // SHRINE_INI,         // 3
    false, // SHRINE_GREGORI,     // 4
    false, // SHRINE_CONCILIA,    // 5
    false, // SHRINE_LEGIOS,      // 6
    false, // SHRINE_HILLEL,      // 7
    false, // SHRINE_JAVE,        // 8
    true,  // SHRINE_NEMA,        // 9
    false, // SHRINE_AROSA,       // 10
    false, // SHRINE_CHASPA       // 11
};

string ShrineInfo::toString() const

{
    StringStream msg;

    msg << "ShrineInfo("
        << "MonsterType:" << (int)m_MonsterType << ",ShrineType:" << (m_ShrineType == SHRINE_GUARD ? "GUARD" : "HOLY")
        << ",ZoneID:" << (int)m_ZoneID << ",X:" << (int)m_X << ",Y:" << (int)m_Y << ",Name:" << m_Name << ")";

    return msg.toString();
}

ShrineInfo& ShrineSet::getReturnGuardShrine()

{
    __BEGIN_TRY

    switch (m_OwnerRace) {
    case RACE_SLAYER:
        return m_SlayerGuardShrine;
        break;
    case RACE_VAMPIRE:
        return m_VampireGuardShrine;
        break;
    case RACE_OUSTERS:
        return m_OustersGuardShrine;
        break;
    }

    // cannot reach here
    Assert(false);
    return m_SlayerGuardShrine;

    __END_CATCH
}

ShrineSet::ShrineSet() {
    m_Mutex.setName("ShrineSet");
    m_pGCBBS = NULL;
}

ShrineSet::~ShrineSet() {
    SAFE_DELETE(m_pGCBBS);
}

Item* ShrineSet::createBloodBibleInGuardShrine()

{
    __BEGIN_TRY

    ShrineInfo* pShrineInfo = &getReturnGuardShrine();
    Zone* pZone = getZoneByZoneID(pShrineInfo->getZoneID());
    Assert(pZone != NULL);

    MonsterCorpse* pShrine = dynamic_cast<MonsterCorpse*>(pZone->getItem(pShrineInfo->getObjectID()));
    Assert(pShrine != NULL);

    list<OptionType_t> optionNULL;
    Item* pItem = g_pItemFactoryManager->createItem(Item::ITEM_CLASS_BLOOD_BIBLE, m_ItemType, optionNULL);
    Assert(pItem != NULL);

    char strZoneID[10];
    sprintf(strZoneID, "%d", (int)pZone->getZoneID());

    pZone->registerObject(pItem);
    pItem->create(strZoneID, STORAGE_CORPSE, pShrine->getObjectID(), 0, 0);

    pShrine->addTreasure(pItem);

    return pItem;

    __END_CATCH
}

void ShrineSet::setOwnerRace(Race_t race)

{
    __BEGIN_TRY

    g_pBloodBibleBonusManager->setBloodBibleBonusRace(m_ShrineID, race);

    m_OwnerRace = race;
    saveBloodBibleOwner();

    __END_CATCH
}

void ShrineSet::setBloodBibleStatus(GCBloodBibleStatus* pGCBBS)

{
    __BEGIN_TRY
    __ENTER_CRITICAL_SECTION(m_Mutex)

    SAFE_DELETE(m_pGCBBS);
    m_pGCBBS = pGCBBS;

    __LEAVE_CRITICAL_SECTION(m_Mutex);
    __END_CATCH
}

void ShrineSet::sendBloodBibleStatus(PlayerCreature* pPC)

{
    __BEGIN_TRY
    __ENTER_CRITICAL_SECTION(m_Mutex)

    if (m_pGCBBS != NULL)
        pPC->getPlayer()->sendPacket(m_pGCBBS);

    __LEAVE_CRITICAL_SECTION(m_Mutex)
    __END_CATCH
}

void ShrineSet::broadcastBloodBibleStatus()

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    //	if ( m_pGCBBS != NULL ) g_pHolyLandManager->broadcast( m_pGCBBS );
    if (m_pGCBBS != NULL)
        g_pZoneGroupManager->broadcast(m_pGCBBS);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

string ShrineSet::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "ShrineSet("
        << "ShrineID:" << (int)m_ShrineID << "," << m_SlayerGuardShrine.toString() << ","
        << m_VampireGuardShrine.toString() << "," << m_OustersGuardShrine.toString() << "," << m_HolyShrine.toString()
        << ",ItemType:" << (int)m_ItemType << ")";

    return msg.toString();

    __END_CATCH
}

ShrineInfoManager::~ShrineInfoManager() {
    clear();
}

void ShrineInfoManager::clear() {
    HashMapShrineSetItor itr = m_ShrineSets.begin();
    for (; itr != m_ShrineSets.end(); itr++) {
        SAFE_DELETE(itr->second);
    }
    m_ShrineSets.clear();
}

void ShrineInfoManager::init()

{
    __BEGIN_TRY

    load();

    addAllShrineToZone();

    __END_CATCH
}

void ShrineInfoManager::load()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectShrineInfoStmt(
            pConn,
            "SELECT ID, Name, ItemType, SlayerGuardZoneID, SlayerGuardX, SlayerGuardY, SlayerGuardMType, "
            "VampireGuardZoneID, VampireGuardX, VampireGuardY, VampireGuardMType, OustersGuardZoneID, OustersGuardX, "
            "OustersGuardY, OustersGuardMType, HolyZoneID, HolyX, HolyY, HolyMType, OwnerRace FROM ShrineInfo");
        pResult = selectShrineInfoStmt.execute();

        while (pResult->next()) {
            int i = 0;

            ShrineSet* pShrineSet = new ShrineSet();

            ShrineInfo& SlayerGuardShrine = pShrineSet->getSlayerGuardShrine();
            ShrineInfo& VampireGuardShrine = pShrineSet->getVampireGuardShrine();
            ShrineInfo& OustersGuardShrine = pShrineSet->getOustersGuardShrine();
            ShrineInfo& HolyShrine = pShrineSet->getHolyShrine();

            pShrineSet->setShrineID(pResult->getInt(++i));

            SlayerGuardShrine.setName(pResult->getString(++i));
            VampireGuardShrine.setName(SlayerGuardShrine.getName());
            OustersGuardShrine.setName(SlayerGuardShrine.getName());
            HolyShrine.setName(SlayerGuardShrine.getName());

            pShrineSet->setBloodBibleItemType(pResult->getInt(++i));
            SlayerGuardShrine.setZoneID(pResult->getInt(++i));
            SlayerGuardShrine.setX(pResult->getInt(++i));
            SlayerGuardShrine.setY(pResult->getInt(++i));
            SlayerGuardShrine.setMonsterType(pResult->getInt(++i));
            VampireGuardShrine.setZoneID(pResult->getInt(++i));
            VampireGuardShrine.setX(pResult->getInt(++i));
            VampireGuardShrine.setY(pResult->getInt(++i));
            VampireGuardShrine.setMonsterType(pResult->getInt(++i));
            OustersGuardShrine.setZoneID(pResult->getInt(++i));
            OustersGuardShrine.setX(pResult->getInt(++i));
            OustersGuardShrine.setY(pResult->getInt(++i));
            OustersGuardShrine.setMonsterType(pResult->getInt(++i));
            HolyShrine.setZoneID(pResult->getInt(++i));
            HolyShrine.setX(pResult->getInt(++i));
            HolyShrine.setY(pResult->getInt(++i));
            HolyShrine.setMonsterType(pResult->getInt(++i));

            pShrineSet->setOwnerRace((Race_t)pResult->getInt(++i));

            SlayerGuardShrine.setShrineType(ShrineInfo::SHRINE_GUARD);
            VampireGuardShrine.setShrineType(ShrineInfo::SHRINE_GUARD);
            OustersGuardShrine.setShrineType(ShrineInfo::SHRINE_GUARD);
            HolyShrine.setShrineType(ShrineInfo::SHRINE_HOLY);


            if (pShrineSet->getBloodBibleItemType() != pShrineSet->getShrineID()) {
                cout << "ShrineID  ItemType  . DB ." << endl;
                Assert(false);
            }

            addShrineSet(pShrineSet);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


void ShrineInfoManager::reloadOwner()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    bool bOwnerChanged = false;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement selectShrineOwnerStmt(pConn, "SELECT ID, OwnerRace FROM ShrineInfo");
        pResult = selectShrineOwnerStmt.execute();

        while (pResult->next()) {
            int i = 0;

            ShrineID_t shrineID = pResult->getInt(++i);
            Race_t OwnerRace = (Race_t)pResult->getInt(++i);

            ShrineSet* pShrineSet = getShrineSet(shrineID);

            if (pShrineSet->getOwnerRace() != OwnerRace) {
                pShrineSet->setOwnerRace(OwnerRace);
                returnBloodBible(shrineID);

                bOwnerChanged = true;
            }
        }
    }
    END_DB(pStmt)

    if (bOwnerChanged) {
        EventRefreshHolyLandPlayer* pEvent = new EventRefreshHolyLandPlayer(NULL);
        pEvent->setDeadline(0);

        g_pClientManager->addEvent_LOCKED(pEvent);
    }

    __END_CATCH
}


void ShrineInfoManager::addAllShrineToZone()

{
    __BEGIN_TRY

    HashMapShrineSetItor itr = m_ShrineSets.begin();
    for (; itr != m_ShrineSets.end(); itr++) {
        ShrineSet* pShrineSet = itr->second;

        if (pShrineSet == NULL)
            continue;

        addShrineToZone(pShrineSet->getSlayerGuardShrine(), pShrineSet->getBloodBibleItemType());
        addShrineToZone(pShrineSet->getVampireGuardShrine(), pShrineSet->getBloodBibleItemType());
        addShrineToZone(pShrineSet->getOustersGuardShrine(), pShrineSet->getBloodBibleItemType());

        Item* pItem = pShrineSet->createBloodBibleInGuardShrine();
        pShrineSet->setBloodBibleItemID(pItem->getItemID());

        addShrineToZone(pShrineSet->getHolyShrine(), pShrineSet->getBloodBibleItemType());
    }

    __END_CATCH
}

void ShrineInfoManager::addShrineToZone(ShrineInfo& shrineInfo, ItemType_t itemType)

{
    __BEGIN_TRY


    if (shrineInfo.getShrineType() == ShrineInfo::SHRINE_HOLY)
        return;


    Zone* pZone = getZoneByZoneID(shrineInfo.getZoneID());
    Assert(pZone != NULL);

    MonsterCorpse* pShrine = new MonsterCorpse(shrineInfo.getMonsterType(), shrineInfo.getName(), 2);
    Assert(pShrine != NULL);

    pShrine->setShrine(true);
    pShrine->setZone(pZone);

    pZone->getObjectRegistry().registerObject(pShrine);
    shrineInfo.setObjectID(pShrine->getObjectID());

    cout << "AddShrine[" << (int)shrineInfo.getZoneID() << "] "
         << (shrineInfo.getShrineType() == ShrineInfo::SHRINE_GUARD ? "Guard" : "Holy")
         << ", mtype=" << shrineInfo.getMonsterType() << ", oid=" << pShrine->getObjectID() << endl;

    if (shrineInfo.getShrineType() == ShrineInfo::SHRINE_GUARD) {
        pShrine->setFlag(Effect::EFFECT_CLASS_SHRINE_GUARD);

        EffectShrineGuard* pEffect = new EffectShrineGuard(pShrine);
        pEffect->setShrineID(itemType);
        pEffect->setTick(60 * 10);

        pShrine->getEffectManager().addEffect(pEffect);
    }
    /*
    else if ( shrineInfo.getShrineType() == ShrineInfo::SHRINE_HOLY )
    {
        pShrine->setFlag( Effect::EFFECT_CLASS_SHRINE_HOLY );

        EffectShrineHoly* pEffect = new EffectShrineHoly(pShrine);
        pEffect->setShrineID( itemType );
        pEffect->setTick( 60 * 10 );

        pShrine->getEffectManager().addEffect( pEffect );
    }
    */

    TPOINT tp = pZone->addItem(pShrine, shrineInfo.getX(), shrineInfo.getY(), true);
    Assert(tp.x != -1);

    if (shrineInfo.getShrineType() == ShrineInfo::SHRINE_GUARD) {
        pShrine->setFlag(Effect::EFFECT_CLASS_SHRINE_SHIELD);

        EffectShrineShield* pEffect = new EffectShrineShield(pShrine);
        pEffect->setShrineID(itemType);
        pEffect->setTick(60 * 10);

        pShrine->getEffectManager().addEffect(pEffect);
    }

    forbidDarkness(pZone, tp.x, tp.y, 2);


    shrineInfo.setX(tp.x);
    shrineInfo.setY(tp.y);

    __END_CATCH
}

void ShrineInfoManager::addShrineSet(ShrineSet* pShrineSet)

{
    __BEGIN_TRY

    if (pShrineSet == NULL)
        return;

    ShrineID_t shrineID = pShrineSet->getShrineID();

    HashMapShrineSetItor itr = m_ShrineSets.find(shrineID);

    if (itr != m_ShrineSets.end()) {
        filelog("ShrineInfoError.log", "ShrineInfoManager::addShrineSet DuplicatedException : %d", (int)shrineID);
        return;
    }

    m_ShrineSets[shrineID] = pShrineSet;

    __END_CATCH
}

void ShrineInfoManager::deleteShrineSet(ShrineID_t shrineID)

{
    __BEGIN_TRY

    HashMapShrineSetItor itr = m_ShrineSets.find(shrineID);

    if (itr != m_ShrineSets.end()) {
        SAFE_DELETE(itr->second);
        m_ShrineSets.erase(shrineID);
    }

    __END_CATCH
}

ShrineSet* ShrineInfoManager::getShrineSet(ShrineID_t shrineID) const

{
    __BEGIN_TRY

    HashMapShrineSetConstItor itr = m_ShrineSets.find(shrineID);

    if (itr != m_ShrineSets.end()) {
        return itr->second;
    }

    return NULL;

    __END_CATCH
}

bool ShrineInfoManager::isMatchGuardShrine(Item* pItem, MonsterCorpse* pMonsterCorpse, PlayerCreature* pPC) const

{
    __BEGIN_TRY

    if (pItem->getItemClass() != Item::ITEM_CLASS_BLOOD_BIBLE)
        return false;

    ItemType_t itemType = pItem->getItemType();
    ShrineID_t shrineID = itemType; // ShrineID = ItemType(of BloodBible)

    ShrineSet* pShrineSet = getShrineSet(shrineID);

    if (pShrineSet == NULL) {
        return false;
    }


    if (pPC->isSlayer()) {
        return pShrineSet->getSlayerGuardShrine().getMonsterType() == pMonsterCorpse->getMonsterType();
    } else if (pPC->isVampire()) {
        return pShrineSet->getVampireGuardShrine().getMonsterType() == pMonsterCorpse->getMonsterType();
    } else if (pPC->isOusters()) {
        return pShrineSet->getOustersGuardShrine().getMonsterType() == pMonsterCorpse->getMonsterType();
    }

    return false;

    __END_CATCH
}


bool ShrineInfoManager::isMatchHolyShrine(Item* pItem, MonsterCorpse* pMonsterCorpse) const

{
    __BEGIN_TRY

    if (pItem->getItemClass() != Item::ITEM_CLASS_BLOOD_BIBLE)
        return false;

    ItemType_t itemType = pItem->getItemType();
    ShrineID_t shrineID = itemType; // ShrineID = ItemType(of BloodBible)

    ShrineSet* pShrineSet = getShrineSet(shrineID);

    if (pShrineSet == NULL) {
        return false;
    }


    return pShrineSet->getHolyShrine().getMonsterType() == pMonsterCorpse->getMonsterType();

    __END_CATCH
}

bool ShrineInfoManager::isDefenderOfGuardShrine(PlayerCreature* pPC, MonsterCorpse* pShrine) const

{
    __BEGIN_TRY

    Zone* pZone = pShrine->getZone();
    Assert(pZone != NULL);


    if (!pZone->isCastle()) {
        return false;
    }

    ZoneID_t castleZoneID = pZone->getZoneID();

    CastleInfo* pCastleInfo = g_pCastleInfoManager->getCastleInfo(castleZoneID);
    if (pCastleInfo == NULL)
        return false;


    if (pPC->getRace() == pCastleInfo->getRace())
        return true;

    return false;

    __END_CATCH
}


bool ShrineInfoManager::canPickupBloodBible(Race_t race, BloodBible* pBloodBible) const

{
    __BEGIN_TRY


    return true;


    __END_CATCH
}

bool ShrineInfoManager::getMatchGuardShrinePosition(Item* pItem, ZoneItemPosition& zip) const

{
    __BEGIN_TRY

    if (pItem->getItemClass() != Item::ITEM_CLASS_BLOOD_BIBLE)
        return false;

    ItemType_t itemType = pItem->getItemType();
    ShrineID_t shrineID = itemType; // ShrineID = ItemType(of BloodBible)

    ShrineSet* pShrineSet = getShrineSet(shrineID);

    if (pShrineSet == NULL) {
        return false;
    }

    ShrineInfo& GuardShrine = pShrineSet->getReturnGuardShrine();

    zip.setZoneID(GuardShrine.getZoneID());
    zip.setZoneX(GuardShrine.getX());
    zip.setZoneY(GuardShrine.getY());

    return true;

    __END_CATCH
}


// 2003. 2. 5. by Sequoia
bool ShrineInfoManager::returnBloodBible(ShrineID_t shrineID, bool bLock) const

{
    __BEGIN_TRY


    ShrineSet* pShrineSet = getShrineSet(shrineID);

    if (pShrineSet == NULL)
        return false;

    Item::ItemClass ItemClass = Item::ITEM_CLASS_BLOOD_BIBLE;
    ItemID_t ItemID = pShrineSet->getBloodBibleItemID();

    if (ItemID == 0)
        return false;

    GlobalItemPosition* pItemPosition = GlobalItemPositionLoader::getInstance()->load(ItemClass, ItemID);

    if (pItemPosition == NULL)
        return false;

    Item* pItem = pItemPosition->popItem(bLock);

    if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_BLOOD_BIBLE) {
        Zone* pZone = pItemPosition->getZone();
        Assert(pZone != NULL);

        BloodBible* pBloodBible = dynamic_cast<BloodBible*>(pItem);
        Assert(pBloodBible != NULL);

        return returnBloodBible(pZone, pBloodBible);
    }

    return false;

    __END_CATCH
}


bool ShrineInfoManager::returnAllBloodBible() const

{
    __BEGIN_TRY

    bool bReturned = false;

    HashMapShrineSetConstItor itr = m_ShrineSets.begin();


    for (; itr != m_ShrineSets.end(); itr++) {
        ShrineSet* pShrineSet = itr->second;

        bReturned = returnBloodBible(pShrineSet->getShrineID()) || bReturned;
    }

    return bReturned;

    __END_CATCH
}


bool ShrineInfoManager::returnBloodBible(Zone* pZone, BloodBible* pBloodBible) const

{
    __BEGIN_TRY

    Assert(pZone != NULL);
    Assert(pBloodBible != NULL);


    ShrineID_t shrineID = pBloodBible->getItemType();
    ShrineSet* pShrineSet = getShrineSet(shrineID);

    if (pShrineSet == NULL)
        return false;

    ShrineInfo& GuardShrine = pShrineSet->getReturnGuardShrine();

    Zone* pTargetZone = getZoneByZoneID(GuardShrine.getZoneID());
    Assert(pTargetZone != NULL);

    ObjectID_t CorpseObjectID = GuardShrine.getObjectID();

    pZone->transportItemToCorpse(pBloodBible, pTargetZone, CorpseObjectID);


    char msg[300];

    const char* race;
    if (pShrineSet->getOwnerRace() == RACE_SLAYER) {
        race = g_pStringPool->c_str(STRID_SLAYER);
    } else if (pShrineSet->getOwnerRace() == RACE_VAMPIRE) {
        race = g_pStringPool->c_str(STRID_VAMPIRE);
    } else if (pShrineSet->getOwnerRace() == RACE_OUSTERS) {
        race = g_pStringPool->c_str(STRID_OUSTERS);
    }

    sprintf(msg, g_pStringPool->c_str(STRID_RETURN_TO_GUARD_SHRINE_BLOOD_BIBLE), GuardShrine.getName().c_str(), race,
            //					(pShrineSet->getOwnerRace()==RACE_SLAYER? g_pStringPool->c_str( STRID_SLAYER ) :
            // g_pStringPool->c_str( STRID_VAMPIRE ) ),
            GuardShrine.getName().c_str());
    GCSystemMessage msgPkt;
    msgPkt.setMessage(msg);

    g_pHolyLandManager->broadcast(&msgPkt);

    return true;

    __END_CATCH
}

bool ShrineInfoManager::putBloodBible(PlayerCreature* pPC, Item* pItem, MonsterCorpse* pCorpse) const

{
    __BEGIN_TRY

    Assert(pPC != NULL);
    Assert(pItem != NULL);
    Assert(pCorpse != NULL);

    ShrineID_t shrineID = pItem->getItemType();

    filelog("WarLog.txt", "%s   [%u]  [%s] .", pPC->getName().c_str(), (uint)shrineID, pCorpse->getName().c_str());


    //	sendBloodBibleEffect( pCorpse, Effect::EFFECT_CLASS_SHRINE_HOLY_WARP );


    Assert(pItem->getObjectID() == pPC->getExtraInventorySlotItem()->getObjectID());
    pPC->deleteItemFromExtraInventorySlot();

    GCDeleteInventoryItem gcDeleteInventoryItem;
    gcDeleteInventoryItem.setObjectID(pItem->getObjectID());

    pPC->getPlayer()->sendPacket(&gcDeleteInventoryItem);

    deleteRelicEffect(pPC, pItem);

    pCorpse->addTreasure(pItem);

    ShrineSet* pShrineSet = getShrineSet(shrineID);
    if (pShrineSet == NULL)
        return false;

    // ZoneID_t castleZoneID = pShrineSet->getReturnGuardShrine().getZoneID();


    if (isMatchHolyShrine(pItem, pCorpse) // && g_pWarSystem->isModifyCastleOwner( castleZoneID, pPC ))

        || isDefenderOfGuardShrine(pPC, pCorpse) && isMatchGuardShrine(pItem, pCorpse, pPC)) {
        pShrineSet->setOwnerRace(pPC->getRace());
        //        g_pWarSystem->endWar(pPC, castleZoneID);


        //        returnBloodBible( shrineID, false );

        // return true;
    }


    returnBloodBible(shrineID, false);

    return false;

    __END_CATCH
}


bool ShrineInfoManager::removeAllShrineShield()

{
    __BEGIN_TRY

    HashMapShrineSetConstItor itr = m_ShrineSets.begin();


    for (; itr != m_ShrineSets.end(); itr++) {
        ShrineSet* pShrineSet = itr->second;

        removeShrineShield(&(pShrineSet->getSlayerGuardShrine()));
        removeShrineShield(&(pShrineSet->getVampireGuardShrine()));
        removeShrineShield(&(pShrineSet->getOustersGuardShrine()));
    }

    return true;

    __END_CATCH
}

bool ShrineInfoManager::removeShrineShield(ShrineInfo* pShrineInfo)

{
    __BEGIN_TRY

    ZoneID_t guardZoneID = pShrineInfo->getZoneID();

    Zone* pZone = getZoneByZoneID(guardZoneID);
    Assert(pZone != NULL);

    Item* pItem = pZone->getItem(pShrineInfo->getObjectID());

    if (pItem != NULL &&
        pItem->getItemClass() == Item::ITEM_CLASS_CORPSE
        //		&& pItem->isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE)
        && pItem->isFlag(Effect::EFFECT_CLASS_SHRINE_SHIELD)) {
        pItem->removeFlag(Effect::EFFECT_CLASS_SHRINE_SHIELD);

        Corpse* pCorpse = dynamic_cast<Corpse*>(pItem);

        EffectManager& EM = pItem->getEffectManager();
        EM.deleteEffect(Effect::EFFECT_CLASS_SHRINE_SHIELD);

        GCRemoveEffect gcRemoveEffect;
        gcRemoveEffect.setObjectID(pItem->getObjectID());
        gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_SHRINE_SHIELD);
        pZone->broadcastPacket(pCorpse->getX(), pCorpse->getY(), &gcRemoveEffect);


        if (pItem->isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE)) {
            Effect* pEffect = EM.findEffect(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE);
            Assert(pEffect != NULL);

            EffectHasRelic* pHasRelicEffect = dynamic_cast<EffectHasRelic*>(pEffect);
            Assert(pHasRelicEffect != NULL);

            pHasRelicEffect->affect();
        }

        return true;
    }

    return false;

    __END_CATCH
}

/*bool ShrineInfoManager::addShrineShield(Zone *pZone)

{
    __BEGIN_TRY

    cout << "ShrineInfoManager::addShrineShield_LOCKED( Zone *pZone ) is deprecated" << endl;
    Assert( false );

    return false;
    Assert(pZone!=NULL);

    bool bAdded = false;

    __ENTER_CRITICAL_SECTION( (*pZone) )

    bAdded = addShrineShield_LOCKED( pZone );

    __LEAVE_CRITICAL_SECTION( (*pZone) )

    return bAdded;

    __END_CATCH
}
*/

void ShrineInfoManager::addAllShrineShield()

{
    __BEGIN_TRY

    HashMapShrineSetConstItor itr = m_ShrineSets.begin();

    for (; itr != m_ShrineSets.end(); itr++) {
        ShrineSet* pShrineSet = itr->second;

        addShrineShield(pShrineSet->getSlayerGuardShrine());
        addShrineShield(pShrineSet->getVampireGuardShrine());
        addShrineShield(pShrineSet->getOustersGuardShrine());
    }

    __END_CATCH
}

bool ShrineInfoManager::addShrineShield(ShrineInfo& shrineInfo)

{
    __BEGIN_TRY

    Zone* pZone = getZoneByZoneID(shrineInfo.getZoneID());
    Assert(pZone != NULL);

    Item* pItem = pZone->getItem(shrineInfo.getObjectID());

    if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_CORPSE &&
        !pItem->isFlag(Effect::EFFECT_CLASS_SHRINE_SHIELD)) {
        pItem->setFlag(Effect::EFFECT_CLASS_SHRINE_SHIELD);

        Corpse* pCorpse = dynamic_cast<Corpse*>(pItem);
        Assert(pCorpse != NULL);

        EffectManager& EM = pItem->getEffectManager();
        EffectShrineShield* pEffect = new EffectShrineShield(pCorpse);
        pEffect->setShrineID(pCorpse->getItemType());
        pEffect->setTick(60 * 10);
        EM.addEffect(pEffect);

        GCAddEffect gcAddEffect;
        gcAddEffect.setObjectID(pItem->getObjectID());
        gcAddEffect.setEffectID(Effect::EFFECT_CLASS_SHRINE_SHIELD);
        gcAddEffect.setDuration(65000);

        pZone->broadcastPacket(pCorpse->getX(), pCorpse->getY(), &gcAddEffect);

        return true;
    }

    return false;

    __END_CATCH
}

bool ShrineInfoManager::saveBloodBibleOwner()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        HashMapShrineSetConstItor itr = m_ShrineSets.begin();

        for (; itr != m_ShrineSets.end(); itr++) {
            ShrineSet* pShrineSet = itr->second;

            pShrineSet->saveBloodBibleOwner();
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    return true;

    __END_CATCH
}

bool ShrineSet::saveBloodBibleOwner()

{
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement updateOwnerRaceStmt(pConn, "UPDATE ShrineInfo SET OwnerRace=? WHERE ID=?");
        updateOwnerRaceStmt.bindInt(1, (int)getOwnerRace());
        updateOwnerRaceStmt.bindInt(2, (int)getShrineID());
        updateOwnerRaceStmt.execute();
    }
    END_DB(pStmt)

    return true;

    __END_CATCH
}

void ShrineInfoManager::registerBloodBibleStatus(ItemType_t m_Part, GCBloodBibleStatus* pGCBBS)

{
    __BEGIN_TRY

    ShrineSet* pShrineSet = getShrineSet(m_Part);
    Assert(pShrineSet != NULL);

    pShrineSet->setBloodBibleStatus(pGCBBS);

    __END_CATCH
}

void ShrineInfoManager::sendBloodBibleStatus(PlayerCreature* pPC)

{
    __BEGIN_TRY

    Assert(pPC != NULL);

    HashMapShrineSetConstItor itr = m_ShrineSets.begin();

    for (; itr != m_ShrineSets.end(); itr++) {
        ShrineSet* pShrineSet = itr->second;

        pShrineSet->sendBloodBibleStatus(pPC);
    }

    __END_CATCH
}

void ShrineInfoManager::broadcastBloodBibleStatus()

{
    __BEGIN_TRY

    HashMapShrineSetConstItor itr = m_ShrineSets.begin();

    for (; itr != m_ShrineSets.end(); itr++) {
        ShrineSet* pShrineSet = itr->second;

        pShrineSet->broadcastBloodBibleStatus();
    }

    __END_CATCH
}

string ShrineInfoManager::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "ShrineInfoManager("
        << "Size:" << size() << ",(\n";

    HashMapShrineSetConstItor itr = m_ShrineSets.begin();

    for (; itr != m_ShrineSets.end(); itr++) {
        msg << "\t" << itr->second->toString() << "\n";
    }

    msg << "))";

    return msg.toString();

    __END_CATCH
}

ShrineInfoManager* g_pShrineInfoManager = NULL;
