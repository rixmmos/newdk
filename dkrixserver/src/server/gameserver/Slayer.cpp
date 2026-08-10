//////////////////////////////////////////////////////////////////////////////
// Filename    : Slayer.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Slayer.h"

#include "DB.h"
#include "EffectManager.h"
#include "LogClient.h"
#include "Player.h"
#include "PreparedStatement.h"
// #include <algo.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>

#include "AbilityBalance.h"
#include "CastleInfoManager.h"
#include "CastleSkillInfo.h"
#include "CreatureUtil.h"
#include "EffectLoaderManager.h"
#include "FlagSet.h"
#include "GamePlayer.h"
#include "ItemInfoManager.h"
#include "OptionInfo.h"
#include "PKZoneInfoManager.h"
#include "ParkingCenter.h"
#include "ResurrectLocationManager.h"
#include "SkillDomainInfoManager.h"
#include "SkillHandlerManager.h"
#include "SkillInfo.h"
#include "Stash.h"
#include "TradeManager.h"
#include "WarSystem.h"
// #include "AttrBalanceInfo.h"
#include "ItemUtil.h"
#include "PacketUtil.h"
#include "Party.h"
#include "Shape.h"
#include "VariableManager.h"
// #include "RankEXPInfo.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "EffectGrandMasterSlayer.h"
#include "GCAddEffect.h"
#include "GCChangeShape.h"
#include "GCModifyInformation.h"
#include "GCOtherModifyInfo.h"
#include "GCPetStashList.h"
#include "GCRealWearingInfo.h"
#include "GCRemoveEffect.h"
#include "GCSkillInfo.h"
#include "GCStatusCurrentHP.h"
#include "GCTakeOff.h"
#include "GCTradeFinish.h"
#include "LoginServerManager.h"
#include "Properties.h"
#include "Rpackets/RCSay.h"
#include "item/AR.h"
#include "item/Belt.h"
#include "item/Motorcycle.h"
#include "item/SG.h"
#include "item/SMG.h"
#include "item/SR.h"
#include "item/Skull.h"
#include "skill/CastleSkillSlot.h"
#include "skill/EffectBless.h"
#include "skill/EffectChargingPower.h"
#include "skill/EffectDancingSword.h"
#include "skill/EffectDoom.h"
#include "skill/EffectGhostBlade.h"
#include "skill/EffectParalyze.h"
#include "skill/EffectPotentialExplosion.h"
#include "skill/EffectProtectionFromAcid.h"
#include "skill/EffectProtectionFromCurse.h"
#include "skill/EffectProtectionFromPoison.h"
// #include "RankEXPInfo.h"
#include "AdvancementClassExpTable.h"
#include "DynamicZone.h"
#include "ExpFwd.h"
#include "GuildUnion.h"
#include "MonsterInfo.h"
#include "RaceWarLimiter.h"
#include "RankExpTable.h"
#include "SlayerAttrExpTable.h"
#include "Store.h"
#include "SystemAvailabilitiesManager.h"
#include "TimeLimitItemManager.h"
#include "skill/SkillUtil.h"
#include "types/ServerType.h"

const Color_t UNIQUE_COLOR = 0xFFFF;
const Color_t UNIQUE_OPTION = 0xFFFF;

const Color_t QUEST_COLOR = 0xFFFE;
const Color_t QUEST_OPTION = 0xFFFE;

const Attr_t MAX_SLAYER_ATTR = 290;
const Attr_t MAX_SLAYER_SUM = 435;
const Attr_t MAX_SLAYER_ATTR_OLD = 200;
const Attr_t MAX_SLAYER_SUM_OLD = 300;

Slayer::Slayer()

    : PlayerCreature(0, NULL) {
    __BEGIN_TRY

    m_CClass = CREATURE_CLASS_SLAYER;

    m_Mutex.setName("Slayer");


    for (int i = 0; i < SKILL_DOUBLE_IMPACT; i++) {
        SkillSlot* pSkillSlot = new SkillSlot;
        // pSkillSlot = new SkillSlot;	// 2002.1.16  by sigi
        pSkillSlot->setName(m_Name);

        pSkillSlot->setSkillType(i);
        pSkillSlot->setInterval(5);
        pSkillSlot->setExpLevel(1);
        pSkillSlot->setExp(1);
        pSkillSlot->setRunTime();
        addSkill(pSkillSlot);
    }

    for (int i = 0; i < WEAR_MAX; i++)
        m_pWearItem[i] = NULL;


    m_pMotorcycle = NULL;

    for (int i = 0; i < MAX_PHONE_SLOT; i++) {
        m_PhoneSlot[i] = 0;
    }


    //	for (int i = 0; i < 4; i++)
    //	{
    //		m_HotKey[i] = 0;
    //	}


    getCurrentTime(m_MPRegenTime);


    m_DomainExpSaveCount = 0;
    m_AttrExpSaveCount = 0;
    m_SkillExpSaveCount = 0;
    m_FameSaveCount = 0;
    m_AlignmentSaveCount = 0;
    //	m_RankExpSaveCount   = 0;
    m_Gold = 0;

    //	m_pRank = NULL;

    __END_CATCH
}

Slayer::~Slayer()

{
    __BEGIN_TRY

    try {
        if (m_pMotorcycle != NULL) {
            // getOffMotorcycle();


            if (g_pParkingCenter->hasMotorcycleBox(m_pMotorcycle->getItemID())) {
                g_pParkingCenter->deleteMotorcycleBox(m_pMotorcycle->getItemID());
            }

            m_pMotorcycle = NULL;
        }


        DWORD flag;
        Color_t color[PCSlayerInfo::SLAYER_COLOR_MAX];
        getShapeInfo(flag, color);

        char pField[128];
        sprintf(pField, "Shape=%ld, HelmetColor=%d, JacketColor=%d, PantsColor=%d, WeaponColor=%d, ShieldColor=%d",
                flag, color[PCSlayerInfo::SLAYER_COLOR_HELMET], color[PCSlayerInfo::SLAYER_COLOR_JACKET],
                color[PCSlayerInfo::SLAYER_COLOR_PANTS], color[PCSlayerInfo::SLAYER_COLOR_WEAPON],
                color[PCSlayerInfo::SLAYER_COLOR_SHIELD]);

        // cout << "SAVE = " << pField << endl;

        tinysave(pField);


        saveGears();
        saveExps();
        saveSkills();


        destroyGears();


        TradeManager* pTradeManager = m_pZone->getTradeManager();
        TradeInfo* pInfo = pTradeManager->getTradeInfo(getName());
        if (pInfo != NULL) {
            pTradeManager->cancelTrade(this);
        }


        deleteAllPartyInfo(this);


        unordered_map<SkillType_t, SkillSlot*>::iterator itr = m_SkillSlot.begin();
        for (; itr != m_SkillSlot.end(); itr++) {
            SkillSlot* pSkillSlot = itr->second;
            SAFE_DELETE(pSkillSlot);
        }

        m_SkillSlot.clear();

        //		SAFE_DELETE( m_pRank );

    } catch (Throwable& t) {
        filelog("slayerDestructor.txt", "%s", t.toString().c_str());
        throw;
    } catch (exception& e) {
        filelog("slayerDestructor.txt", "Unknown std::exception");
        throw;
    } catch (...) {
        filelog("slayerDestructor.txt", "Unknown ... exception");
        throw;
    }

    m_bDeriveDestructed = true;

    __END_CATCH_NO_RETHROW
}


void Slayer::registerObject()

{
    __BEGIN_TRY

    Assert(getZone() != NULL);


    ObjectRegistry& OR = getZone()->getObjectRegistry();

    __ENTER_CRITICAL_SECTION(OR)


    if (m_pTimeLimitItemManager != NULL)
        m_pTimeLimitItemManager->clear();


    OR.registerObject_NOLOCKED(this);


    registerInventory(OR);


    registerGoodsInventory(OR);


    for (int i = 0; i < WEAR_MAX; i++) {
        Item* pItem = m_pWearItem[i];

        if (pItem != NULL) {
            bool bCheck = true;


            if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem))
                bCheck = false;

            if (bCheck)
                registerItem(pItem, OR);
        }
    }


    Item* pSlotItem = m_pExtraInventorySlot->getItem();
    if (pSlotItem != NULL)
        registerItem(pSlotItem, OR);


    if (m_pMotorcycle != NULL)
        OR.registerObject_NOLOCKED(m_pMotorcycle);

    m_Garbage.registerObject(OR);

    for (int i = 0; i < MAX_PET_STASH; ++i) {
        Item* pItem = getPetStashItem(i);
        if (pItem != NULL)
            registerItem(pItem, OR);
    }

    __LEAVE_CRITICAL_SECTION(OR)

    m_SlayerInfo.setObjectID(m_ObjectID);
    m_pStore->updateStoreInfo();

    __END_CATCH
}


void Slayer::registerInitObject()

{
    __BEGIN_TRY

    Assert(getZone() != NULL);


    ObjectRegistry& OR = getZone()->getObjectRegistry();

    __ENTER_CRITICAL_SECTION(OR)


    if (m_pTimeLimitItemManager != NULL)
        m_pTimeLimitItemManager->clear();


    OR.registerObject_NOLOCKED(this);


    registerInitInventory(OR);


    registerGoodsInventory(OR);


    for (int i = 0; i < WEAR_MAX; i++) {
        Item* pItem = m_pWearItem[i];

        if (pItem != NULL) {
            pItem->setTraceItem(bTraceLog(pItem));

            bool bCheck = true;


            if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem))
                bCheck = false;

            if (bCheck)
                registerItem(pItem, OR);
        }
    }


    Item* pSlotItem = m_pExtraInventorySlot->getItem();
    if (pSlotItem != NULL) {
        pSlotItem->setTraceItem(bTraceLog(pSlotItem));
        registerItem(pSlotItem, OR);
    }


    if (m_pMotorcycle != NULL)
        OR.registerObject_NOLOCKED(m_pMotorcycle);

    m_Garbage.registerObject(OR);

    __LEAVE_CRITICAL_SECTION(OR)

    m_SlayerInfo.setObjectID(m_ObjectID);

    __END_CATCH
}


void Slayer::checkItemTimeLimit() {
    __BEGIN_TRY


    {
        list<Item*> ItemList;
        int height = m_pInventory->getHeight();
        int width = m_pInventory->getWidth();

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                Item* pItem = m_pInventory->getItem(i, j);
                if (pItem != NULL) {
                    list<Item*>::iterator itr = find(ItemList.begin(), ItemList.end(), pItem);

                    if (itr == ItemList.end()) {
                        i += pItem->getVolumeWidth() - 1;

                        if (wasteIfTimeLimitExpired(pItem)) {
                            m_pInventory->deleteItem(pItem->getObjectID());
                            SAFE_DELETE(pItem);
                        } else {
                            ItemList.push_back(pItem);
                        }
                    }
                }
            }
        }
    }


    {
        for (int i = 0; i < WEAR_MAX; i++) {
            Item* pItem = m_pWearItem[i];

            if (pItem != NULL) {
                bool bCheck = true;

                if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem))
                    bCheck = false;

                if (bCheck) {
                    if (wasteIfTimeLimitExpired(pItem)) {
                        deleteWearItem((WearPart)i);
                        if (i == WEAR_LEFTHAND && isTwohandWeapon(pItem))
                            deleteWearItem(WEAR_RIGHTHAND);
                        SAFE_DELETE(pItem);
                    }
                }
            }
        }
    }


    {
        Item* pSlotItem = m_pExtraInventorySlot->getItem();
        if (pSlotItem != NULL && wasteIfTimeLimitExpired(pSlotItem)) {
            deleteItemFromExtraInventorySlot();
            SAFE_DELETE(pSlotItem);
        }
    }


    /*	{
            if (m_pMotorcycle != NULL && wasteIfTimeLimitExpired( m_pMotorcycle ) )
            {
            }
        }*/

    __END_CATCH
}

void Slayer::updateEventItemTime(DWORD time) {
    __BEGIN_TRY


    {
        list<Item*> ItemList;
        int height = m_pInventory->getHeight();
        int width = m_pInventory->getWidth();

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                Item* pItem = m_pInventory->getItem(i, j);
                if (pItem != NULL) {
                    list<Item*>::iterator itr = find(ItemList.begin(), ItemList.end(), pItem);

                    if (itr == ItemList.end()) {
                        i += pItem->getVolumeWidth() - 1;

                        updateItemTimeLimit(pItem, time);


                        ItemList.push_back(pItem);
                    }
                }
            }
        }
    }


    {
        for (int i = 0; i < WEAR_MAX; i++) {
            Item* pItem = m_pWearItem[i];

            if (pItem != NULL) {
                bool bCheck = true;

                if (i == WEAR_RIGHTHAND && isTwohandWeapon(pItem))
                    bCheck = false;

                if (bCheck) {
                    updateItemTimeLimit(pItem, time);
                }
            }
        }
    }


    {
        Item* pSlotItem = m_pExtraInventorySlot->getItem();
        if (pSlotItem != NULL) {
            updateItemTimeLimit(pSlotItem, time);
        }
    }

    __END_CATCH
}

void Slayer::loadItem(bool checkTimeLimit)

{
    __BEGIN_TRY

    PlayerCreature::loadItem();


    SAFE_DELETE(m_pInventory);
    m_pInventory = new Inventory(10, 6);
    m_pInventory->setOwner(getName());


    g_pItemLoaderManager->load(this);


    PlayerCreature::loadGoods();


    registerInitObject();


    if (m_pFlagSet->isOn(FLAGSET_RECEIVE_NEWBIE_ITEM_AUTO)) {
        addNewbieItemToInventory(this);
        addNewbieGoldToInventory(this);
        addNewbieItemToGear(this);

        m_pFlagSet->turnOff(FLAGSET_RECEIVE_NEWBIE_ITEM_AUTO);
        m_pFlagSet->save(getName());
    }

    if (checkTimeLimit) {
        checkItemTimeLimit();
    }


    initAllStat();

    // cout << "Slayer::loadItem() : STR[CURRENT]" << (int)m_STR[ATTR_CURRENT] << endl;
    // cout << "Slayer::loadItem() : STR[MAX]" << (int)m_STR[ATTR_MAX] << endl;
    // cout << "Slayer::loadItem() : STR[BASIC]" << (int)m_STR[ATTR_BASIC] << endl;
    // cout << "Slayer::loadItem() : DEX[CURRENT]" << (int)m_DEX[ATTR_CURRENT] << endl;
    // cout << "Slayer::loadItem() : DEX[MAX]" << (int)m_DEX[ATTR_MAX] << endl;
    // cout << "Slayer::loadItem() : DEX[BASIC]" << (int)m_DEX[ATTR_BASIC] << endl;
    // cout << "Slayer::loadItem() : INT[CURRENT]" << (int)m_INT[ATTR_CURRENT] << endl;
    // cout << "Slayer::loadItem() : INT[MAX]" << (int)m_INT[ATTR_MAX] << endl;
    // cout << "Slayer::loadItem() : INT[BASIC]" << (int)m_INT[ATTR_BASIC] << endl;

    __END_CATCH
}

bool Slayer::load()

{
    __BEGIN_TRY

    if (!PlayerCreature::load())
        return false;

    for (int i = 0; i < SKILL_DOMAIN_VAMPIRE; i++) {
        m_SkillDomainLevels[i] = 0;
        //		m_SkillDomainExps[i]   = 0;
    }

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    int reward = 0;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement loadSlayerStmt(
            pConn,
            "SELECT Name, AdvancementClass, AdvancementGoalExp, Competence, CompetenceShape, Sex,MasterEffectColor, "
            "HairStyle, HairColor, SkinColor, Phone, "
            "STR, STRGoalExp,"
            "DEX, DEXGoalExp,"
            "INTE, INTGoalExp,"
            "AdvancedSTR, AdvancedDEX, AdvancedINT, Bonus,"
            "`Rank`, RankGoalExp,"
            "CurrentHP, HP, CurrentMP, MP,"
            "Fame, Gold, GuildID,"
            "BladeLevel, BladeGoalExp,"
            "SwordLevel, SwordGoalExp,"
            "GunLevel, GunGoalExp,"
            "EnchantLevel, EnchantGoalExp,"
            "HealLevel, HealGoalExp,"
            "ETCLevel, ETCGoalExp,"
            "ZoneID, XCoord, YCoord, Sight,"
            "GunBonusExp, RifleBonusExp,"
            "Alignment,"
            "StashGold, StashNum, ResurrectZone, Reward,"
            "SMSCharge "
            "FROM Slayer WHERE Name = ? AND Active = 'ACTIVE'");
        loadSlayerStmt.bindString(1, m_Name);
        pResult = loadSlayerStmt.execute();

        if (pResult->getRowCount() == 0) {
            return false;
        }

        pResult->next();

        uint i = 0;

        setName(pResult->getString(++i));

        Level_t advLevel = pResult->getInt(++i);
        Exp_t advGoalExp = pResult->getInt(++i);

        m_pAdvancementClass =
            new AdvancementClass(advLevel, advGoalExp, AdvancementClassExpTable::s_AdvancementClassExpTable);
        if (getAdvancementClassLevel() > 0)
            m_bAdvanced = true;

        int competence = pResult->getInt(++i);
        if (competence >= 4)
            competence = 3;

        setCompetence(competence);
        setCompetenceShape(pResult->getInt(++i));
        setSex(pResult->getString(++i));
        setMasterEffectColor(pResult->getInt(++i));
        setHairStyle(pResult->getString(++i));
        setHairColor(pResult->getInt(++i));
        setSkinColor(pResult->getInt(++i));
        setPhoneNumber(atoi(pResult->getString(++i)));

        m_STR[ATTR_BASIC] = pResult->getInt(++i);
        m_STR[ATTR_MAX] = m_STR[ATTR_BASIC];
        m_STR[ATTR_CURRENT] = m_STR[ATTR_BASIC];
        //		m_STRExp            = pResult->getInt(++i);
        Exp_t STRGoalExp = pResult->getInt(++i);

        m_pAttrs[ATTR_KIND_STR] =
            new Attr(m_STR[ATTR_BASIC], STRGoalExp, SlayerAttrExpTable::s_SlayerAttrExpTable[ATTR_KIND_STR]);

        m_DEX[ATTR_BASIC] = pResult->getInt(++i);
        m_DEX[ATTR_MAX] = m_DEX[ATTR_BASIC];
        m_DEX[ATTR_CURRENT] = m_DEX[ATTR_BASIC];
        //		m_DEXExp            = pResult->getInt(++i);
        Exp_t DEXGoalExp = pResult->getInt(++i);

        m_pAttrs[ATTR_KIND_DEX] =
            new Attr(m_DEX[ATTR_BASIC], DEXGoalExp, SlayerAttrExpTable::s_SlayerAttrExpTable[ATTR_KIND_DEX]);

        m_INT[ATTR_BASIC] = pResult->getInt(++i);
        m_INT[ATTR_MAX] = m_INT[ATTR_BASIC];
        m_INT[ATTR_CURRENT] = m_INT[ATTR_BASIC];
        //		m_INTExp            = pResult->getInt(++i);
        Exp_t INTGoalExp = pResult->getInt(++i);

        m_pAttrs[ATTR_KIND_INT] =
            new Attr(m_INT[ATTR_BASIC], INTGoalExp, SlayerAttrExpTable::s_SlayerAttrExpTable[ATTR_KIND_INT]);

        m_AdvancedSTR = pResult->getInt(++i);
        m_AdvancedDEX = pResult->getInt(++i);
        m_AdvancedINT = pResult->getInt(++i);
        m_AdvancedAttrBonus = pResult->getInt(++i);

        Rank_t CurRank = pResult->getInt(++i);
        //		RankExp_t RankExp            = pResult->getInt(++i);
        RankExp_t RankGoalExp = pResult->getInt(++i);

        m_pRank = new Rank(CurRank, RankGoalExp, RankExpTable::s_RankExpTables[RANK_TYPE_SLAYER]);
        //		cout << getRankGoalExp() << endl;

        // cout << "Slayer::load() : STR[CURRENT]" << (int)m_STR[ATTR_CURRENT] << endl;
        // cout << "Slayer::load() : STR[MAX]" << (int)m_STR[ATTR_MAX] << endl;
        // cout << "Slayer::load() : STR[BASIC]" << (int)m_STR[ATTR_BASIC] << endl;
        // cout << "Slayer::load() : DEX[CURRENT]" << (int)m_DEX[ATTR_CURRENT] << endl;
        // cout << "Slayer::load() : DEX[MAX]" << (int)m_DEX[ATTR_MAX] << endl;
        // cout << "Slayer::load() : DEX[BASIC]" << (int)m_DEX[ATTR_BASIC] << endl;
        // cout << "Slayer::load() : INT[CURRENT]" << (int)m_INT[ATTR_CURRENT] << endl;
        // cout << "Slayer::load() : INT[MAX]" << (int)m_INT[ATTR_MAX] << endl;
        // cout << "Slayer::load() : INT[BASIC]" << (int)m_INT[ATTR_BASIC] << endl;

        m_HP[ATTR_CURRENT] = pResult->getInt(++i);
        m_HP[ATTR_MAX] = pResult->getInt(++i);
        m_HP[ATTR_BASIC] = 0;

        m_MP[ATTR_CURRENT] = pResult->getInt(++i);
        m_MP[ATTR_MAX] = pResult->getInt(++i);
        m_MP[ATTR_BASIC] = 0;

        setFame(pResult->getInt(++i));
        setGold(pResult->getInt(++i));
        setGuildID(pResult->getInt(++i));
        // setInMagics(pResult->getString(++i));

        setSkillDomainLevel(SKILL_DOMAIN_BLADE, pResult->getInt(++i));
        //		setSkillDomainExp  (SKILL_DOMAIN_BLADE,   pResult->getInt(++i));
        setGoalExp(SKILL_DOMAIN_BLADE, pResult->getInt(++i));
        setSkillDomainLevel(SKILL_DOMAIN_SWORD, pResult->getInt(++i));
        //		setSkillDomainExp  (SKILL_DOMAIN_SWORD,   pResult->getInt(++i));
        setGoalExp(SKILL_DOMAIN_SWORD, pResult->getInt(++i));
        setSkillDomainLevel(SKILL_DOMAIN_GUN, pResult->getInt(++i));
        //		setSkillDomainExp  (SKILL_DOMAIN_GUN,     pResult->getInt(++i));
        setGoalExp(SKILL_DOMAIN_GUN, pResult->getInt(++i));
        // setSkillDomainLevel(SKILL_DOMAIN_RIFLE   , pResult->getInt(++i));
        // setSkillDomainExp(SKILL_DOMAIN_RIFLE   , pResult->getInt(++i));
        // setGoalExp(SKILL_DOMAIN_RIFLE	  , pResult->getInt(++i));
        setSkillDomainLevel(SKILL_DOMAIN_ENCHANT, pResult->getInt(++i));
        //		setSkillDomainExp  (SKILL_DOMAIN_ENCHANT, pResult->getInt(++i));
        setGoalExp(SKILL_DOMAIN_ENCHANT, pResult->getInt(++i));
        setSkillDomainLevel(SKILL_DOMAIN_HEAL, pResult->getInt(++i));
        //		setSkillDomainExp  (SKILL_DOMAIN_HEAL,    pResult->getInt(++i));
        setGoalExp(SKILL_DOMAIN_HEAL, pResult->getInt(++i));
        setSkillDomainLevel(SKILL_DOMAIN_ETC, pResult->getInt(++i));
        //		setSkillDomainExp  (SKILL_DOMAIN_ETC,     pResult->getInt(++i));
        setGoalExp(SKILL_DOMAIN_ETC, pResult->getInt(++i));

        //		setZoneID(pResult->getInt(++i));
        ZoneID_t zoneID = pResult->getInt(++i);
        setX(pResult->getInt(++i));
        setY(pResult->getInt(++i));

        setSight(pResult->getInt(++i));
        setSight(13);
        setGunBonusExp(pResult->getInt(++i));
        setRifleBonusExp(pResult->getInt(++i));
        setAlignment(pResult->getInt(++i));

        //		for (int j = 0; j < 4; j++) setHotKey(j, pResult->getInt(++i));

        setStashGold(pResult->getInt(++i));
        setStashNum(pResult->getBYTE(++i));
        setResurrectZoneID(pResult->getInt(++i));

        reward = pResult->getInt(++i);
        setSMSCharge(pResult->getInt(++i));


        //		m_HP[ATTR_MAX]      = m_STR[ATTR_CURRENT]*2;

        try {
            setZoneID(zoneID);
        } catch (Error& e) {
            ZONE_COORD ResurrectCoord;
            g_pResurrectLocationManager->getSlayerPosition(12, ResurrectCoord);
            setZoneID(ResurrectCoord.id);
            setX(ResurrectCoord.x);
            setY(ResurrectCoord.y);
        }
    }
    END_DB(pStmt)


    ObjectRegistry& OR = getZone()->getObjectRegistry();
    OR.registerObject(this);

    m_SlayerInfo.setObjectID(m_ObjectID);
    m_SlayerInfo.setName(m_Name);
    m_SlayerInfo.setX(m_X);
    m_SlayerInfo.setY(m_Y);
    m_SlayerInfo.setDir(m_Dir);
    m_SlayerInfo.setSex(m_Sex);
    m_SlayerInfo.setHairStyle(m_HairStyle);


    m_SlayerInfo.setCompetence(m_CompetenceShape);


    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectSkillSaveStmt(
            pConn,
            "SELECT SkillType, SkillLevel, SkillExp, Delay, CastingTime, NextTime FROM SkillSave WHERE OwnerID = ?");
        selectSkillSaveStmt.bindString(1, m_Name);
        pResult = selectSkillSaveStmt.execute();

        while (pResult->next()) {
            int i = 0;
            SkillSlot* pSkillSlot = new SkillSlot();

            pSkillSlot->setName(m_Name);
            pSkillSlot->setSkillType(pResult->getInt(++i));
            pSkillSlot->setExpLevel(pResult->getInt(++i));
            pSkillSlot->setExp(pResult->getInt(++i));

            pSkillSlot->setInterval(pResult->getInt(++i));

            pSkillSlot->setCastingTime(pResult->getInt(++i));
            // pSkillSlot->setRunTime (pResult->getInt(++i));
            pSkillSlot->setRunTime();


            SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(pSkillSlot->getSkillType());
            Assert(pSkillInfo != NULL);


            if (pSkillInfo->getLevel() > m_SkillDomainLevels[pSkillInfo->getDomainType()] &&
                pSkillInfo->getDomainType() != SKILL_DOMAIN_ETC) {
                pSkillSlot->setDisable();
            }

            addSkill(pSkillSlot);
        }
    }
    END_DB(pStmt)


    g_pEffectLoaderManager->load(this);


    loadRankBonus();


    // by sigi. 2002.11.8
    if (getHighestSkillDomainLevel() >= 100 && SystemAvailabilitiesManager::getInstance()->isAvailable(
                                                   SystemAvailabilitiesManager::SYSTEM_GRAND_MASTER_EFFECT)) {
        if (!isFlag(Effect::EFFECT_CLASS_GRAND_MASTER_SLAYER)) {
            EffectGrandMasterSlayer* pEffect = new EffectGrandMasterSlayer(this);
            pEffect->setDeadline(999999);
            getEffectManager()->addEffect(pEffect);
            setFlag(Effect::EFFECT_CLASS_GRAND_MASTER_SLAYER);
        }
    }


    m_pFlagSet->load(getName());


    m_SlayerInfo.setHelmetType(HELMET_NONE);
    m_SlayerInfo.setJacketType(JACKET_BASIC);
    m_SlayerInfo.setPantsType(PANTS_BASIC);
    m_SlayerInfo.setWeaponType(WEAPON_NONE);
    m_SlayerInfo.setShieldType(SHIELD_NONE);
    m_SlayerInfo.setMotorcycleType(MOTORCYCLE_NONE);
    m_SlayerInfo.setMasterEffectColor(m_HairColor);
    m_SlayerInfo.setHairColor(m_HairColor);
    m_SlayerInfo.setSkinColor(m_SkinColor);
    m_SlayerInfo.setShoulderType(0);

    m_SlayerInfo.setAdvancementLevel(getAdvancementClassLevel());


    if (getRank() == 0) {
        saveInitialRank();
    }


    initAllStat();


    if (RaceWarLimiter::isInPCList(this)) {
        setFlag(Effect::EFFECT_CLASS_RACE_WAR_JOIN_TICKET);
    }

    if (m_pZone->isHolyLand() && g_pWarSystem->hasActiveRaceWar() &&
        !isFlag(Effect::EFFECT_CLASS_RACE_WAR_JOIN_TICKET)) {
        ZONE_COORD ResurrectCoord;
        g_pResurrectLocationManager->getPosition(this, ResurrectCoord);
        setZoneID(ResurrectCoord.id);
        setX(ResurrectCoord.x);
        setY(ResurrectCoord.y);
    }

    return true;

    __END_CATCH
}

void Slayer::save() const

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement saveSlayerStmt(pConn,
                                         "UPDATE Slayer SET CurrentHP=?, HP=?, CurrentMP=?, MP=?, ZoneID=?, XCoord=?, "
                                         "YCoord=? WHERE Name=?");
        saveSlayerStmt.bindInt(1, m_HP[ATTR_CURRENT]);
        saveSlayerStmt.bindInt(2, m_HP[ATTR_MAX]);
        saveSlayerStmt.bindInt(3, m_MP[ATTR_CURRENT]);
        saveSlayerStmt.bindInt(4, m_MP[ATTR_MAX]);
        saveSlayerStmt.bindInt(5, getZoneID());
        saveSlayerStmt.bindInt(6, (int)m_X);
        saveSlayerStmt.bindInt(7, (int)m_Y);
        saveSlayerStmt.bindString(8, m_Name);
        saveSlayerStmt.execute();

        // Assert(saveSlayerStmt.getAffectedRowCount() == 1);
    }
    END_DB(pStmt)


    m_pEffectManager->save(m_Name);


    if (m_pMotorcycle != NULL) {
        // m_pMotorcycle->save("", STORAGE_ZONE, m_pZone->getZoneID(), m_X, m_Y);
        //  by sigi. 2002.5.15
        char pField[128];

        sprintf(pField, "OwnerID='', Storage=%d, StorageID=%u, X=%d, Y=%d", STORAGE_ZONE, m_pZone->getZoneID(), m_X,
                m_Y);
        m_pMotorcycle->tinysave(pField);
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

// tinysave
void Slayer::tinysave(const string& field) const {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment (see callers, e.g.
        // saveSilverDamage), not a single bindable value; PreparedStatement cannot
        // parameterise an entire dynamic assignment list. Left spliced, matching
        // the Guild::tinysave precedent (batch 7). Only Name is bound.
        PreparedStatement tinysaveSlayerStmt(pConn, "UPDATE Slayer SET " + field + " WHERE NAME=?");
        tinysaveSlayerStmt.bindString(1, m_Name);
        tinysaveSlayerStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

PhoneNumber_t Slayer::getPhoneSlotNumber(SlotID_t SlotID)

{
    __BEGIN_TRY

    Assert(SlotID <= MAX_PHONE_SLOT);

    return m_PhoneSlot[SlotID];

    __END_CATCH
}

void Slayer::setPhoneSlotNumber(SlotID_t SlotID, PhoneNumber_t PhoneNumber)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    m_PhoneSlot[SlotID] = PhoneNumber;

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

bool Slayer::isSlotByPhoneNumber(PhoneNumber_t PhoneNumber)

{
    __BEGIN_TRY

    bool isFind = false;

    for (int i = 0; i < MAX_PHONE_SLOT; i++) {
        if (m_PhoneSlot[i] == PhoneNumber)
            isFind = true;
    }

    return isFind;

    __END_CATCH
}

SlotID_t Slayer::getSlotWithPhoneNumber(PhoneNumber_t PhoneNumber)

{
    __BEGIN_TRY

    for (int i = 0; i < MAX_PHONE_SLOT; i++) {
        if (m_PhoneSlot[i] == PhoneNumber)
            return i;
    }

    return MAX_PHONE_SLOT;

    __END_CATCH
}

SlotID_t Slayer::findEmptyPhoneSlot()

{
    __BEGIN_TRY

    for (int i = 0; i < MAX_PHONE_SLOT; i++) {
        if (m_PhoneSlot[i] == 0) {
            return i;
        }
    }

    return MAX_PHONE_SLOT;

    __END_CATCH
}

bool Slayer::isEmptyPhoneSlot()

{
    __BEGIN_TRY

    bool Success = false;

    for (int i = 0; i < MAX_PHONE_SLOT; i++) {
        if (m_PhoneSlot[i] == 0) {
            Success = true;
        }
    }

    return Success;

    __END_CATCH
}


SkillSlot* Slayer::getSkill(SkillType_t SkillType) const

{
    __BEGIN_TRY

    unordered_map<SkillType_t, SkillSlot*>::const_iterator itr = m_SkillSlot.find(SkillType);
    if (itr != m_SkillSlot.end()) {
        return itr->second;
    }

    return NULL;

    __END_CATCH
}


void Slayer::addSkill(SkillSlot* pSkillSlot)

{
    __BEGIN_TRY

    SkillType_t SkillType = pSkillSlot->getSkillType();
    switch (SkillType) {
    case SKILL_UN_BURROW:
    case SKILL_UN_TRANSFORM:
    case SKILL_UN_INVISIBILITY:
    case SKILL_THROW_HOLY_WATER:
    case SKILL_EAT_CORPSE:
    case SKILL_HOWL:
        filelog("SlayerError.log", "SkillType[%d], %s", SkillType, toString().c_str());
        Assert(false);
        break;
    default:
        break;
    }

    unordered_map<SkillType_t, SkillSlot*>::iterator itr = m_SkillSlot.find(pSkillSlot->getSkillType());

    if (itr == m_SkillSlot.end()) {
        m_SkillSlot[pSkillSlot->getSkillType()] = pSkillSlot;
    }
    // 2002.1.16 by sigi

    else {
        if (pSkillSlot != itr->second)
            SAFE_DELETE(pSkillSlot);
    }

    __END_CATCH
}


void Slayer::addSkill(SkillType_t SkillType)

{
    __BEGIN_TRY

    switch (SkillType) {
    case SKILL_UN_BURROW:
    case SKILL_UN_TRANSFORM:
    case SKILL_UN_INVISIBILITY:
    case SKILL_THROW_HOLY_WATER:
    case SKILL_EAT_CORPSE:
    case SKILL_HOWL:
        filelog("SlayerError.log", "2 SkillType[%d], %s", SkillType, toString().c_str());
        Assert(false);
        break;
    default:
        break;
    }

    unordered_map<SkillType_t, SkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);

    if (itr == m_SkillSlot.end()) {
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        Turn_t Delay = pSkillInfo->getMaxDelay();

        SkillSlot* pSkillSlot = new SkillSlot();
        pSkillSlot->setName(m_Name);
        pSkillSlot->setSkillType(SkillType);
        pSkillSlot->setInterval(Delay);
        pSkillSlot->setExpLevel(0);
        pSkillSlot->setExp(1);
        pSkillSlot->setRunTime();
        pSkillSlot->create(m_Name);

        m_SkillSlot[SkillType] = pSkillSlot;
    }

    __END_CATCH
}


void Slayer::removeCastleSkill(SkillType_t SkillType)

{
    __BEGIN_TRY


    if (g_pCastleSkillInfoManager->getZoneID(SkillType) == 0)
        return;

    unordered_map<SkillType_t, SkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);

    if (itr != m_SkillSlot.end()) {
        CastleSkillSlot* pCastleSkillSlot = dynamic_cast<CastleSkillSlot*>(itr->second);

        SAFE_DELETE(pCastleSkillSlot);

        m_SkillSlot.erase(itr);
    }

    __END_CATCH
}


void Slayer::removeAllCastleSkill()

{
    __BEGIN_TRY

    unordered_map<SkillType_t, SkillSlot*>::iterator itr = m_SkillSlot.begin();

    while (itr != m_SkillSlot.end()) {
        if (itr->second != NULL) {
            SkillSlot* pSkillSlot = itr->second;
            if (g_pCastleSkillInfoManager->getZoneID(pSkillSlot->getSkillType()) == 0) {
                ++itr;
                continue;
            }


            SAFE_DELETE(pSkillSlot);
            unordered_map<SkillType_t, SkillSlot*>::iterator prevItr = itr;

            ++itr;
            m_SkillSlot.erase(prevItr);
        } else {
            Assert(false);
        }
    }

    __END_CATCH
}

// Slayer::wearItem()


void Slayer::wearItem(WearPart Part, Item* pItem)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    Assert(pItem != NULL);

    Item::ItemClass IClass = pItem->getItemClass();
    Item* pLeft = NULL;
    Item* pRight = NULL;
    Item* pPrevItem = NULL;
    OptionInfo* pOptionInfo = NULL;


    if (pItem->getFirstOptionType() != 0)
        pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getFirstOptionType());


    if (isTwohandWeapon(pItem)) {
        if (isWear(WEAR_RIGHTHAND) && isWear(WEAR_LEFTHAND)) {
            pLeft = getWearItem(WEAR_RIGHTHAND);
            pRight = getWearItem(WEAR_LEFTHAND);


            if (pLeft == pRight) {
                m_pWearItem[WEAR_RIGHTHAND] = pItem;
                m_pWearItem[WEAR_LEFTHAND] = pItem;

                // by sigi. 2002.5.15
                char pField[128];
                // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
                sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
                pItem->tinysave(pField);


                addItemToExtraInventorySlot(pLeft);
                // pLeft->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
                sprintf(pField, "Storage=%d, StorageID=0", STORAGE_EXTRASLOT);
                pLeft->tinysave(pField);
            }

            else {
                cerr << "    ,     ." << endl;
                return;
            }
        }

        else {
            char pField[128];


            if (isWear(WEAR_RIGHTHAND)) {
                pRight = getWearItem(WEAR_RIGHTHAND);

                m_pWearItem[WEAR_RIGHTHAND] = pItem;
                m_pWearItem[WEAR_LEFTHAND] = pItem;

                // by sigi. 2002.5.15
                // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
                sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
                pItem->tinysave(pField);


                addItemToExtraInventorySlot(pRight);
                // pRight->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
                sprintf(pField, "Storage=%d, StorageID=0", STORAGE_EXTRASLOT);
                pRight->tinysave(pField);
            }

            else if (isWear(WEAR_LEFTHAND)) {
                pLeft = getWearItem(WEAR_LEFTHAND);

                m_pWearItem[WEAR_RIGHTHAND] = pItem;
                m_pWearItem[WEAR_LEFTHAND] = pItem;

                // by sigi. 2002.5.15
                // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
                sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
                pItem->tinysave(pField);


                addItemToExtraInventorySlot(pLeft);
                // pLeft->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
                sprintf(pField, "Storage=%d, StorageID=0", STORAGE_EXTRASLOT);
                pLeft->tinysave(pField);
            }

            else {
                m_pWearItem[WEAR_RIGHTHAND] = pItem;
                m_pWearItem[WEAR_LEFTHAND] = pItem;

                // by sigi. 2002.5.15
                // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
                sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
                pItem->tinysave(pField);
            }
        }
    } else {
        char pField[128];

        if (isWear(Part)) {
            pPrevItem = getWearItem(Part);

            m_pWearItem[Part] = pItem;

            // by sigi. 2002.5.15
            // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
            sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
            pItem->tinysave(pField);


            addItemToExtraInventorySlot(pPrevItem);

            // pPrevItem->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
            sprintf(pField, "Storage=%d, StorageID=0", STORAGE_EXTRASLOT);
            pPrevItem->tinysave(pField);
        } else {
            m_pWearItem[Part] = pItem;
            // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
            sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
            pItem->tinysave(pField);
        }
    }

    ItemType_t IType = pItem->getItemType();


    Color_t color = getItemShapeColor(pItem, pOptionInfo);

    switch (IClass) {
    case Item::ITEM_CLASS_MACE:
        // m_SlayerInfo.setWeaponType(WEAPON_MACE);
        m_SlayerInfo.setWeaponType(WEAPON_MACE);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_CROSS:
        m_SlayerInfo.setWeaponType(WEAPON_CROSS);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_BLADE:
        m_SlayerInfo.setWeaponType(WEAPON_BLADE);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_AR:
        m_SlayerInfo.setWeaponType(WEAPON_AR);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_SR:
        m_SlayerInfo.setWeaponType(WEAPON_SR);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_SMG:
        m_SlayerInfo.setWeaponType(WEAPON_SMG);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_SG:
        m_SlayerInfo.setWeaponType(WEAPON_SG);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_HELM:
        m_SlayerInfo.setHelmetType(getHelmetType(IType));
        m_SlayerInfo.setHelmetColor(color);
        break;
    case Item::ITEM_CLASS_SHIELD:
        m_SlayerInfo.setShieldType(getShieldType(IType));
        m_SlayerInfo.setShieldColor(color);
        break;
    case Item::ITEM_CLASS_SWORD:
        m_SlayerInfo.setWeaponType(WEAPON_SWORD);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_COAT:
        m_SlayerInfo.setJacketType(getJacketType(IType));
        // cout << "Jacket: ItemType=" << (int)IType << ", JacketType=" <<
        // JacketType2String[m_SlayerInfo.getJacketType()] << endl;
        m_SlayerInfo.setJacketColor(color);
        break;
    case Item::ITEM_CLASS_TROUSER:
        m_SlayerInfo.setPantsType(getPantsType(IType));
        // cout << "Pants: ItemType=" << (int)IType << ", PantsType=" << PantsType2String[m_SlayerInfo.getPantsType()]
        // << endl;
        m_SlayerInfo.setPantsColor(color);
        break;
    case Item::ITEM_CLASS_SHOULDER_ARMOR:
        m_SlayerInfo.setShoulderType(getShoulderType(IType));
        // cout << "Pants: ItemType=" << (int)IType << ", PantsType=" << PantsType2String[m_SlayerInfo.getPantsType()]
        // << endl;
        m_SlayerInfo.setShoulderColor(color);
        break;
    default:
        break;
    }

    __END_DEBUG
    __END_CATCH
}

// Slayer::wearItem()

void Slayer::wearItem(WearPart Part)

{
    __BEGIN_TRY
    __BEGIN_DEBUG


    Item* pItem = getExtraInventorySlotItem();
    Assert(pItem != NULL);

    Item::ItemClass IClass = pItem->getItemClass();
    OptionInfo* pOptionInfo = NULL;
    Item* pLeft = NULL;
    Item* pRight = NULL;
    Item* pPrevItem = NULL;
    GCTakeOff _GCTakeOff;


    if (pItem->getFirstOptionType() != 0)
        pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getFirstOptionType());


    if (IClass == Item::ITEM_CLASS_SHIELD)
        Part = WEAR_LEFTHAND;
    if (IClass == Item::ITEM_CLASS_SWORD)
        Part = WEAR_RIGHTHAND;


    SLAYER_RECORD prev;
    getSlayerRecord(prev);


    if (isTwohandWeapon(pItem)) {
        if (isWear(WEAR_RIGHTHAND) && isWear(WEAR_LEFTHAND)) {
            pLeft = getWearItem(WEAR_RIGHTHAND);
            pRight = getWearItem(WEAR_LEFTHAND);


            if (pLeft == pRight) {
                char pField[128];

                takeOffItem(WEAR_LEFTHAND, false, false);


                m_pWearItem[WEAR_RIGHTHAND] = pItem;
                m_pWearItem[WEAR_LEFTHAND] = pItem;
                // by sigi. 2002.5.15
                // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
                sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
                pItem->tinysave(pField);


                deleteItemFromExtraInventorySlot();

                addItemToExtraInventorySlot(pLeft);
                // pLeft->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
                sprintf(pField, "Storage=%d, StorageID=0", STORAGE_EXTRASLOT);
                pLeft->tinysave(pField);

            }

            else {
                return;
            }
        }

        else {
            // by sigi. 2002.5.15
            char pField[128];


            if (isWear(WEAR_RIGHTHAND)) {
                pRight = getWearItem(WEAR_RIGHTHAND);

                takeOffItem(WEAR_RIGHTHAND, false, false);


                m_pWearItem[WEAR_RIGHTHAND] = pItem;
                m_pWearItem[WEAR_LEFTHAND] = pItem;
                // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);

                // by sigi. 2002.5.15
                sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
                pItem->tinysave(pField);


                deleteItemFromExtraInventorySlot();

                addItemToExtraInventorySlot(pRight);
                // pRight->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
                sprintf(pField, "Storage=%d, StorageID=0", STORAGE_EXTRASLOT);
                pRight->tinysave(pField);

            }

            else if (isWear(WEAR_LEFTHAND)) {
                pLeft = getWearItem(WEAR_LEFTHAND);

                takeOffItem(WEAR_LEFTHAND, false, false);


                m_pWearItem[WEAR_RIGHTHAND] = pItem;
                m_pWearItem[WEAR_LEFTHAND] = pItem;

                // by sigi. 2002.5.15
                // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
                sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
                pItem->tinysave(pField);


                deleteItemFromExtraInventorySlot();

                addItemToExtraInventorySlot(pLeft);
                // pLeft->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
                sprintf(pField, "Storage=%d, StorageID=0", STORAGE_EXTRASLOT);
                pLeft->tinysave(pField);
            }

            else {
                m_pWearItem[WEAR_RIGHTHAND] = pItem;
                m_pWearItem[WEAR_LEFTHAND] = pItem;

                pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);

                deleteItemFromExtraInventorySlot();
            }
        }
    } else {
        char pField[128];

        if (isWear(Part)) {
            pPrevItem = getWearItem(Part);

            takeOffItem(Part, false, false);


            m_pWearItem[Part] = pItem;
            // by sigi. 2002.5.15
            // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
            sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
            pItem->tinysave(pField);


            deleteItemFromExtraInventorySlot();

            addItemToExtraInventorySlot(pPrevItem);
            // pPrevItem->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
            sprintf(pField, "Storage=%d, StorageID=0", STORAGE_EXTRASLOT);
            pPrevItem->tinysave(pField);
        } else {
            m_pWearItem[Part] = pItem;

            // by sigi. 2002.5.15
            // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
            sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
            pItem->tinysave(pField);

            deleteItemFromExtraInventorySlot();
        }
    }


    // by sigi. 2002.10.31
    m_pRealWearingCheck[Part] = true;

    initAllStat();
    sendRealWearingInfo();
    sendModifyInfo(prev);

    // bool bisWeapon = false;
    bool bisChange = false;

    // ItemType_t IType = pItem->getItemType();

    Color_t color = getItemShapeColor(pItem, pOptionInfo);


    bisChange = changeShape(pItem, color);


    if (m_pRealWearingCheck[Part])
    // if (bisChange)
    {
        GCChangeShape _GCChangeShape;
        _GCChangeShape.setObjectID(getObjectID());
        _GCChangeShape.setItemClass(IClass);
        _GCChangeShape.setItemType(pItem->getItemType());
        _GCChangeShape.setOptionType(pItem->getFirstOptionType());
        _GCChangeShape.setAttackSpeed(m_AttackSpeed[ATTR_CURRENT]);

        if (color == QUEST_COLOR)
            _GCChangeShape.setFlag(SHAPE_FLAG_QUEST);

        Zone* pZone = m_pZone;
        pZone->broadcastPacket(m_X, m_Y, &_GCChangeShape, this);

        // cout << _GCChangeShape.toString().c_str()  << endl;
    }

    if (m_pZone != NULL) {
        GCOtherModifyInfo gcOtherModifyInfo;
        makeGCOtherModifyInfo(&gcOtherModifyInfo, this, &prev);

        if (gcOtherModifyInfo.getShortCount() != 0 || gcOtherModifyInfo.getLongCount() != 0) {
            m_pZone->broadcastPacket(m_X, m_Y, &gcOtherModifyInfo, this);
        }
    }

    __END_DEBUG
    __END_CATCH
}

// Slayer::takeOffItem()


void Slayer::takeOffItem(WearPart Part, bool bAddOnMouse, bool bSendModifyInfo)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    SLAYER_RECORD prev;

    // Part can arrive from a client packet (see CGAddGearToMouseHandler), and
    // this function both reads m_pWearItem[Part] and later writes NULL to it --
    // an out-of-range value is an arbitrary-offset read *and* write inside a
    // live Slayer. Reject it outright; a real check, not Assert().
    if (Part < 0 || Part >= WEAR_MAX)
        return;

    Item* pItem = m_pWearItem[Part];
    Assert(pItem != NULL);
    Item::ItemClass IClass = pItem->getItemClass();

    if (Part == WEAR_LEFTHAND || Part == WEAR_RIGHTHAND) {
        if (m_pWearItem[WEAR_RIGHTHAND] && m_pWearItem[WEAR_LEFTHAND]) {
            if (m_pWearItem[WEAR_RIGHTHAND] == m_pWearItem[WEAR_LEFTHAND]) {
                m_pWearItem[WEAR_RIGHTHAND] = NULL;
                m_pWearItem[WEAR_LEFTHAND] = NULL;
            }
        }
    }


    if (isTwohandWeapon(pItem)) {
        m_pWearItem[WEAR_RIGHTHAND] = NULL;
        m_pWearItem[WEAR_LEFTHAND] = NULL;
    } else
        m_pWearItem[Part] = NULL;


    if (bSendModifyInfo) {
        getSlayerRecord(prev);
        initAllStat();
        sendRealWearingInfo();
        sendModifyInfo(prev);
    } else {
        initAllStat();
    }


    if (bAddOnMouse) {
        addItemToExtraInventorySlot(pItem);


        // pItem->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
        char pField[128];

        if (pItem->isSilverWeapon()) {
            if (pItem->isGun()) {
                //				Gun* pGun = dynamic_cast<Gun*>(pItem);
                sprintf(pField, "Storage=%d, StorageID=0, Durability=%d, BulletCount=%d, Silver=%d", STORAGE_EXTRASLOT,
                        pItem->getDurability(), pItem->getBulletCount(), pItem->getSilver());
            } else {
                sprintf(pField, "Storage=%d, StorageID=0, Durability=%d, Silver=%d", STORAGE_EXTRASLOT,
                        pItem->getDurability(), pItem->getSilver());
            }
        } else {
            sprintf(pField, "Storage=%d, StorageID=0, Durability=%d", STORAGE_EXTRASLOT, pItem->getDurability());
        }

        pItem->tinysave(pField);
    }

    GCTakeOff _GCTakeOff;

    bool bisWeapon = false;
    switch (IClass) {
    case Item::ITEM_CLASS_MACE:
    case Item::ITEM_CLASS_CROSS:
    case Item::ITEM_CLASS_BLADE:
    case Item::ITEM_CLASS_AR:
    case Item::ITEM_CLASS_SR:
    case Item::ITEM_CLASS_SMG:
    case Item::ITEM_CLASS_SG:
        bisWeapon = true;
        m_SlayerInfo.setWeaponType(WEAPON_NONE);
        _GCTakeOff.setObjectID(getObjectID());
        _GCTakeOff.setSlotID((SlotID_t)ADDON_RIGHTHAND);
        m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, this);
        break;
    case Item::ITEM_CLASS_HELM:
        m_SlayerInfo.setHelmetType(HELMET_NONE);
        _GCTakeOff.setObjectID(getObjectID());
        _GCTakeOff.setSlotID((SlotID_t)ADDON_HELM);
        m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, this);
        break;
    case Item::ITEM_CLASS_SHIELD:
        m_SlayerInfo.setShieldType(SHIELD_NONE);
        _GCTakeOff.setObjectID(getObjectID());
        _GCTakeOff.setSlotID((SlotID_t)ADDON_LEFTHAND);
        m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, this);
        break;
    case Item::ITEM_CLASS_SWORD:
        bisWeapon = true;
        m_SlayerInfo.setWeaponType(WEAPON_NONE);
        _GCTakeOff.setObjectID(getObjectID());
        _GCTakeOff.setSlotID((SlotID_t)ADDON_RIGHTHAND);
        m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, this);
        break;
    case Item::ITEM_CLASS_COAT:
        m_SlayerInfo.setJacketType(JACKET_BASIC);
        _GCTakeOff.setObjectID(getObjectID());
        _GCTakeOff.setSlotID((SlotID_t)ADDON_COAT);
        m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, this);
        break;
    case Item::ITEM_CLASS_TROUSER:
        m_SlayerInfo.setPantsType(PANTS_BASIC);
        _GCTakeOff.setObjectID(getObjectID());
        _GCTakeOff.setSlotID((SlotID_t)ADDON_TROUSER);
        m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, this);
        break;
    case Item::ITEM_CLASS_SHOULDER_ARMOR:
        m_SlayerInfo.setShoulderType(0);
        _GCTakeOff.setObjectID(getObjectID());
        _GCTakeOff.setSlotID((SlotID_t)ADDON_SHOULDER);
        m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, this);
        break;
    default:
        break;
    }


    if (m_pZone != NULL) {
        GCOtherModifyInfo gcOtherModifyInfo;
        makeGCOtherModifyInfo(&gcOtherModifyInfo, this, &prev);

        if (gcOtherModifyInfo.getShortCount() != 0 || gcOtherModifyInfo.getLongCount() != 0) {
            m_pZone->broadcastPacket(m_X, m_Y, &gcOtherModifyInfo, this);
        }
    }

    __END_DEBUG
    __END_CATCH
}

// destroyGears

void Slayer::destroyGears()

{
    __BEGIN_DEBUG

    for (int j = 0; j < WEAR_MAX; j++) {
        Item* pItem = m_pWearItem[j];
        if (pItem != NULL) {
            if (isTwohandWeapon(pItem)) {
                m_pWearItem[WEAR_RIGHTHAND] = NULL;
                m_pWearItem[WEAR_LEFTHAND] = NULL;
            } else
                m_pWearItem[j] = NULL;

            SAFE_DELETE(pItem);
        }
    }

    __END_DEBUG
}

bool Slayer::isRealWearing(WearPart part) const

{
    __BEGIN_TRY

    if (part >= WEAR_MAX)
        throw("Slayer::isRealWearing() : invalid wear point!");
    if (m_pWearItem[part] == NULL)
        return false;
    if (part >= WEAR_ZAP1 && part <= WEAR_ZAP4) {
        if (m_pWearItem[part - WEAR_ZAP1 + WEAR_FINGER1] == NULL)
            return false;
    }

    return isRealWearing(m_pWearItem[part]);

    __END_CATCH
}

bool Slayer::isRealWearing(Item* pItem) const

{
    __BEGIN_TRY

    if (pItem == NULL)
        return false;

    /*	if ( m_pZone != NULL && m_pZone->isDynamicZone() && m_pZone->getDynamicZone()->getTemplateZoneID() == 4003 )
        {
            if ( !isSlayerWeapon( pItem->getItemClass() ) ) return false;
        }*/

    ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());

    Level_t ReqAdvancedLevel = pItemInfo->getReqAdvancedLevel();
    if (ReqAdvancedLevel > 0 && (!isAdvanced() || getAdvancementClassLevel() < ReqAdvancedLevel))
        return false;

    if (isSlayerWeapon(pItem->getItemClass()) || pItem->getItemClass() == Item::ITEM_CLASS_COAT ||
        pItem->getItemClass() == Item::ITEM_CLASS_TROUSER) {
        if (ReqAdvancedLevel <= 0 && isAdvanced())
            return false;
    }


    if (pItem->isTimeLimitItem()) {
        Attr_t ReqGender = pItemInfo->getReqGender();
        if ((m_Sex == MALE && ReqGender == GENDER_FEMALE) || (m_Sex == FEMALE && ReqGender == GENDER_MALE))
            return false;
        return true;
    }


    if (getZone()->isPremiumZone() &&
        (pItem->isUnique() || pItem->getOptionTypeSize() > 1 || pItem->getItemClass() == Item::ITEM_CLASS_COUPLE_RING ||
         pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_COUPLE_RING)) {
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(getPlayer());
        if (!pGamePlayer->isPayPlaying() && !pGamePlayer->isPremiumPlay()) {
            // cout << "Premium Item :" << pItem->getItemClassName().c_str() << endl;
            return false;
        }
    }

    if (isCoupleRing(pItem)) {
        return true;
    }

    Attr_t ReqSTR = pItemInfo->getReqSTR();
    Attr_t ReqDEX = pItemInfo->getReqDEX();
    Attr_t ReqINT = pItemInfo->getReqINT();
    Attr_t ReqSum = pItemInfo->getReqSum();
    Attr_t ReqGender = pItemInfo->getReqGender();


    Attr_t ReqSumMax = ((ReqSum > MAX_SLAYER_SUM_OLD) ? MAX_SLAYER_SUM : MAX_SLAYER_SUM_OLD);
    Attr_t ReqSTRMax = ((ReqSTR > MAX_SLAYER_ATTR_OLD) ? MAX_SLAYER_ATTR : MAX_SLAYER_ATTR_OLD);
    Attr_t ReqDEXMax = ((ReqDEX > MAX_SLAYER_ATTR_OLD) ? MAX_SLAYER_ATTR : MAX_SLAYER_ATTR_OLD);
    Attr_t ReqINTMax = ((ReqINT > MAX_SLAYER_ATTR_OLD) ? MAX_SLAYER_ATTR : MAX_SLAYER_ATTR_OLD);


    const list<OptionType_t>& optionTypes = pItem->getOptionTypeList();
    if (!optionTypes.empty()) {
        list<OptionType_t>::const_iterator itr;
        for (itr = optionTypes.begin(); itr != optionTypes.end(); itr++) {
            OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo(*itr);

            if (ReqSTR != 0)
                ReqSTR += (pOptionInfo->getReqSum() * 2);
            if (ReqDEX != 0)
                ReqDEX += (pOptionInfo->getReqSum() * 2);
            if (ReqINT != 0)
                ReqINT += (pOptionInfo->getReqSum() * 2);
            if (ReqSum != 0)
                ReqSum += (pOptionInfo->getReqSum());
        }

        /*
        ReqSTR = max((int)ReqSTR, pOptionInfo->getReqSTR());
        ReqDEX = max((int)ReqDEX, pOptionInfo->getReqDEX());
        ReqINT = max((int)ReqINT, pOptionInfo->getReqINT());
        ReqSum = max((int)ReqSum, pOptionInfo->getReqSum());
        */
    }

    // 2003.1.6 by Sequoia, Bezz

    ReqSTR = min(ReqSTR, ReqSTRMax);
    ReqDEX = min(ReqDEX, ReqDEXMax);
    ReqINT = min(ReqINT, ReqINTMax);
    ReqSum = min(ReqSum, ReqSumMax);


    Attr_t CSTR = m_STR[ATTR_CURRENT];
    Attr_t CDEX = m_DEX[ATTR_CURRENT];
    Attr_t CINT = m_INT[ATTR_CURRENT];
    Attr_t CSUM = CSTR + CDEX + CINT;

    if (CSTR < ReqSTR || CDEX < ReqDEX || CINT < ReqINT || CSUM < ReqSum ||
        m_Sex == MALE && ReqGender == GENDER_FEMALE || m_Sex == FEMALE && ReqGender == GENDER_MALE) {
        // cout << "Disable: " << pItem->getItemClassName().c_str() << endl;
        return false;
    }

    // cout << "Enable: " << pItem->getItemClassName().c_str() << endl;

    return true;

    __END_CATCH
}

bool Slayer::isRealWearingEx(WearPart part) const {
    if (part >= WEAR_MAX)
        return false;
    return m_pRealWearingCheck[part];
}

DWORD Slayer::sendRealWearingInfo(void) const

{
    __BEGIN_TRY

    DWORD info = 0;
    DWORD flag = 1;

    for (int i = 0; i < WEAR_MAX; i++) {
        if (isRealWearing((Slayer::WearPart)i))
            info |= flag;
        flag <<= 1;
    }

    GCRealWearingInfo pkt;
    pkt.setInfo(info);
    m_pPlayer->sendPacket(&pkt);

    return info;

    __END_CATCH
}

void Slayer::setMotorcycle(Motorcycle* pMotorcycle)

{
    __BEGIN_DEBUG


    m_pMotorcycle = pMotorcycle;


    // m_SlayerInfo.setMotorcycleType(MOTORCYCLE1);
    // by sigi.2002.6.22
    m_SlayerInfo.setMotorcycleType(getMotorcycleType(pMotorcycle->getItemType()));

    if (!pMotorcycle->hasOptionType()) {
        m_SlayerInfo.setMotorcycleColor(388);
    } else {
        OptionType_t option = pMotorcycle->getFirstOptionType();
        OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo(option);

        if (pOptionInfo != NULL) {
            m_SlayerInfo.setMotorcycleColor(pOptionInfo->getColor());
        } else {
            m_SlayerInfo.setMotorcycleColor(388);
        }
    }

    __END_DEBUG
}

void Slayer::getOffMotorcycle()

{
    __BEGIN_DEBUG


    TPOINT pt = m_pZone->addItem((Item*)m_pMotorcycle, m_X, m_Y);

    if (pt.x != -1) {
        // m_pMotorcycle->save("", STORAGE_ZONE, m_pZone->getZoneID(), pt.x, pt.y);


        char pField[128];
        sprintf(pField, "OwnerID='', Storage=%d, StorageID=%u, X=%d, Y=%d", STORAGE_ZONE, m_pZone->getZoneID(),
                (int)pt.x, (int)pt.y);
        m_pMotorcycle->tinysave(pField);

        MotorcycleBox* pMotorcycleBox = g_pParkingCenter->getMotorcycleBox(m_pMotorcycle->getItemID());

        if (pMotorcycleBox != NULL) {
            pMotorcycleBox->setZone(m_pZone);
            pMotorcycleBox->setX(pt.x);
            pMotorcycleBox->setY(pt.y);
        } else {
            // cout << "Slayer::getOffMotorcycle() - pMotorcycleBox is NULL" << endl;
            filelog("errorLog.txt", "Slayer::getOffMotorcycle() - No MotorcycleBox: %d",
                    (int)m_pMotorcycle->getItemID());
        }
    } else {
        if (g_pParkingCenter->hasMotorcycleBox(m_pMotorcycle->getItemID())) {
            g_pParkingCenter->deleteMotorcycleBox(m_pMotorcycle->getItemID());
        }
    }


    m_pMotorcycle = NULL;


    m_SlayerInfo.setMotorcycleType(MOTORCYCLE_NONE);

    __END_DEBUG
}

PCSlayerInfo2* Slayer::getSlayerInfo2() const

{
    __BEGIN_DEBUG

    PCSlayerInfo2* pInfo = new PCSlayerInfo2();

    pInfo->setObjectID(m_ObjectID);
    pInfo->setName(m_Name);
    pInfo->setSex(m_Sex);
    pInfo->setHairStyle(m_HairStyle);
    pInfo->setHairColor(m_HairColor);
    pInfo->setSkinColor(m_SkinColor);
    pInfo->setMasterEffectColor(m_MasterEffectColor);

    // cout << "PCSlayerInfo2: HairStyle = " << HairStyle2String[pInfo->getHairStyle()] << endl;

    // pInfo->setPhoneNumber(m_PhoneNumber);


    pInfo->setAlignment(m_Alignment);

    // cout << "STR[CURRENT]" << (int)m_STR[ATTR_CURRENT] << endl;
    // cout << "STR[MAX]" << (int)m_STR[ATTR_MAX] << endl;
    // cout << "STR[BASIC]" << (int)m_STR[ATTR_BASIC] << endl;
    // cout << "DEX[CURRENT]" << (int)m_DEX[ATTR_CURRENT] << endl;
    // cout << "DEX[MAX]" << (int)m_DEX[ATTR_MAX] << endl;
    // cout << "DEX[BASIC]" << (int)m_DEX[ATTR_BASIC] << endl;
    // cout << "INT[CURRENT]" << (int)m_INT[ATTR_CURRENT] << endl;
    // cout << "INT[MAX]" << (int)m_INT[ATTR_MAX] << endl;
    // cout << "INT[BASIC]" << (int)m_INT[ATTR_BASIC] << endl;


    pInfo->setSTR(m_STR[ATTR_CURRENT], ATTR_CURRENT);
    pInfo->setSTR(m_STR[ATTR_MAX], ATTR_MAX);
    pInfo->setSTR(m_STR[ATTR_BASIC], ATTR_BASIC);
    pInfo->setDEX(m_DEX[ATTR_CURRENT], ATTR_CURRENT);
    pInfo->setDEX(m_DEX[ATTR_MAX], ATTR_MAX);
    pInfo->setDEX(m_DEX[ATTR_BASIC], ATTR_BASIC);
    pInfo->setINT(m_INT[ATTR_CURRENT], ATTR_CURRENT);
    pInfo->setINT(m_INT[ATTR_MAX], ATTR_MAX);
    pInfo->setINT(m_INT[ATTR_BASIC], ATTR_BASIC);


    //	pInfo->setSTRExp(m_STRExp);
    //	pInfo->setDEXExp(m_DEXExp);
    //	pInfo->setINTExp(m_INTExp);
    pInfo->setSTRExp(getSTRGoalExp());
    pInfo->setDEXExp(getDEXGoalExp());
    pInfo->setINTExp(getINTGoalExp());


    pInfo->setRank(getRank());
    pInfo->setRankExp(getRankGoalExp());

    //	cout << getRankGoalExp() << endl;

    pInfo->setHP(m_HP[ATTR_CURRENT], m_HP[ATTR_MAX]);
    pInfo->setMP(m_MP[ATTR_CURRENT], m_MP[ATTR_MAX]);
    pInfo->setFame(m_Fame);
    pInfo->setGold(m_Gold);
    pInfo->setSkillDomain(SKILL_DOMAIN_BLADE, m_SkillDomainLevels[SKILL_DOMAIN_BLADE], m_GoalExp[SKILL_DOMAIN_BLADE]);
    pInfo->setSkillDomain(SKILL_DOMAIN_SWORD, m_SkillDomainLevels[SKILL_DOMAIN_SWORD], m_GoalExp[SKILL_DOMAIN_SWORD]);
    pInfo->setSkillDomain(SKILL_DOMAIN_GUN, m_SkillDomainLevels[SKILL_DOMAIN_GUN], m_GoalExp[SKILL_DOMAIN_GUN]);
    pInfo->setSkillDomain(SKILL_DOMAIN_ETC, m_SkillDomainLevels[SKILL_DOMAIN_ETC], m_GoalExp[SKILL_DOMAIN_ETC]);
    pInfo->setSkillDomain(SKILL_DOMAIN_ENCHANT, m_SkillDomainLevels[SKILL_DOMAIN_ENCHANT],
                          m_GoalExp[SKILL_DOMAIN_ENCHANT]);
    pInfo->setSkillDomain(SKILL_DOMAIN_HEAL, m_SkillDomainLevels[SKILL_DOMAIN_HEAL], m_GoalExp[SKILL_DOMAIN_HEAL]);
    pInfo->setSight(m_Sight);

    //	for (int i = 0; i < 4; i++)
    //	{
    //		pInfo->setHotKey(i, m_HotKey[i]);
    //	}


    pInfo->setCompetence(m_CompetenceShape);
    pInfo->setGuildID(m_GuildID);
    pInfo->setGuildName(getGuildName());
    pInfo->setGuildMemberRank(getGuildMemberRank());

    GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion(m_GuildID);
    if (pUnion == NULL)
        pInfo->setUnionID(0);
    else
        pInfo->setUnionID(pUnion->getUnionID());

    pInfo->setAdvancementLevel(getAdvancementClassLevel());
    pInfo->setAdvancementGoalExp(getAdvancementClassGoalExp());
    pInfo->setAttrBonus(getBonus());

    return pInfo;

    __END_DEBUG
}

PCSlayerInfo3 Slayer::getSlayerInfo3() const

{
    __BEGIN_DEBUG


    m_SlayerInfo.setObjectID(m_ObjectID);
    m_SlayerInfo.setX(m_X);
    m_SlayerInfo.setY(m_Y);
    m_SlayerInfo.setDir(m_Dir);
    m_SlayerInfo.setCurrentHP(m_HP[ATTR_CURRENT]);
    m_SlayerInfo.setMaxHP(m_HP[ATTR_MAX]);
    m_SlayerInfo.setAlignment(m_Alignment);
    m_SlayerInfo.setGuildID(m_GuildID);


    m_SlayerInfo.setAttackSpeed(m_AttackSpeed[ATTR_CURRENT]);

    // by sigi. 2002.9.10
    m_SlayerInfo.setRank(getRank());


    m_SlayerInfo.setHairColor(m_HairColor);
    m_SlayerInfo.setSkinColor(m_SkinColor);
    m_SlayerInfo.setMasterEffectColor(m_MasterEffectColor);

    GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion(m_GuildID);
    if (pUnion == NULL)
        m_SlayerInfo.setUnionID(0);
    else
        m_SlayerInfo.setUnionID(pUnion->getUnionID());

    m_SlayerInfo.setAdvancementLevel(getAdvancementClassLevel());

    return m_SlayerInfo;

    __END_DEBUG
}

ExtraInfo* Slayer::getExtraInfo() const

{
    __BEGIN_DEBUG

    BYTE ItemCount = 0;

    ExtraInfo* pExtraInfo = new ExtraInfo();

    Item* pItem = m_pExtraInventorySlot->getItem();

    if (pItem != NULL) {
        // Item::ItemClass IClass = pItem->getItemClass();

        ExtraSlotInfo* pExtraSlotInfo = new ExtraSlotInfo();
        pItem->makePCItemInfo(*pExtraSlotInfo);


        pExtraInfo->addListElement(pExtraSlotInfo);

        ItemCount++;
    }

    pExtraInfo->setListNum(ItemCount);

    return pExtraInfo;

    __END_DEBUG
}

GearInfo* Slayer::getGearInfo() const

{
    __BEGIN_DEBUG

    BYTE ItemCount = 0;
    GearInfo* pGearInfo = new GearInfo();

    for (int i = 0; i < WEAR_MAX; i++) {
        Item* pItem = m_pWearItem[i];

        if (pItem != NULL) {
            //			Item::ItemClass IClass = pItem->getItemClass();

            GearSlotInfo* pGearSlotInfo = new GearSlotInfo();
            pItem->makePCItemInfo(*pGearSlotInfo);

            pGearSlotInfo->setSlotID(i);


            pGearInfo->addListElement(pGearSlotInfo);

            ItemCount++;
        }
    }

    pGearInfo->setListNum(ItemCount);

    return pGearInfo;

    __END_DEBUG
}

//////////////////////////////////////////////////////////////////////////////
// get Inventory Info
//////////////////////////////////////////////////////////////////////////////
InventoryInfo* Slayer::getInventoryInfo() const

{
    __BEGIN_DEBUG

    BYTE ItemCount = 0;
    InventoryInfo* pInventoryInfo = new InventoryInfo();
    list<Item*> ItemList;
    VolumeHeight_t Height = m_pInventory->getHeight();
    VolumeWidth_t Width = m_pInventory->getWidth();

    for (int j = 0; j < Height; j++) {
        for (int i = 0; i < Width; i++) {
            if (m_pInventory->hasItem(i, j)) {
                Item* pItem = m_pInventory->getItem(i, j);
                VolumeWidth_t ItemWidth = pItem->getVolumeWidth();
                //				Item::ItemClass IClass = pItem->getItemClass();

                list<Item*>::iterator itr = find(ItemList.begin(), ItemList.end(), pItem);

                if (itr == ItemList.end()) {
                    ItemList.push_back(pItem);

                    InventorySlotInfo* pInventorySlotInfo = new InventorySlotInfo();
                    pItem->makePCItemInfo(*pInventorySlotInfo);
                    pInventorySlotInfo->setInvenX(i);
                    pInventorySlotInfo->setInvenY(j);


                    pInventoryInfo->addListElement(pInventorySlotInfo);
                    ItemCount++;
                    i = i + ItemWidth - 1;
                }
            }
        }
    }

    pInventoryInfo->setListNum(ItemCount);

    return pInventoryInfo;

    __END_DEBUG
}

RideMotorcycleInfo* Slayer::getRideMotorcycleInfo() const

{
    __BEGIN_DEBUG

    RideMotorcycleInfo* pRideMotorcycleInfo = new RideMotorcycleInfo();

    pRideMotorcycleInfo->setObjectID(m_pMotorcycle->getObjectID());
    pRideMotorcycleInfo->setItemType(m_pMotorcycle->getItemType());
    pRideMotorcycleInfo->setOptionType(m_pMotorcycle->getOptionTypeList());
    pRideMotorcycleInfo->setListNum(0);

    m_SlayerInfo.setMotorcycleColor(388);

    return pRideMotorcycleInfo;

    __END_DEBUG
}

void Slayer::sendSlayerSkillInfo()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    try {
        SlayerSkillInfo* pSlayerSkillInfo[SKILL_DOMAIN_VAMPIRE];

        for (int i = 0; i < SKILL_DOMAIN_VAMPIRE; i++) {
            pSlayerSkillInfo[i] = new SlayerSkillInfo();
            pSlayerSkillInfo[i]->setDomainType((SkillDomainType_t)i);
        }

        BYTE SkillCount[SKILL_DOMAIN_VAMPIRE] = {
            0,
        };
        SkillInfo* pSkillInfo = NULL;
        SkillDomainType_t SDomainType = 0;


        Timeval currentTime;
        getCurrentTime(currentTime);

        unordered_map<SkillType_t, SkillSlot*>::const_iterator itr = m_SkillSlot.begin();
        for (; itr != m_SkillSlot.end(); itr++) {
            SkillSlot* pSkillSlot = itr->second;
            Assert(pSkillSlot != NULL);


            if (pSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT) {
                pSkillInfo = g_pSkillInfoManager->getSkillInfo(pSkillSlot->getSkillType());


                SDomainType = pSkillInfo->getDomainType();


                SubSlayerSkillInfo* pSubSlayerSkillInfo = new SubSlayerSkillInfo();
                pSubSlayerSkillInfo->setSkillType(pSkillSlot->getSkillType());
                pSubSlayerSkillInfo->setSkillExp(pSkillSlot->getExp());
                pSubSlayerSkillInfo->setSkillExpLevel(pSkillSlot->getExpLevel());
                pSubSlayerSkillInfo->setSkillTurn(pSkillSlot->getInterval());


                // pSubSlayerSkillInfo->setCastingTime(pSkillSlot->getCastingTime());
                pSubSlayerSkillInfo->setCastingTime(pSkillSlot->getRemainTurn(currentTime));
                pSubSlayerSkillInfo->setEnable(pSkillSlot->canUse());


                pSlayerSkillInfo[SDomainType]->addListElement(pSubSlayerSkillInfo);
                SkillCount[SDomainType]++;
                pSlayerSkillInfo[SDomainType]->setListNum(SkillCount[SDomainType]);
            }
        }

        pSlayerSkillInfo[SDomainType]->setListNum(SkillCount[SDomainType]);
        GCSkillInfo gcSkillInfo;
        gcSkillInfo.setPCType(PC_SLAYER);

        for (int i = 0; i < SKILL_DOMAIN_VAMPIRE; i++) {
            SkillType_t LearnSkillType = g_pSkillInfoManager->getSkillTypeByLevel(i, m_SkillDomainLevels[i]);


            if (LearnSkillType != 0) {
                if (hasSkill(LearnSkillType) == NULL) {
                    pSlayerSkillInfo[i]->setLearnNewSkill(true);
                }
            }

            if (pSlayerSkillInfo[i]->isLearnNewSkill() || pSlayerSkillInfo[i]->getListNum() > 0) {
                gcSkillInfo.addListElement(pSlayerSkillInfo[i]);
            } else {
                SAFE_DELETE(pSlayerSkillInfo[i]);
            }
        }

        m_pPlayer->sendPacket(&gcSkillInfo);
    } catch (Throwable& t) {
        filelog("slayerBug.log", "%s", t.toString().c_str());
    }


    __END_DEBUG
    __END_CATCH
}


EffectInfo* Slayer::getEffectInfo() const {
    EffectInfo* pEffectInfo = m_pEffectManager->getEffectInfo();
    return pEffectInfo;
}

void Slayer::setGold(Gold_t gold)

{
    __BEGIN_TRY


    // 2003.1.8  by bezz.
    m_Gold = min((Gold_t)MAX_MONEY, gold);

    __END_CATCH
}

void Slayer::setGoldEx(Gold_t gold)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    setGold(gold);

    /*
    StringStream sql;
    sql << "Gold = " << (int)m_Gold;

    tinysave(sql.toString());
    */
    char pField[128];
    sprintf(pField, "Gold = %ld", m_Gold);
    tinysave(pField);

    __END_DEBUG
    __END_CATCH
}

void Slayer::increaseGoldEx(Gold_t gold)

{
    __BEGIN_TRY
    __BEGIN_DEBUG


    // 2003.1.8  by bezz.
    if (m_Gold + gold > MAX_MONEY)
        gold = MAX_MONEY - m_Gold;

    setGold(m_Gold + gold);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement increaseGoldStmt(pConn, "UPDATE Slayer SET Gold=Gold+? WHERE NAME=?");
        increaseGoldStmt.bindUInt(1, gold);
        increaseGoldStmt.bindString(2, m_Name);
        increaseGoldStmt.execute();
    }
    END_DB(pStmt)


    __END_DEBUG
    __END_CATCH
}

void Slayer::decreaseGoldEx(Gold_t gold)

{
    __BEGIN_TRY
    __BEGIN_DEBUG


    // 2003.1.8  by bezz.
    if (m_Gold < gold)
        gold = m_Gold;

    setGold(m_Gold - gold);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement decreaseGoldStmt(pConn, "UPDATE Slayer SET Gold=Gold-? WHERE NAME=?");
        decreaseGoldStmt.bindUInt(1, gold);
        decreaseGoldStmt.bindString(2, m_Name);
        decreaseGoldStmt.execute();
    }
    END_DB(pStmt)

    __END_DEBUG
    __END_CATCH
}

bool Slayer::checkGoldIntegrity() {
    __BEGIN_TRY

    Statement* pStmt = NULL;
    bool ret = false;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectGoldStmt(pConn, "SELECT Gold FROM Slayer WHERE NAME=?");
        selectGoldStmt.bindString(1, m_Name);
        Result* pResult = selectGoldStmt.execute();

        if (pResult->next()) {
            ret = pResult->getInt(1) == m_Gold;
        }
    }
    END_DB(pStmt)

    return ret;

    __END_CATCH
}

bool Slayer::checkStashGoldIntegrity() {
    __BEGIN_TRY

    Statement* pStmt = NULL;
    bool ret = false;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectStashGoldStmt(pConn, "SELECT StashGold FROM Slayer WHERE NAME=?");
        selectStashGoldStmt.bindString(1, m_Name);
        Result* pResult = selectStashGoldStmt.execute();

        if (pResult->next()) {
            ret = pResult->getInt(1) == m_StashGold;
        }
    }
    END_DB(pStmt)

    return ret;

    __END_CATCH
}


void Slayer::heartbeat(const Timeval& currentTime)

{
    __BEGIN_DEBUG

    /*#ifdef __ACTIVE_QUEST__
        // by sigi. 2002.12.3
        if (m_pQuestManager!=NULL)
        {
            m_pQuestManager->heartbeat();
        }
    #endif*/

    PlayerCreature::heartbeat(currentTime);


    Item* pWeapon = getWearItem(Slayer::WEAR_RIGHTHAND);
    if (pWeapon != NULL) {
        Item::ItemClass IClass = pWeapon->getItemClass();

        SkillSlot* pPrayer = hasSkill(SKILL_PRAYER);
        SkillSlot* pMeditation = hasSkill(SKILL_MEDITATION);

        if (IClass == Item::ITEM_CLASS_CROSS && pPrayer != NULL && pPrayer->canUse() &&
            !isFlag(Effect::EFFECT_CLASS_PLEASURE_EXPLOSION)) {
            Timeval currentTime;
            getCurrentTime(currentTime);

            if (m_MPRegenTime < currentTime) {
                MP_t MPMax = getMP(ATTR_MAX);
                int MPRegenPercent = 3 + getINT(ATTR_CURRENT) / 20;
                MP_t MPQuantity = max(1, getPercentValue(MPMax, MPRegenPercent));
                MP_t oldMP = getMP(ATTR_CURRENT);
                MP_t newMP = min((int)MPMax, (int)(oldMP + MPQuantity));

                if (oldMP != newMP) {
                    setMP(newMP, ATTR_CURRENT);

                    GCModifyInformation gcMI;
                    gcMI.addShortData(MODIFY_CURRENT_MP, newMP);
                    m_pPlayer->sendPacket(&gcMI);
                }


                m_MPRegenTime.tv_sec = currentTime.tv_sec + 5;
                m_MPRegenTime.tv_usec = currentTime.tv_usec;
            }
        } else if (IClass == Item::ITEM_CLASS_MACE && pMeditation != NULL && pMeditation->canUse() &&
                   !isFlag(Effect::EFFECT_CLASS_PLEASURE_EXPLOSION)) {
            Timeval currentTime;
            getCurrentTime(currentTime);

            if (m_MPRegenTime < currentTime) {
                MP_t MPMax = getMP(ATTR_MAX);
                int MPRegenPercent = 3 + getINT(ATTR_CURRENT) / 20;
                MP_t MPQuantity = max(1, getPercentValue(MPMax, MPRegenPercent));
                MP_t oldMP = getMP(ATTR_CURRENT);
                MP_t newMP = min((int)MPMax, (int)(oldMP + MPQuantity));

                if (oldMP != newMP) {
                    setMP(newMP, ATTR_CURRENT);

                    GCModifyInformation gcMI;
                    gcMI.addShortData(MODIFY_CURRENT_MP, newMP);
                    m_pPlayer->sendPacket(&gcMI);
                }


                m_MPRegenTime.tv_sec = (currentTime.tv_sec + 5);
                m_MPRegenTime.tv_usec = currentTime.tv_usec;
            }
        }
    }


    __END_DEBUG
}

void Slayer::getSlayerRecord(SLAYER_RECORD& record) const

{
    __BEGIN_TRY

    record.pSTR[0] = m_STR[0];
    record.pSTR[1] = m_STR[1];
    record.pSTR[2] = m_STR[2];

    record.pDEX[0] = m_DEX[0];
    record.pDEX[1] = m_DEX[1];
    record.pDEX[2] = m_DEX[2];

    record.pINT[0] = m_INT[0];
    record.pINT[1] = m_INT[1];
    record.pINT[2] = m_INT[2];

    record.pHP[0] = m_HP[0];
    record.pHP[1] = m_HP[1];

    record.pMP[0] = m_MP[0];
    record.pMP[1] = m_MP[1];

    record.Rank = getRank();

    record.pDamage[0] = m_Damage[0];
    record.pDamage[1] = m_Damage[1];

    record.Defense = m_Defense[0];
    record.Protection = m_Protection[0];
    record.ToHit = m_ToHit[0];
    record.AttackSpeed = m_AttackSpeed[0];

    __END_CATCH
}

void Slayer::setResurrectZoneIDEx(ZoneID_t id)

{
    __BEGIN_TRY

    setResurrectZoneID(id);

    /*
    StringStream sql;
    sql << "ResurrectZone = " << (int)id;
    tinysave(sql.toString());
    */

    // by sigi. 2002.5.15
    char pField[128];
    sprintf(pField, "ResurrectZone=%d", id);
    tinysave(pField);

    __END_CATCH
}

void Slayer::saveAlignment(Alignment_t alignment)

{
    __BEGIN_TRY

    setAlignment(alignment);

    /*
    StringStream sql;
    sql << "Alignment = " << (int)alignment;

    tinysave(sql.toString());
    */
    // by sigi. 2002.5.15
    char pField[128];
    sprintf(pField, "Alignment=%d", alignment);
    tinysave(pField);


    __END_CATCH
}

uint Slayer::getSlayerLevel(void) const

{
    __BEGIN_TRY

    uint SumAttr = 0, SumDomain = 0;

    SumAttr += m_STR[ATTR_BASIC];
    SumAttr += m_DEX[ATTR_BASIC];
    SumAttr += m_INT[ATTR_BASIC];

    SumDomain += m_SkillDomainLevels[SKILL_DOMAIN_SWORD];
    SumDomain += m_SkillDomainLevels[SKILL_DOMAIN_BLADE];
    SumDomain += m_SkillDomainLevels[SKILL_DOMAIN_GUN];
    SumDomain += m_SkillDomainLevels[SKILL_DOMAIN_HEAL];
    SumDomain += m_SkillDomainLevels[SKILL_DOMAIN_ENCHANT];

    return (uint)(SumAttr / 4 + SumDomain / 2);

    __END_CATCH
}

string Slayer::toString() const

{
    __BEGIN_DEBUG

    StringStream msg;
    msg << "Slayer("
        //<< "ObjectID:"     << (int)getObjectID()
        << ",Name:" << m_Name << ",Sex:" << Sex2String[m_Sex] << ",HairStyle:" << HairStyle2String[m_HairStyle]
        << ",HairColor:" << (int)m_HairColor << ",SkinColor:" << (int)m_SkinColor << ",Rank:"
        << (int)getRank()
        //		<< ",RankExp:"     << (int)m_RankExp
        << ",RankGoalExp:" << (int)getRankGoalExp() << ",STR:" << (int)m_STR[ATTR_CURRENT] << "/"
        << (int)m_STR[ATTR_MAX] << ",DEX:" << (int)m_DEX[ATTR_CURRENT] << "/" << (int)m_DEX[ATTR_MAX]
        << ",INT:" << (int)m_INT[ATTR_CURRENT] << "/"
        << (int)m_INT[ATTR_MAX]
        //		<< ",STRExp:"      << (int)m_STRExp
        << ",STRGoalExp :"
        << (int)getSTRGoalExp()
        //		<< ",DEXExp:"      << (int)m_DEXExp
        //		<< ",INTExp:"      << (int)m_INTExp
        << ",HP:" << (int)m_HP[ATTR_CURRENT] << "/" << (int)m_HP[ATTR_MAX] << ",MP:" << (int)m_MP[ATTR_CURRENT] << "/"
        << (int)m_MP[ATTR_MAX] << ",Fame:" << (int)m_Fame << ",Gold:" << (int)m_Gold << ",GuildID:" << (int)m_GuildID
        << ",ZoneID:" << (int)getZoneID() << ",X:" << (int)m_X << ",Y:" << (int)m_Y << ",Sight :" << (int)m_Sight
        << ")";
    return msg.toString();

    __END_DEBUG
}

SkillLevel_t Slayer::getSkillDomainLevelSum() const

{
    __BEGIN_TRY

    SkillLevel_t sum = 0;
    sum += m_SkillDomainLevels[SKILL_DOMAIN_BLADE];
    sum += m_SkillDomainLevels[SKILL_DOMAIN_SWORD];
    sum += m_SkillDomainLevels[SKILL_DOMAIN_GUN];
    sum += m_SkillDomainLevels[SKILL_DOMAIN_HEAL];
    sum += m_SkillDomainLevels[SKILL_DOMAIN_ENCHANT];
    return sum;

    __END_CATCH
}


SkillLevel_t Slayer::getHighestSkillDomainLevel() const

{
    __BEGIN_TRY

    SkillLevel_t highest;

    highest = max(m_SkillDomainLevels[SKILL_DOMAIN_BLADE], m_SkillDomainLevels[SKILL_DOMAIN_SWORD]);
    highest = max(highest, m_SkillDomainLevels[SKILL_DOMAIN_GUN]);
    highest = max(highest, m_SkillDomainLevels[SKILL_DOMAIN_HEAL]);
    highest = max(highest, m_SkillDomainLevels[SKILL_DOMAIN_ENCHANT]);

    return highest;

    __END_CATCH
}

SkillDomainType_t Slayer::getHighestSkillDomain() const

{
    __BEGIN_TRY

    SkillDomainType_t highest;

    if (m_SkillDomainLevels[SKILL_DOMAIN_BLADE] > m_SkillDomainLevels[SKILL_DOMAIN_SWORD])
        highest = SKILL_DOMAIN_BLADE;
    else
        highest = SKILL_DOMAIN_SWORD;

    if (m_SkillDomainLevels[SKILL_DOMAIN_GUN] > m_SkillDomainLevels[highest])
        highest = SKILL_DOMAIN_GUN;

    if (m_SkillDomainLevels[SKILL_DOMAIN_HEAL] > m_SkillDomainLevels[highest])
        highest = SKILL_DOMAIN_HEAL;

    if (m_SkillDomainLevels[SKILL_DOMAIN_ENCHANT] > m_SkillDomainLevels[highest])
        highest = SKILL_DOMAIN_ENCHANT;

    return highest;

    __END_CATCH
}

void Slayer::saveSkills(void) const

{
    __BEGIN_TRY

    unordered_map<SkillType_t, SkillSlot*>::const_iterator itr = m_SkillSlot.begin();
    for (; itr != m_SkillSlot.end(); itr++) {
        SkillSlot* pSkillSlot = itr->second;
        Assert(pSkillSlot != NULL);


        if (pSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT) {
            pSkillSlot->save(m_Name);
        }
    }

    __END_CATCH
}

IP_t Slayer::getIP(void) const {
    ///*
    Assert(m_pPlayer != NULL);
    Socket* pSocket = m_pPlayer->getSocket();
    Assert(pSocket != NULL);
    return pSocket->getHostIP();
    //*/
}

void Slayer::saveGears(void) const

{
    __BEGIN_TRY


    char pField[128];

    for (int i = 0; i < Slayer::WEAR_MAX; i++) {
        Item* pItem = m_pWearItem[i];
        if (pItem != NULL) {
            Durability_t maxDurability = computeMaxDurability(pItem);
            if (pItem->getDurability() < maxDurability) {
                if (i == Slayer::WEAR_RIGHTHAND) {
                    if (pItem->isGun()) {
                        //						Gun* pGun = dynamic_cast<Gun*>(pItem);

                        if (pItem != NULL) {
                            // pItem->saveBullet();
                            sprintf(pField, "Durability=%d, BulletCount=%d, Silver=%d", pItem->getDurability(),
                                    pItem->getBulletCount(), pItem->getSilver());
                            pItem->tinysave(pField);
                        }
                    }

                    else // if (pItem->isSilverWeapon())
                    {
                        sprintf(pField, "Durability=%d, Silver=%d", pItem->getDurability(), pItem->getSilver());
                        pItem->tinysave(pField);
                    }
                } else {
                    // pItem->save(m_Name, STORAGE_GEAR, 0, i, 0);

                    sprintf(pField, "Durability=%d", pItem->getDurability());
                    pItem->tinysave(pField);
                }
            }
        }
    }

    __END_CATCH
}

void Slayer::saveExps(void) const

{
    __BEGIN_TRY

    // Divide by 10 to reduce the number of queries in the skill handler,
    // If the server is not down and you log out normally
    // If you don't explicitly save, the part that goes up below 10 will be blown away.
    // So save here.

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement saveExpsSlayerStmt(
            pConn, "UPDATE Slayer SET STRGoalExp=?, DEXGoalExp=?, INTGoalExp=?, BladeGoalExp=?, SwordGoalExp=?, "
                   "GunGoalExp=?, EnchantGoalExp=?, HealGoalExp=?, ETCGoalExp=?, Alignment=?, Fame=?, `Rank`=?, "
                   "RankGoalExp=?, AdvancementClass=?, AdvancementGoalExp=?, AdvancedSTR=?, AdvancedDEX=?, "
                   "AdvancedINT=?, Bonus=? WHERE Name=?");
        saveExpsSlayerStmt.bindULong(1, getSTRGoalExp());
        saveExpsSlayerStmt.bindULong(2, getDEXGoalExp());
        saveExpsSlayerStmt.bindULong(3, getINTGoalExp());
        saveExpsSlayerStmt.bindULong(4, m_GoalExp[SKILL_DOMAIN_BLADE]);
        saveExpsSlayerStmt.bindULong(5, m_GoalExp[SKILL_DOMAIN_SWORD]);
        saveExpsSlayerStmt.bindULong(6, m_GoalExp[SKILL_DOMAIN_GUN]);
        saveExpsSlayerStmt.bindULong(7, m_GoalExp[SKILL_DOMAIN_ENCHANT]);
        saveExpsSlayerStmt.bindULong(8, m_GoalExp[SKILL_DOMAIN_HEAL]);
        saveExpsSlayerStmt.bindULong(9, m_GoalExp[SKILL_DOMAIN_ETC]);
        saveExpsSlayerStmt.bindInt(10, m_Alignment);
        saveExpsSlayerStmt.bindLong(11, m_Fame);
        saveExpsSlayerStmt.bindInt(12, getRank());
        saveExpsSlayerStmt.bindULong(13, getRankGoalExp());
        saveExpsSlayerStmt.bindUInt(14, getAdvancementClassLevel());
        saveExpsSlayerStmt.bindInt(15, getAdvancementClassGoalExp());
        saveExpsSlayerStmt.bindUInt(16, m_AdvancedSTR);
        saveExpsSlayerStmt.bindUInt(17, m_AdvancedDEX);
        saveExpsSlayerStmt.bindUInt(18, m_AdvancedINT);
        saveExpsSlayerStmt.bindUInt(19, m_AdvancedAttrBonus);
        saveExpsSlayerStmt.bindString(20, m_Name);
        saveExpsSlayerStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

//----------------------------------------------------------------------
// getShapeInfo
//----------------------------------------------------------------------


//----------------------------------------------------------------------


//


//----------------------------------------------------------------------
void Slayer::getShapeInfo(DWORD& flag, Color_t colors[PCSlayerInfo::SLAYER_COLOR_MAX]) const
//
{
    __BEGIN_DEBUG

    Item* pItem;
    // OptionInfo* 				pOptionInfo;
    PCSlayerInfo::SlayerBits slayerBit;
    PCSlayerInfo::SlayerColors slayerColor;

    WearPart Part;


    flag = 0;

    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    slayerBit = PCSlayerInfo::SLAYER_BIT_SEX;
    flag |= ((m_Sex ? 1 : 0) << slayerBit);

    //-----------------------------------------------------------------
    // HairStyle
    //-----------------------------------------------------------------
    slayerBit = PCSlayerInfo::SLAYER_BIT_HAIRSTYLE1;
    flag |= (m_HairStyle << slayerBit);

    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    Part = WEAR_LEG;
    pItem = m_pWearItem[Part];
    slayerBit = PCSlayerInfo::SLAYER_BIT_PANTS1;
    slayerColor = PCSlayerInfo::SLAYER_COLOR_PANTS;
    if (pItem != NULL && m_pRealWearingCheck[Part]) {
        ItemType_t IType = pItem->getItemType();

        /*
        if (pItem->getOptionType() != 0)
        {
            pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getOptionType());
            colors[slayerColor] = (pOptionInfo==NULL? 377 : pOptionInfo->getColor());
        }
        */

        //		colors[slayerColor] = (pItem->isUnique()? UNIQUE_OPTION : pItem->getFirstOptionType());
        if (pItem->isTimeLimitItem())
            colors[slayerColor] = QUEST_OPTION;
        else if (pItem->isUnique())
            colors[slayerColor] = UNIQUE_OPTION;
        else
            colors[slayerColor] = pItem->getFirstOptionType();

        flag |= (getPantsType(IType) << slayerBit);
    } else {
        colors[slayerColor] = 0;
        flag |= (PANTS_BASIC << slayerBit);
    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    Part = WEAR_BODY;
    pItem = m_pWearItem[Part];
    slayerBit = PCSlayerInfo::SLAYER_BIT_JACKET1;
    slayerColor = PCSlayerInfo::SLAYER_COLOR_JACKET;
    if (pItem != NULL && m_pRealWearingCheck[Part]) {
        ItemType_t IType = pItem->getItemType();

        /*
        if (pItem->getOptionType() != 0)
        {
            pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getOptionType());
            colors[slayerColor] = (pOptionInfo==NULL? 377 : pOptionInfo->getColor());
        }
        */

        //		colors[slayerColor] = (pItem->isUnique()? UNIQUE_OPTION : pItem->getFirstOptionType());

        if (pItem->isTimeLimitItem())
            colors[slayerColor] = QUEST_OPTION;
        else if (pItem->isUnique())
            colors[slayerColor] = UNIQUE_OPTION;
        else
            colors[slayerColor] = pItem->getFirstOptionType();

        flag |= (getJacketType(IType) << slayerBit);
    } else {
        colors[slayerColor] = 0;
        flag |= (JACKET_BASIC << slayerBit);
    }

    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    Part = WEAR_HEAD;
    pItem = m_pWearItem[Part];
    slayerBit = PCSlayerInfo::SLAYER_BIT_HELMET1;
    slayerColor = PCSlayerInfo::SLAYER_COLOR_HELMET;
    if (pItem != NULL && m_pRealWearingCheck[Part]) {
        ItemType_t IType = pItem->getItemType();

        /*
        if (pItem->getOptionType() != 0)
        {
            pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getOptionType());
            colors[slayerColor] = (pOptionInfo==NULL? 377 : pOptionInfo->getColor());
        }
        */

        //		colors[slayerColor] = (pItem->isUnique()? UNIQUE_OPTION : pItem->getFirstOptionType());

        if (pItem->isTimeLimitItem())
            colors[slayerColor] = QUEST_OPTION;
        else if (pItem->isUnique())
            colors[slayerColor] = UNIQUE_OPTION;
        else
            colors[slayerColor] = pItem->getFirstOptionType();

        flag |= (getHelmetType(IType) << slayerBit);

    } else {
        colors[slayerColor] = 0;
    }

    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    Part = WEAR_LEFTHAND;
    pItem = m_pWearItem[Part];
    slayerBit = PCSlayerInfo::SLAYER_BIT_SHIELD1;
    slayerColor = PCSlayerInfo::SLAYER_COLOR_SHIELD;
    if (pItem != NULL && m_pRealWearingCheck[Part] && pItem->getItemClass() == Item::ITEM_CLASS_SHIELD) {
        ItemType_t IType = pItem->getItemType();

        /*
        if (pItem->getOptionType() != 0)
        {
            pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getOptionType());
            colors[slayerColor] = (pOptionInfo==NULL? 377 : pOptionInfo->getColor());
        }
        */

        //		colors[slayerColor] = (pItem->isUnique()? UNIQUE_OPTION : pItem->getFirstOptionType());

        //		if ( pItem->isUnique() ) colors[slayerColor] = UNIQUE_OPTION;
        if (pItem->isTimeLimitItem())
            colors[slayerColor] = QUEST_OPTION;
        else if (pItem->isUnique())
            colors[slayerColor] = UNIQUE_OPTION;
        else
            colors[slayerColor] = pItem->getFirstOptionType();

        flag |= (getShieldType(IType) << slayerBit);
    } else {
        colors[slayerColor] = 0;
    }

    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    Part = WEAR_RIGHTHAND;
    pItem = m_pWearItem[Part];
    slayerBit = PCSlayerInfo::SLAYER_BIT_WEAPON1;
    slayerColor = PCSlayerInfo::SLAYER_COLOR_WEAPON;

    if (pItem != NULL && m_pRealWearingCheck[Part]) {
        DWORD weaponType = 0;

        if (pItem->getItemClass() == Item::ITEM_CLASS_SWORD)
            weaponType = WEAPON_SWORD;
        else if (pItem->getItemClass() == Item::ITEM_CLASS_BLADE)
            weaponType = WEAPON_BLADE;
        else if (pItem->getItemClass() == Item::ITEM_CLASS_SR)
            weaponType = WEAPON_SR;
        else if (pItem->getItemClass() == Item::ITEM_CLASS_AR)
            weaponType = WEAPON_AR;
        else if (pItem->getItemClass() == Item::ITEM_CLASS_SG)
            weaponType = WEAPON_SG;
        else if (pItem->getItemClass() == Item::ITEM_CLASS_SMG)
            weaponType = WEAPON_SMG;
        else if (pItem->getItemClass() == Item::ITEM_CLASS_CROSS)
            weaponType = WEAPON_CROSS;
        else if (pItem->getItemClass() == Item::ITEM_CLASS_MACE)
            weaponType = WEAPON_MACE; // MACE;

        /*
        if (pItem->getOptionType() != 0)
        {
            pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getOptionType());
            colors[slayerColor] = (pOptionInfo==NULL? 377 : pOptionInfo->getColor());
        }
        */

        // colors[slayerColor] = (pItem->isUnique()? UNIQUE_OPTION : pItem->getFirstOptionType());

        if (pItem->isUnique())
            colors[slayerColor] = UNIQUE_OPTION;
        else if (pItem->isTimeLimitItem())
            colors[slayerColor] = QUEST_OPTION;
        else
            colors[slayerColor] = pItem->getFirstOptionType();

        flag |= (weaponType << slayerBit);
    } else {
        colors[slayerColor] = 0;
    }

    __END_DEBUG
}


//----------------------------------------------------------------------
// save InitialRank
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void Slayer::saveInitialRank(void)

{
    int maxDomainLevel = getHighestSkillDomainLevel();

    int curRank = max(1, (maxDomainLevel + 3) / 4);
    m_pRank->SET_LEVEL(curRank);
    /*
        SLAYER_RECORD prev;
        getSlayerRecord(prev);

        RankExp_t accumExp = 0;

        setRank(curRank);

        if (curRank!=1)
        {
            RankEXPInfo* pBeforeExpInfo = g_pRankEXPInfoManager[RANK_TYPE_SLAYER]->getRankEXPInfo(curRank-1);
            accumExp = pBeforeExpInfo->getAccumExp();
        }

        RankEXPInfo* pNextExpInfo = g_pRankEXPInfoManager[RANK_TYPE_SLAYER]->getRankEXPInfo(curRank);
        Exp_t NextGoalExp = pNextExpInfo->getGoalExp();

        setRankGoalExp(NextGoalExp);
    */
    char pField[128];
    sprintf(pField, "`Rank`=%d, RankExp=%lu, RankGoalExp=%lu", getRank(), getRankExp(), getRankGoalExp());
    tinysave(pField);
    setRankExpSaveCount(0);

    /*
    sendModifyInfo(prev);

    if (m_pZone != NULL)
    {
        GCOtherModifyInfo gcOtherModifyInfo;
        gcOtherModifyInfo.setObjectID(getObjectID());
        gcOtherModifyInfo.addShortData(MODIFY_RANK, curRank);

        m_pZone->broadcastPacket(m_X, m_Y, &gcOtherModifyInfo, this);
    }
    */
}

Slayer::WearPart Slayer::getWearPart(Item::ItemClass IClass) const {
    switch (IClass) {
    case Item::ITEM_CLASS_COAT:
        return WEAR_BODY;
    case Item::ITEM_CLASS_TROUSER:
        return WEAR_LEG;

    case Item::ITEM_CLASS_SWORD:
    case Item::ITEM_CLASS_BLADE:
    case Item::ITEM_CLASS_AR:
    case Item::ITEM_CLASS_SR:
    case Item::ITEM_CLASS_SG:
    case Item::ITEM_CLASS_SMG:
    case Item::ITEM_CLASS_MACE:
    case Item::ITEM_CLASS_CROSS:
        return WEAR_RIGHTHAND;

    case Item::ITEM_CLASS_HELM:
        return WEAR_HEAD;

    case Item::ITEM_CLASS_SHIELD:
        return WEAR_LEFTHAND;

    case Item::ITEM_CLASS_SHOULDER_ARMOR:
        return WEAR_SHOULDER;

    default:
        return WEAR_MAX;
    }

    return WEAR_MAX;
}

bool Slayer::changeShape(Item* pItem, Color_t color, bool bSendPacket) {
    Item::ItemClass IClass = pItem->getItemClass();
    ItemType_t IType = pItem->getItemType();

    WearPart Part = getWearPart(IClass);

    if (Part == WEAR_MAX)
        return false;

    bool bRealWear = m_pRealWearingCheck[Part];

    if (bRealWear)
        return addShape(IClass, IType, color);

    return removeShape(IClass, bSendPacket);
}

bool Slayer::addShape(Item::ItemClass IClass, ItemType_t IType, Color_t color) {
    bool bisWeapon = false;
    bool bisChange = false;

    switch (IClass) {
    case Item::ITEM_CLASS_MACE:
        bisWeapon = true;
        bisChange = true;
        // m_SlayerInfo.setWeaponType(WEAPON_MACE);

        m_SlayerInfo.setWeaponType(WEAPON_MACE);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_CROSS:
        bisWeapon = true;
        bisChange = true;

        m_SlayerInfo.setWeaponType(WEAPON_CROSS);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_BLADE:
        bisWeapon = true;
        bisChange = true;

        m_SlayerInfo.setWeaponType(WEAPON_BLADE);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_AR:
        bisWeapon = true;
        bisChange = true;

        m_SlayerInfo.setWeaponType(WEAPON_AR);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_SR:
        bisWeapon = true;
        bisChange = true;

        m_SlayerInfo.setWeaponType(WEAPON_SR);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_SMG:
        bisWeapon = true;
        bisChange = true;

        m_SlayerInfo.setWeaponType(WEAPON_SMG);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_SG:
        bisWeapon = true;
        bisChange = true;

        m_SlayerInfo.setWeaponType(WEAPON_SG);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_HELM:
        bisChange = true;

        m_SlayerInfo.setHelmetType(getHelmetType(IType));
        m_SlayerInfo.setHelmetColor(color);
        break;
    case Item::ITEM_CLASS_SHIELD:
        bisChange = true;

        m_SlayerInfo.setShieldType(getShieldType(IType));
        m_SlayerInfo.setShieldColor(color);
        break;
    case Item::ITEM_CLASS_SWORD:
        bisWeapon = true;
        bisChange = true;

        m_SlayerInfo.setWeaponType(WEAPON_SWORD);
        m_SlayerInfo.setWeaponColor(color);
        break;
    case Item::ITEM_CLASS_COAT:
        bisChange = true;

        m_SlayerInfo.setJacketType(getJacketType(IType));
        m_SlayerInfo.setJacketColor(color);
        break;
    case Item::ITEM_CLASS_TROUSER:
        bisChange = true;

        m_SlayerInfo.setPantsType(getPantsType(IType));
        m_SlayerInfo.setPantsColor(color);
        break;

    case Item::ITEM_CLASS_SHOULDER_ARMOR:
        bisChange = true;

        m_SlayerInfo.setShoulderType(getShoulderType(IType));
        m_SlayerInfo.setShoulderColor(color);
        break;

    default:
        break;
    }

    return bisChange;
}

bool Slayer::removeShape(Item::ItemClass IClass, bool bSendPacket) {
    bool bisWeapon = false;


    // by sigi. 2002.11.7
    Creature* pOwner = (isDead() ? NULL : this);

    switch (IClass) {
    case Item::ITEM_CLASS_MACE:
    case Item::ITEM_CLASS_CROSS:
    case Item::ITEM_CLASS_BLADE:
    case Item::ITEM_CLASS_AR:
    case Item::ITEM_CLASS_SR:
    case Item::ITEM_CLASS_SMG:
    case Item::ITEM_CLASS_SG: {
        bisWeapon = true;
        m_SlayerInfo.setWeaponType(WEAPON_NONE);

        if (bSendPacket) // by sigi. 2002.11.6
        {
            GCTakeOff _GCTakeOff;
            _GCTakeOff.setObjectID(getObjectID());
            _GCTakeOff.setSlotID((SlotID_t)ADDON_RIGHTHAND);
            m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
        }
    } break;

    case Item::ITEM_CLASS_HELM: {
        m_SlayerInfo.setHelmetType(HELMET_NONE);

        if (bSendPacket) // by sigi. 2002.11.6
        {
            GCTakeOff _GCTakeOff;
            _GCTakeOff.setObjectID(getObjectID());
            _GCTakeOff.setSlotID((SlotID_t)ADDON_HELM);
            m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
        }
    } break;

    case Item::ITEM_CLASS_SHIELD: {
        m_SlayerInfo.setShieldType(SHIELD_NONE);

        if (bSendPacket) // by sigi. 2002.11.6
        {
            GCTakeOff _GCTakeOff;
            _GCTakeOff.setObjectID(getObjectID());
            _GCTakeOff.setSlotID((SlotID_t)ADDON_LEFTHAND);
            m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
        }
    } break;

    case Item::ITEM_CLASS_SWORD: {
        bisWeapon = true;
        m_SlayerInfo.setWeaponType(WEAPON_NONE);

        if (bSendPacket) // by sigi. 2002.11.6
        {
            GCTakeOff _GCTakeOff;
            _GCTakeOff.setObjectID(getObjectID());
            _GCTakeOff.setSlotID((SlotID_t)ADDON_RIGHTHAND);
            m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
        }
    } break;

    case Item::ITEM_CLASS_COAT: {
        m_SlayerInfo.setJacketType(JACKET_BASIC);

        if (bSendPacket) // by sigi. 2002.11.6
        {
            GCTakeOff _GCTakeOff;
            _GCTakeOff.setObjectID(getObjectID());
            _GCTakeOff.setSlotID((SlotID_t)ADDON_COAT);
            m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
        }
    } break;

    case Item::ITEM_CLASS_TROUSER: {
        m_SlayerInfo.setPantsType(PANTS_BASIC);

        if (bSendPacket) // by sigi. 2002.11.6
        {
            GCTakeOff _GCTakeOff;
            _GCTakeOff.setObjectID(getObjectID());
            _GCTakeOff.setSlotID((SlotID_t)ADDON_TROUSER);
            m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
        }
    } break;

    case Item::ITEM_CLASS_SHOULDER_ARMOR: {
        m_SlayerInfo.setShoulderType(0);

        if (bSendPacket) // by sigi. 2002.11.6
        {
            GCTakeOff _GCTakeOff;
            _GCTakeOff.setObjectID(getObjectID());
            _GCTakeOff.setSlotID((SlotID_t)ADDON_TROUSER);
            m_pZone->broadcastPacket(getX(), getY(), &_GCTakeOff, pOwner);
        }
    } break;

    default:
        break;
    }

    return bisWeapon;
}

Color_t Slayer::getItemShapeColor(Item* pItem, OptionInfo* pOptionInfo) const {
    Color_t color;

    if (pItem->isTimeLimitItem()) {
        color = QUEST_COLOR;
    } else if (pItem->isUnique()) {
        color = UNIQUE_COLOR;
    }

    else if (pOptionInfo != NULL) {
        color = pOptionInfo->getColor();
    }

    else if (pItem->getFirstOptionType() != 0) {
        OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo(pItem->getFirstOptionType());
        color = pOptionInfo->getColor();
    } else {
        color = 377;
    }

    return color;
}


bool Slayer::canPlayFree()

{
    __BEGIN_TRY

    return getSkillDomainLevelSum() <= g_pVariableManager->getVariable(FREE_PLAY_SLAYER_DOMAIN_SUM);

    __END_CATCH
}

bool Slayer::isPayPlayAvaiable()

{
    __BEGIN_TRY

    if (m_pPlayer == NULL)
        return false;

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);

#ifdef __CONNECT_BILLING_SYSTEM__
    if (pGamePlayer->isPayPlaying()) {
        if (pGamePlayer->getPayType() == PAY_TYPE_FREE)
            return true;

        int DomainSUM = getSkillDomainLevelSum();


        if (DomainSUM <= g_pVariableManager->getVariable(FREE_PLAY_SLAYER_DOMAIN_SUM)) {
            return true;
        }
    }

    return false;


#elif defined(__PAY_SYSTEM_FREE_LIMIT__)

    if (!pGamePlayer->isPayPlaying()) {
        int DomainSUM = getSkillDomainLevelSum();


        if (DomainSUM <= g_pVariableManager->getVariable(FREE_PLAY_SLAYER_DOMAIN_SUM)) {
            return true;
        }

        return false;
    }

    return true;

#else

    return pGamePlayer->isPayPlaying();

#endif


    __END_CATCH
}

QuestGrade_t Slayer::getQuestGrade() const {
    return getTotalAttr(ATTR_BASIC) - getSkillDomainLevel(SKILL_DOMAIN_HEAL) * 1.5 -
           getSkillDomainLevel(SKILL_DOMAIN_ENCHANT) * 1.5;
}


bool Slayer::isNovice() const {
    return (m_STR[ATTR_BASIC] + m_DEX[ATTR_BASIC] + m_INT[ATTR_BASIC]) <= 40;
}

void Slayer::divideAttrExp(AttrKind kind, Damage_t damage, ModifyInfo& modifyInfo) {
    SLAYER_RECORD prev;
    getSlayerRecord(prev);

    damage = (Damage_t)getPercentValue(damage, AttrExpTimebandFactor[getZoneTimeband(m_pZone)]);

    if (g_pVariableManager->getExpRatio() > 100 && g_pVariableManager->getEventActivate() == 1)
        damage = getPercentValue(damage, g_pVariableManager->getExpRatio());


    if (isAffectExp2X())
        damage <<= 1;

    if (isFlag(Effect::EFFECT_CLASS_BONUS_EXP))
        damage *= 2;

    Exp_t MainPoint = max(1, damage * 8 / 10);
    Exp_t SubPoint = max(1, damage / 10);

#ifdef __CHINA_SERVER__


    float userExpConst = 1.0;

    SkillLevel_t HighSkillLevel = getHighestSkillDomainLevel();

    if (HighSkillLevel < 50)
        userExpConst = 1.0;
    else if (HighSkillLevel > 49 && HighSkillLevel < 75)
        userExpConst = 0.8;
    else if (HighSkillLevel > 74 && HighSkillLevel < 100)
        userExpConst = 0.6;
    else if (HighSkillLevel > 99)
        userExpConst = 0.4;

    MainPoint = (Exp_t)(MainPoint * userExpConst);
    SubPoint = (Exp_t)(SubPoint * userExpConst);

#endif


    SkillLevel_t MaxDomainLevel = getHighestSkillDomainLevel();
    Attr_t TotalAttr = getTotalAttr(ATTR_BASIC);
    Attr_t TotalAttrBound = 0;
    Attr_t AttrBound = 0;
    Attr_t OneAttrExpBound = 0;
    Attr_t SubAttrMax = 0;

    if (MaxDomainLevel <= SLAYER_BOUND_LEVEL && TotalAttr <= SLAYER_BOUND_ATTR_SUM) {
        TotalAttrBound = SLAYER_BOUND_ATTR_SUM;      // 300
        AttrBound = SLAYER_BOUND_ATTR;               // 200
        OneAttrExpBound = SLAYER_BOUND_ONE_EXP_ATTR; // 200
        SubAttrMax = SLAYER_BOUND_SUB_ATTR;          // 50
    } else {
        TotalAttrBound = SLAYER_MAX_ATTR_SUM;  // 435
        AttrBound = SLAYER_MAX_ATTR;           // 295
        OneAttrExpBound = SLAYER_ONE_EXP_ATTR; // 400
        SubAttrMax = SLAYER_MAX_SUB_ATTR;      // 70
    }

    Attr* pMainAttr = m_pAttrs[kind];
    Attr* pSubAttrs[2];
    int count = 0;

    bool levelUpSubAttrs[2] = {false, false};

    for (int itr = ATTR_KIND_STR; itr != ATTR_KIND_MAX; ++itr) {
        if (m_pAttrs[itr] != pMainAttr)
            pSubAttrs[count++] = m_pAttrs[itr];
    }

    if (pSubAttrs[0]->getLevel() < pSubAttrs[1]->getLevel()) {
        Attr* pTemp;
        SWAP(pSubAttrs[0], pSubAttrs[1], pTemp);
    }

    bool downOtherLevel = TotalAttr >= TotalAttrBound;
    bool upOtherLevel = TotalAttr < OneAttrExpBound;
    bool canLevelUp = pMainAttr->getLevel() < AttrBound;
    bool levelUpMainAttr = pMainAttr->increaseExp(MainPoint, canLevelUp);

    if (levelUpMainAttr && downOtherLevel) {
        pSubAttrs[0]->levelDown();
        levelUpSubAttrs[0] = true;
    }

    if (upOtherLevel) {
        levelUpSubAttrs[0] = pSubAttrs[0]->increaseExp(SubPoint) || levelUpSubAttrs[0];
        levelUpSubAttrs[1] = pSubAttrs[1]->increaseExp(SubPoint);
    } else {
        if (pSubAttrs[0]->getLevel() < SubAttrMax)
            levelUpSubAttrs[0] = pSubAttrs[0]->increaseExp(SubPoint) || levelUpSubAttrs[0];
        if (pSubAttrs[1]->getLevel() < SubAttrMax)
            levelUpSubAttrs[1] = pSubAttrs[1]->increaseExp(SubPoint) || levelUpSubAttrs[1];
    }

    if (++m_AttrExpSaveCount > ATTR_EXP_SAVE_PERIOD) {
        char pField[256];
        sprintf(pField, "STRGoalExp=%ld, DEXGoalExp=%ld, INTGoalExp=%ld", getSTRGoalExp(), getDEXGoalExp(),
                getINTGoalExp());

        tinysave(pField);

        m_AttrExpSaveCount = 0;
    }

    if (levelUpMainAttr || levelUpSubAttrs[0] || levelUpSubAttrs[1]) {
        healCreatureForLevelUp(this, modifyInfo, &prev);
        sendEffectLevelUp(this);
        if (g_pVariableManager->isNewbieTransportToGuild())
            checkNewbieTransportToGuild(this);

        char pField[256];
        sprintf(pField, "STR=%d, DEX=%d, INTE=%d, STRGoalExp=%ld, DEXGoalExp=%ld, INTGoalExp=%ld",
                //							getSTR(ATTR_BASIC), getDEX(ATTR_BASIC), getINT(ATTR_BASIC), getSTRGoalExp(),
                // getDEXGoalExp(), getINTGoalExp();
                m_pAttrs[ATTR_KIND_STR]->getLevel(), m_pAttrs[ATTR_KIND_DEX]->getLevel(),
                m_pAttrs[ATTR_KIND_INT]->getLevel(), getSTRGoalExp(), getDEXGoalExp(), getINTGoalExp());

        tinysave(pField);
    }

    modifyInfo.addLongData(MODIFY_STR_EXP, getSTRGoalExp());
    modifyInfo.addLongData(MODIFY_DEX_EXP, getDEXGoalExp());
    modifyInfo.addLongData(MODIFY_INT_EXP, getINTGoalExp());
}

void Slayer::setLastTarget(ObjectID_t value) {
    if (getPet() != NULL && value != getLastTarget()) {
        GCOtherModifyInfo gcOMI;
        gcOMI.setObjectID(getObjectID());
        gcOMI.addLongData(MODIFY_LAST_TARGET, value);

        m_pZone->broadcastPacket(getX(), getY(), &gcOMI, this);

        GCModifyInformation gcMI;
        gcMI.addLongData(MODIFY_LAST_TARGET, value);

        getPlayer()->sendPacket(&gcMI);
    }

    Creature::setLastTarget(value);
}

void Slayer::initPetQuestTarget() {
    QuestGrade_t grade = getQuestGrade();

    int minClass = 1, maxClass = 1;

    if (grade <= 60) {
        minClass = maxClass = 2;
    } else if (grade <= 95) {
        minClass = maxClass = 3;
    } else if (grade <= 130) {
        minClass = 4;
        maxClass = 5;
    } else if (grade <= 170) {
        minClass = maxClass = 6;
    } else if (grade <= 210) {
        minClass = maxClass = 7;
    } else if (grade <= 240) {
        minClass = 7;
        maxClass = 8;
    } else if (grade <= 270) {
        minClass = 8;
        maxClass = 9;
    } else if (grade <= 290) {
        minClass = 9;
        maxClass = 10;
    } else if (grade <= 300) {
        minClass = maxClass = 10;
    } else if (grade <= 320) {
        minClass = 10;
        maxClass = 11;
    } else if (grade <= 360) {
        minClass = 10;
        maxClass = 11;
    } else {
        minClass = 11;
        maxClass = 12;
    }

    m_TargetMonster = g_pMonsterInfoManager->getRandomMonsterByClass(minClass, maxClass);
    m_TargetNum = 80;
    m_TimeLimit = 3600;
}
