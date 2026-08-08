//////////////////////////////////////////////////////////////////////////////
// Filename    : Vampire.cpp
// Written By  : Elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Vampire.h"

#include "AbilityBalance.h"
#include "CreatureUtil.h"
#include "DB.h"
#include "EffectLoaderManager.h"
#include "FlagSet.h"
#include "GamePlayer.h"
#include "ItemInfoManager.h"
#include "ItemLoaderManager.h"
#include "ItemUtil.h"
#include "OptionInfo.h"
#include "PacketUtil.h"
#include "Party.h"
#include "Player.h"
#include "PreparedStatement.h"
#include "Shape.h"
#include "SkillInfo.h"
#include "SkillParentInfo.h"
#include "SkillUtil.h"
#include "Stash.h"
#include "TradeManager.h"
#include "VampEXPInfo.h"
// #include "RankEXPInfo.h"
#include <stdio.h>

#include "AdvancementClassExpTable.h"
#include "CastleSkillInfo.h"
#include "DynamicZone.h"
#include "EffectGrandMasterVampire.h"
#include "GCChangeShape.h"
#include "GCModifyInformation.h"
#include "GCOtherModifyInfo.h"
#include "GCPetStashList.h"
#include "GCRealWearingInfo.h"
#include "GCSkillInfo.h"
#include "GCStatusCurrentHP.h"
#include "GCTakeOff.h"
#include "GuildUnion.h"
#include "MonsterInfo.h"
#include "PKZoneInfoManager.h"
#include "PetInfo.h"
#include "RaceWarLimiter.h"
#include "RankExpTable.h"
#include "ResurrectLocationManager.h"
#include "Store.h"
#include "SystemAvailabilitiesManager.h"
#include "TimeLimitItemManager.h"
#include "VariableManager.h"
#include "WarSystem.h"
#include "item/AR.h"
#include "item/Belt.h"
#include "item/PetItem.h"
#include "item/SG.h"
#include "item/SMG.h"
#include "item/SR.h"
#include "item/Skull.h"
#include "skill/EffectBless.h"
#include "skill/EffectDoom.h"
#include "skill/EffectParalyze.h"
#include "skill/EffectTransformToBat.h"
#include "skill/EffectTransformToWolf.h"
#include "skill/VampireCastleSkillSlot.h"

const Color_t UNIQUE_COLOR = 0xFFFF;
const Color_t QUEST_COLOR = 0xFFFE;

const Level_t MAX_VAMPIRE_LEVEL = 150;
const Level_t MAX_VAMPIRE_LEVEL_OLD = 100;


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


Vampire::Vampire()

    : PlayerCreature(0, NULL) {
    __BEGIN_TRY

    m_CClass = CREATURE_CLASS_VAMPIRE;

    m_Mutex.setName("Vampire");

    
    for (int i = 0; i < SKILL_DOUBLE_IMPACT; i++) {
        VampireSkillSlot* pVampireSkillSlot = new VampireSkillSlot;
        // pVampireSkillSlot = new VampireSkillSlot;	// 2002.1.16 by sigi
        pVampireSkillSlot->setName(m_Name);
        pVampireSkillSlot->setSkillType(i);
        pVampireSkillSlot->setInterval(5);
        pVampireSkillSlot->setRunTime();

        addSkill(pVampireSkillSlot);
    }

    for (int i = 0; i < VAMPIRE_WEAR_MAX; i++)
        m_pWearItem[i] = NULL;

    
    //	for (int i = 0; i < 8; i++)
    //	{
    //		m_HotKey[i] = 0;
    //	}

    m_SilverDamage = 0;
    m_ClanType = 0;

    
    getCurrentTime(m_HPRegenTime);

    
    //	m_RankExpSaveCount       = 0;
    m_ExpSaveCount = 0;
    m_FameSaveCount = 0;
    m_AlignmentSaveCount = 0;

    __END_CATCH
}

Vampire::~Vampire()

{
    __BEGIN_TRY

    try {
        
        DWORD flag;
        Color_t color[PCVampireInfo::VAMPIRE_COLOR_MAX];
        getShapeInfo(flag, color);

        char pField[128];
        sprintf(pField, "Shape=%ld, CoatColor=%d", flag, color[PCVampireInfo::VAMPIRE_COLOR_COAT]);

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

        
        unordered_map<SkillType_t, VampireSkillSlot*>::iterator itr = m_SkillSlot.begin();
        for (; itr != m_SkillSlot.end(); itr++) {
            VampireSkillSlot* pVampireSkillSlot = itr->second;
            SAFE_DELETE(pVampireSkillSlot);
        }
    } catch (Throwable& t) {
        filelog("vampireDestructor.txt", "%s", t.toString().c_str());
    } catch (exception& e) {
        filelog("vampireDestructor.txt", "Unknown std::exception");
    } catch (...) {
        filelog("vampireDestructor.txt", "Unknown ... exception");
    }

    m_bDeriveDestructed = true;

    __END_CATCH_NO_RETHROW
}

// registerObject


void Vampire::registerObject()

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

    
    for (int i = 0; i < VAMPIRE_WEAR_MAX; i++) {
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

    m_Garbage.registerObject(OR);

    for (int i = 0; i < MAX_PET_STASH; ++i) {
        Item* pItem = getPetStashItem(i);
        if (pItem != NULL)
            registerItem(pItem, OR);
    }

    __LEAVE_CRITICAL_SECTION(OR)

    m_VampireInfo.setObjectID(m_ObjectID);
    m_pStore->updateStoreInfo();

    __END_CATCH
}



void Vampire::registerInitObject()

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

    
    for (int i = 0; i < VAMPIRE_WEAR_MAX; i++) {
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

    m_Garbage.registerObject(OR);

    __LEAVE_CRITICAL_SECTION(OR)

    m_VampireInfo.setObjectID(m_ObjectID);

    __END_CATCH
}



void Vampire::checkItemTimeLimit() {
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
        for (int i = 0; i < VAMPIRE_WEAR_MAX; i++) {
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

    __END_CATCH
}

void Vampire::updateEventItemTime(DWORD time) {
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
        for (int i = 0; i < VAMPIRE_WEAR_MAX; i++) {
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

///////////////////////////////////////////


//
void Vampire::loadItem(bool checkTimeLimit)

{
    __BEGIN_TRY

    PlayerCreature::loadItem();

    
    SAFE_DELETE(m_pInventory);
    m_pInventory = new Inventory(10, 6);
    m_pInventory->setOwner(getName());

    
    g_pItemLoaderManager->load(this);

    
    PlayerCreature::loadGoods();

    
    registerInitObject();

    if (checkTimeLimit) {
        checkItemTimeLimit();
    }

    
    initAllStat();

    __END_CATCH
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Vampire::load()

{
    __BEGIN_TRY

    if (!PlayerCreature::load())
        return false;

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    int reward = 0;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // add by Sonic 2006.10.28
        PreparedStatement loadVampireStmt(pConn,
                                           "SELECT Name, AdvancementClass, AdvancementGoalExp, Sex, "
                                           "MasterEffectColor, "
                                           "BatColor, SkinColor,"
                                           "STR, DEX, INTE, HP, CurrentHP, Fame,"
                                           "GoalExp, Level, Bonus, Gold, GuildID,"
                                           "ZoneID, XCoord, YCoord, Sight, Alignment,"
                                           "StashGold, StashNum, Competence, CompetenceShape, ResurrectZone, "
                                           "SilverDamage, Reward, SMSCharge,"
                                           "`Rank`, RankGoalExp FROM Vampire WHERE Name = ? AND Active = 'ACTIVE'");
        loadVampireStmt.bindString(1, m_Name);
        pResult = loadVampireStmt.execute();
        // end by Sonic

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

        setSex(pResult->getString(++i));
        // edit by sonic 2006.10.28
        setMasterEffectColor(pResult->getInt(++i));
        // end by sonic
        setBatColor(pResult->getInt(++i));
        setSkinColor(pResult->getInt(++i));

        m_STR[ATTR_BASIC] = pResult->getInt(++i);
        m_STR[ATTR_CURRENT] = m_STR[ATTR_BASIC];
        m_STR[ATTR_MAX] = m_STR[ATTR_BASIC];

        m_DEX[ATTR_BASIC] = pResult->getInt(++i);
        m_DEX[ATTR_CURRENT] = m_DEX[ATTR_BASIC];
        m_DEX[ATTR_MAX] = m_DEX[ATTR_BASIC];

        m_INT[ATTR_BASIC] = pResult->getInt(++i);
        m_INT[ATTR_CURRENT] = m_INT[ATTR_BASIC];
        m_INT[ATTR_MAX] = m_INT[ATTR_BASIC];

        setHP(pResult->getInt(++i), ATTR_MAX);
        setHP(getHP(ATTR_MAX), ATTR_BASIC);
        setHP(pResult->getInt(++i), ATTR_CURRENT);

        setFame(pResult->getInt(++i));

        //		setExp(pResult->getInt(++i));
        setGoalExp(pResult->getInt(++i));
        //		setExpOffset(pResult->getInt(++i));
        setLevel(pResult->getInt(++i));
        setBonus(pResult->getInt(++i));

        // setInMagics(pResult->getString(++i));
        setGold(pResult->getInt(++i));
        setGuildID(pResult->getInt(++i));

        //		setZoneID(pResult->getInt(++i));
        ZoneID_t zoneID = pResult->getInt(++i);
        setX(pResult->getInt(++i));
        setY(pResult->getInt(++i));

        setSight(pResult->getInt(++i));

        setAlignment(pResult->getInt(++i));

        //		for (int j = 0; j < 8; j++)
        //			setHotKey(j, pResult->getInt(++i));

        setStashGold(pResult->getInt(++i));
        setStashNum(pResult->getBYTE(++i));

        m_Competence = pResult->getBYTE(++i);

        if (m_Competence >= 4)
            m_Competence = 3;

        m_CompetenceShape = pResult->getBYTE(++i);

        setResurrectZoneID(pResult->getInt(++i));

        m_SilverDamage = pResult->getInt(++i);

        reward = pResult->getInt(++i);
        setSMSCharge(pResult->getInt(++i));

        Rank_t CurRank = pResult->getInt(++i);
        RankExp_t RankGoalExp = pResult->getInt(++i);

        m_pRank = new Rank(CurRank, RankGoalExp, RankExpTable::s_RankExpTables[RANK_TYPE_VAMPIRE]);

        //		setRank( pResult->getInt(++i) );
        //		setRankExp( pResult->getInt(++i) );
        //		setRankGoalExp( pResult->getInt(++i) );

        
        // 2002.7.15 by sigi
        
        int maxHP = m_STR[ATTR_CURRENT] * 2 + m_INT[ATTR_CURRENT] + m_DEX[ATTR_CURRENT] + m_Level;
        maxHP = min((int)maxHP, VAMPIRE_MAX_HP);
        setHP(maxHP, ATTR_MAX);

        try {
            setZoneID(zoneID);
        } catch (Error& e) {
            
            
            
            ZONE_COORD ResurrectCoord;
            g_pResurrectLocationManager->getVampirePosition(1003, ResurrectCoord);
            setZoneID(ResurrectCoord.id);
            setX(ResurrectCoord.x);
            setY(ResurrectCoord.y);
        }
    }
    END_DB(pStmt)

    /*	if (reward != 0)
        {
            BEGIN_DB
            {
                StringStream SQL;
                SQL << "UPDATE Vampire SET Reward = 0 WHERE Name = '" << m_Name << "'";

                pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
                pStmt->executeQuery(SQL.toString());
                SAFE_DELETE(pStmt);
            }
            END_DB(pStmt)

            int amount = 0;

            if (m_Level>=1 && m_Level<=10)
                amount = m_Level*2000;
            else if (m_Level>10 && m_Level<=30)
                amount = m_Level*3000;
            else if (m_Level>30)
                amount = m_Level*10000;

            int offset = 1000;

            for (int i=0; i<amount; i += offset)
            {
                increaseVampExp(offset);
            }

            reward = 0;
        }*/

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    
    m_VampireInfo.setObjectID(m_ObjectID);
    m_VampireInfo.setName(m_Name);
    m_VampireInfo.setSex(m_Sex);
    m_VampireInfo.setBatColor(m_BatColor);
    m_VampireInfo.setSkinColor(m_SkinColor);
    m_VampireInfo.setMasterEffectColor(m_MasterEffectColor);

    m_VampireInfo.setCompetence(m_CompetenceShape);

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectVampireSkillSaveStmt(
            pConn, "SELECT SkillType, Delay, CastingTime, NextTime FROM VampireSkillSave WHERE OwnerID = ?");
        selectVampireSkillSaveStmt.bindString(1, m_Name);
        pResult = selectVampireSkillSaveStmt.execute();

        while (pResult->next()) {
            int i = 0;
            SkillType_t SkillType = pResult->getInt(++i);

            if (hasSkill(SkillType) == NULL) {
                VampireSkillSlot* pVampireSkillSlot = new VampireSkillSlot();

                pVampireSkillSlot->setName(m_Name);
                pVampireSkillSlot->setSkillType(SkillType);
                pVampireSkillSlot->setInterval(pResult->getInt(++i));
                pVampireSkillSlot->setCastingTime(pResult->getInt(++i));
                // pVampireSkillSlot->setRunTime (pResult->getInt(++i));
                pVampireSkillSlot->setRunTime();

                addSkill(pVampireSkillSlot);
            }
        }
    }
    END_DB(pStmt)

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    loadRankBonus();

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    g_pEffectLoaderManager->load(this);

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    // by sigi. 2002.11.8
    if (m_Level >= 100 && SystemAvailabilitiesManager::getInstance()->isAvailable(
                              SystemAvailabilitiesManager::SYSTEM_GRAND_MASTER_EFFECT)) {
        if (!isFlag(Effect::EFFECT_CLASS_GRAND_MASTER_VAMPIRE)) {
            EffectGrandMasterVampire* pEffect = new EffectGrandMasterVampire(this);
            pEffect->setDeadline(999999);
            getEffectManager()->addEffect(pEffect);
            setFlag(Effect::EFFECT_CLASS_GRAND_MASTER_VAMPIRE);
        }
    }

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    m_pFlagSet->load(getName());

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    /*
    ItemType_t coatType = 0;
    Item* pItem = m_pWearItem[WEAR_BODY];
    if (pItem!=NULL)
    {
        coatType = pItem->getItemType();
    }
    */

    m_VampireInfo.setCoatType(0);
    m_VampireInfo.setCoatColor(JACKET_BASIC);
    m_VampireInfo.setCoatColor(377);
    m_VampireInfo.setAdvancementLevel(getAdvancementClassLevel());
    // m_VampireInfo.setCoatColor(2 , SUB_COLOR);


    
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

//----------------------------------------------------------------------
//----------------------------------------------------------------------
void Vampire::save() const

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    Statement* pStmt = NULL;

    //--------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------
    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        // Was built via StringStream + executeQueryString, splicing m_Name
        // straight into the WHERE clause with no escaping. Migrated to bound
        // parameters. Only CurrentHP/HP/SilverDamage/ZoneID/XCoord/YCoord/Name
        // were ever live here; BatColor, SkinColor, STR, DEX, INTE, Fame, Exp,
        // ExpOffset, Rank, RankExp, Level, Bonus, Gold, Sight, F5-F12, and
        // InMagics were already commented out / never wired up in the old
        // StringStream chain (dead, preserved as history in this comment).
        PreparedStatement saveVampireStmt(pConn,
                                           "UPDATE Vampire SET CurrentHP=?, HP=?, SilverDamage=?, ZoneID=?, "
                                           "XCoord=?, YCoord=? WHERE Name=?");
        saveVampireStmt.bindInt(1, (int)m_HP[ATTR_CURRENT]);
        saveVampireStmt.bindInt(2, (int)m_HP[ATTR_MAX]);
        saveVampireStmt.bindInt(3, (int)m_SilverDamage);
        saveVampireStmt.bindInt(4, (int)getZoneID());
        saveVampireStmt.bindInt(5, (int)m_X);
        saveVampireStmt.bindInt(6, (int)m_Y);
        saveVampireStmt.bindString(7, m_Name);
        saveVampireStmt.execute();

        // Assert(saveVampireStmt.getAffectedRowCount() != 1);
    }
    END_DB(pStmt)

     


    //--------------------------------------------------
    
    //--------------------------------------------------
    m_pEffectManager->save(m_Name);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//----------------------------------------------------------------------
// tinysave
//----------------------------------------------------------------------
void Vampire::tinysave(const string& field) // by sigi. 2002.5.15
    const {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        // field is a caller-built "Column=value" SQL fragment, not a single
        // bindable value; PreparedStatement cannot parameterise an entire
        // dynamic assignment list. Left spliced, matching the Guild::tinysave
        // precedent (batch 7). Only Name is bound.
        PreparedStatement tinysaveVampireStmt(pConn, "UPDATE Vampire SET " + field + " WHERE Name=?");
        tinysaveVampireStmt.bindString(1, m_Name);
        tinysaveVampireStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
//

//
//
////////////////////////////////////////////////////////////////////////////////


VampireSkillSlot* Vampire::getSkill(SkillType_t SkillType) const

{
    __BEGIN_TRY

    unordered_map<SkillType_t, VampireSkillSlot*>::const_iterator itr = m_SkillSlot.find(SkillType);
    if (itr != m_SkillSlot.end()) {
        return itr->second;
    }

    return NULL;

    __END_CATCH
}


void Vampire::addSkill(SkillType_t SkillType)

{
    __BEGIN_TRY

    switch (SkillType) {
    case SKILL_UN_BURROW:
    case SKILL_UN_TRANSFORM:
    case SKILL_UN_INVISIBILITY:
    case SKILL_THROW_HOLY_WATER:
    case SKILL_EAT_CORPSE:
        // case SKILL_HOWL:
        filelog("VampireError.log", "SkillType[%d], %s", SkillType, toString().c_str());
        Assert(false);
        break;
    default:
        break;
    }

    unordered_map<SkillType_t, VampireSkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);

    if (itr == m_SkillSlot.end()) {
        SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
        Turn_t Delay = pSkillInfo->getMaxDelay();

        VampireSkillSlot* pVampireSkillSlot = new VampireSkillSlot;

        pVampireSkillSlot->setName(m_Name);
        pVampireSkillSlot->setSkillType(SkillType);
        pVampireSkillSlot->setInterval(Delay);
        pVampireSkillSlot->setRunTime();
        pVampireSkillSlot->create(m_Name);

        m_SkillSlot[SkillType] = pVampireSkillSlot;
    }

    __END_CATCH
}


void Vampire::addSkill(VampireSkillSlot* pVampireSkillSlot)

{
    __BEGIN_TRY

    SkillType_t SkillType = pVampireSkillSlot->getSkillType();
    switch (SkillType) {
    case SKILL_UN_BURROW:
    case SKILL_UN_TRANSFORM:
    case SKILL_UN_INVISIBILITY:
    case SKILL_THROW_HOLY_WATER:
    case SKILL_EAT_CORPSE:
        //		case SKILL_HOWL:
        filelog("VampireError.log", "SkillType[%d], %s", SkillType, toString().c_str());
        Assert(false);
        break;
    default:
        break;
    }

    unordered_map<SkillType_t, VampireSkillSlot*>::iterator itr = m_SkillSlot.find(pVampireSkillSlot->getSkillType());

    if (itr == m_SkillSlot.end()) {
        m_SkillSlot[pVampireSkillSlot->getSkillType()] = pVampireSkillSlot;
    }
    // 2002.1.16 by sigi
    else {
        delete pVampireSkillSlot;
    }

    __END_CATCH
}


void Vampire::removeCastleSkill(SkillType_t SkillType)

{
    __BEGIN_TRY

    
    if (g_pCastleSkillInfoManager->getZoneID(SkillType) == 0)
        return;

    unordered_map<SkillType_t, VampireSkillSlot*>::iterator itr = m_SkillSlot.find(SkillType);

    if (itr != m_SkillSlot.end()) {
        VampireCastleSkillSlot* pCastleSkillSlot = dynamic_cast<VampireCastleSkillSlot*>(itr->second);

        SAFE_DELETE(pCastleSkillSlot);

        m_SkillSlot.erase(itr);
    }

    __END_CATCH
}


void Vampire::removeAllCastleSkill()

{
    __BEGIN_TRY

    unordered_map<SkillType_t, VampireSkillSlot*>::iterator itr = m_SkillSlot.begin();

    while (itr != m_SkillSlot.end()) {
        if (itr->second != NULL) {
            VampireSkillSlot* pSkillSlot = itr->second;
            if (g_pCastleSkillInfoManager->getZoneID(pSkillSlot->getSkillType()) == 0) {
                
                ++itr;
                continue;
            }

            
            SAFE_DELETE(pSkillSlot);
            unordered_map<SkillType_t, VampireSkillSlot*>::iterator prevItr = itr;

            ++itr;
            m_SkillSlot.erase(prevItr);
        } else {
            
            Assert(false);
        }
    }

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
//
//

//
//
////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
//
// Vampire::WearItem()
//

//
//----------------------------------------------------------------------
void Vampire::wearItem(WearPart Part, Item* pItem)

{
    __BEGIN_TRY

    Assert(pItem != NULL);

    Item* pPrevItem = NULL;
    Item* pLeft = NULL;
    Item* pRight = NULL;

    
    
    

    
    
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
        if (isWear(Part)) {
            pPrevItem = getWearItem(Part);
            m_pWearItem[Part] = pItem;

            // by sigi. 2002.5.15
            char pField[128];
            // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
            sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
            pItem->tinysave(pField);

            addItemToExtraInventorySlot(pPrevItem);
            // pPrevItem->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
            sprintf(pField, "Storage=%d, StorageID=0", STORAGE_EXTRASLOT);
            pPrevItem->tinysave(pField);
        } else {
            
            m_pWearItem[Part] = pItem;

            // by sigi. 2002.5.15
            char pField[128];
            // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
            sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
            pItem->tinysave(pField);
        }
    }

    
    
    
    if (pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_COAT) {
        m_VampireInfo.setCoatColor(getItemShapeColor(pItem));

        
        m_VampireInfo.setCoatType(pItem->getItemType());
    }

    __END_CATCH
}


//----------------------------------------------------------------------
// Vampire::WearItem()

//----------------------------------------------------------------------
void Vampire::wearItem(WearPart Part)

{
    __BEGIN_TRY

    
    Item* pItem = getExtraInventorySlotItem();
    Assert(pItem != NULL);

    Item* pPrevItem = NULL;
    Item* pLeft = NULL;
    Item* pRight = NULL;

    
    
    VAMPIRE_RECORD prev;
    getVampireRecord(prev);

    
    
    
    char pField[128];

    
    
    if (isTwohandWeapon(pItem)) {
        
        if (isWear(WEAR_RIGHTHAND) && isWear(WEAR_LEFTHAND)) {
            pLeft = getWearItem(WEAR_RIGHTHAND);
            pRight = getWearItem(WEAR_LEFTHAND);

            
            if (pLeft == pRight) {
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
            deleteItemFromExtraInventorySlot();

            // by sigi. 2002.5.15
            // pItem->save(m_Name, STORAGE_GEAR, 0, Part, 0);
            sprintf(pField, "Storage=%d, StorageID=0, X=%d", STORAGE_GEAR, Part);
            pItem->tinysave(pField);
        }
    }

    initAllStat();
    sendRealWearingInfo();
    sendModifyInfo(prev);

    
    
    

    
    if (m_pRealWearingCheck[Part]) {
        if (pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_COAT) {
            Color_t color = getItemShapeColor(pItem);
            m_VampireInfo.setCoatColor(color);
            m_VampireInfo.setCoatType(pItem->getItemType());

            
            GCChangeShape pkt;
            pkt.setObjectID(getObjectID());
            pkt.setItemClass(Item::ITEM_CLASS_VAMPIRE_COAT);
            pkt.setItemType(pItem->getItemType());
            pkt.setOptionType(pItem->getFirstOptionType());
            pkt.setAttackSpeed(m_AttackSpeed[ATTR_CURRENT]);

            if (color == QUEST_COLOR)
                pkt.setFlag(SHAPE_FLAG_QUEST);

            Zone* pZone = getZone();
            pZone->broadcastPacket(m_X, m_Y, &pkt, this);
        }
    }

    if (m_pZone != NULL) {
        GCOtherModifyInfo gcOtherModifyInfo;
        makeGCOtherModifyInfo(&gcOtherModifyInfo, this, &prev);

        if (gcOtherModifyInfo.getShortCount() != 0 || gcOtherModifyInfo.getLongCount() != 0) {
            m_pZone->broadcastPacket(m_X, m_Y, &gcOtherModifyInfo, this);
        }
    }

    __END_CATCH
}


//----------------------------------------------------------------------
//
// Vampire::takeOffItem()
//
//----------------------------------------------------------------------
void Vampire::takeOffItem(WearPart Part, bool bAddOnMouse, bool bSendModifyInfo)

{
    __BEGIN_TRY

    VAMPIRE_RECORD prev;

    
    Item* pItem = m_pWearItem[Part];
    Assert(pItem != NULL);

    // m_pWearItem[Part] = NULL;

    
    
    // Item::ItemClass IClass = pItem->getItemClass();

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
        getVampireRecord(prev);
        initAllStat();
        sendRealWearingInfo();
        sendModifyInfo(prev);
    } else {
        initAllStat();
    }

    //---------------------------------------------
    
    
    //---------------------------------------------
    if (bAddOnMouse) {
        addItemToExtraInventorySlot(pItem);
        // pItem->save(m_Name, STORAGE_EXTRASLOT, 0, 0, 0);
        
        char pField[128];
        sprintf(pField, "Storage=%d, StorageID=0, Durability=%d", STORAGE_EXTRASLOT, pItem->getDurability());
        pItem->tinysave(pField);
    }

    if (pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_COAT) {
        m_VampireInfo.setCoatColor(377);
        m_VampireInfo.setCoatType(0);

        GCTakeOff pkt;
        pkt.setObjectID(getObjectID());
        pkt.setSlotID((SlotID_t)ADDON_COAT);
        m_pZone->broadcastPacket(getX(), getY(), &pkt, this);
    }

    if (m_pZone != NULL) {
        GCOtherModifyInfo gcOtherModifyInfo;
        makeGCOtherModifyInfo(&gcOtherModifyInfo, this, &prev);

        if (gcOtherModifyInfo.getShortCount() != 0 || gcOtherModifyInfo.getLongCount() != 0) {
            m_pZone->broadcastPacket(m_X, m_Y, &gcOtherModifyInfo, this);
        }
    }

    __END_CATCH
}


//----------------------------------------------------------------------
// destroyGears

//----------------------------------------------------------------------
void Vampire::destroyGears()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    for (int j = 0; j < VAMPIRE_WEAR_MAX; j++) {
        Item* pItem = m_pWearItem[j];
        if (pItem != NULL) {
            Item::ItemClass IClass = pItem->getItemClass();

            //-------------------------------------------------------------
            
            //-------------------------------------------------------------
            Assert(IClass != Item::ITEM_CLASS_AR);
            Assert(IClass != Item::ITEM_CLASS_SR);
            Assert(IClass != Item::ITEM_CLASS_SG);
            Assert(IClass != Item::ITEM_CLASS_SMG);
            Assert(IClass != Item::ITEM_CLASS_SWORD);
            Assert(IClass != Item::ITEM_CLASS_BLADE);
            Assert(IClass != Item::ITEM_CLASS_SHIELD);
            Assert(IClass != Item::ITEM_CLASS_CROSS);
            Assert(IClass != Item::ITEM_CLASS_MACE);
            Assert(IClass != Item::ITEM_CLASS_HELM);
            Assert(IClass != Item::ITEM_CLASS_GLOVE);
            Assert(IClass != Item::ITEM_CLASS_TROUSER);
            Assert(IClass != Item::ITEM_CLASS_COAT);

            
            
            if (isTwohandWeapon(pItem)) {
                m_pWearItem[WEAR_RIGHTHAND] = NULL;
                m_pWearItem[WEAR_LEFTHAND] = NULL;
            } else
                m_pWearItem[j] = NULL;

            SAFE_DELETE(pItem);
        }
    }

    __END_DEBUG
    __END_CATCH
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Vampire::isRealWearing(WearPart part) const

{
    __BEGIN_TRY

    if (part >= VAMPIRE_WEAR_MAX)
        throw("Vampire::isRealWearing() : invalid wear point!");

    if (m_pWearItem[part] == NULL)
        return false;
    if (part >= WEAR_ZAP1 && part <= WEAR_ZAP4) {
        
        if (m_pWearItem[part - WEAR_ZAP1 + WEAR_FINGER1] == NULL)
            return false;
    }

    return isRealWearing(m_pWearItem[part]);

    __END_CATCH
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Vampire::isRealWearing(Item* pItem) const

{
    __BEGIN_TRY

    if (pItem == NULL)
        return false;

    /*	if ( m_pZone != NULL && m_pZone->isDynamicZone() && m_pZone->getDynamicZone()->getTemplateZoneID() == 4004 )
        {
            if ( !isVampireWeapon( pItem->getItemClass() ) ) return false;
        }*/

    ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());

    Level_t ReqAdvancedLevel = pItemInfo->getReqAdvancedLevel();
    if (ReqAdvancedLevel > 0 && (!isAdvanced() || getAdvancementClassLevel() < ReqAdvancedLevel))
        return false;

    if (pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_COAT ||
        pItem->getItemClass() == Item::ITEM_CLASS_VAMPIRE_WEAPON) {
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
            return false;
        }
    }

    if (isCoupleRing(pItem))
        return true;

    Item::ItemClass IClass = pItem->getItemClass();
    Level_t ReqLevel = pItemInfo->getReqLevel();
    Attr_t ReqGender = pItemInfo->getReqGender();

    
    
    // 2003.3.21 by Sequoia
    Level_t ReqLevelMax = ((ReqLevel > MAX_VAMPIRE_LEVEL_OLD) ? MAX_VAMPIRE_LEVEL : MAX_VAMPIRE_LEVEL_OLD);

    
    
    const list<OptionType_t>& optionTypes = pItem->getOptionTypeList();
    list<OptionType_t>::const_iterator itr;

    for (itr = optionTypes.begin(); itr != optionTypes.end(); itr++) {
        OptionInfo* pOptionInfo = g_pOptionInfoManager->getOptionInfo(*itr);
        ReqLevel += pOptionInfo->getReqLevel();
    }

    // 2003.1.6 by Sequoia, Bezz
    
    ReqLevel = min(ReqLevel, ReqLevelMax);

    
    
    if (ReqLevel > 0 || ReqGender != GENDER_BOTH) {
        if (ReqLevel > 0 && m_Level < ReqLevel)
            return false;
        if (m_Sex == MALE && ReqGender == GENDER_FEMALE)
            return false;
        if (m_Sex == FEMALE && ReqGender == GENDER_MALE)
            return false;
    }

    return true;

    __END_CATCH
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
bool Vampire::isRealWearingEx(WearPart part) const {
    if (part >= VAMPIRE_WEAR_MAX)
        return false;
    return m_pRealWearingCheck[part];
}

DWORD Vampire::sendRealWearingInfo(void) const

{
    __BEGIN_TRY

    DWORD info = 0;
    DWORD flag = 1;

    for (int i = 0; i < VAMPIRE_WEAR_MAX; i++) {
        if (isRealWearing((Vampire::WearPart)i))
            info |= flag;
        flag <<= 1;
    }

    GCRealWearingInfo pkt;
    pkt.setInfo(info);
    m_pPlayer->sendPacket(&pkt);

    return info;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
//
//

//
//
////////////////////////////////////////////////////////////////////////////////

PCVampireInfo2* Vampire::getVampireInfo2()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    PCVampireInfo2* pInfo = new PCVampireInfo2();

    pInfo->setObjectID(m_ObjectID);
    pInfo->setName(m_Name);
    pInfo->setLevel(m_Level);
    pInfo->setSex(m_Sex);
    pInfo->setBatColor(m_BatColor);
    pInfo->setSkinColor(m_SkinColor);
    pInfo->setMasterEffectColor(m_MasterEffectColor);

    
    pInfo->setAlignment(m_Alignment);

    
    pInfo->setSTR(m_STR[ATTR_CURRENT], ATTR_CURRENT);
    pInfo->setSTR(m_STR[ATTR_MAX], ATTR_MAX);
    pInfo->setSTR(m_STR[ATTR_BASIC], ATTR_BASIC);
    pInfo->setDEX(m_DEX[ATTR_CURRENT], ATTR_CURRENT);
    pInfo->setDEX(m_DEX[ATTR_MAX], ATTR_MAX);
    pInfo->setDEX(m_DEX[ATTR_BASIC], ATTR_BASIC);
    pInfo->setINT(m_INT[ATTR_CURRENT], ATTR_CURRENT);
    pInfo->setINT(m_INT[ATTR_MAX], ATTR_MAX);
    pInfo->setINT(m_INT[ATTR_BASIC], ATTR_BASIC);

    pInfo->setHP(m_HP[ATTR_CURRENT], m_HP[ATTR_MAX]);
    pInfo->setFame(m_Fame);
    pInfo->setExp(m_GoalExp);
    //	pInfo->setExp(m_Exp);
    pInfo->setGold(m_Gold);
    pInfo->setSight(m_Sight);
    pInfo->setBonus(m_Bonus);
    pInfo->setSilverDamage(m_SilverDamage);

    // by sigi. 2002.8.30
    pInfo->setRank(getRank());
    pInfo->setRankExp(getRankGoalExp());

    //	for (int i = 0; i < 8; i++)
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

    return pInfo;

    __END_DEBUG
    __END_CATCH
}


//----------------------------------------------------------------------
// Vampire Outlook Information
//----------------------------------------------------------------------
PCVampireInfo3 Vampire::getVampireInfo3() const

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    m_VampireInfo.setX(m_X);
    m_VampireInfo.setY(m_Y);
    m_VampireInfo.setDir(m_Dir);
    m_VampireInfo.setCurrentHP(m_HP[ATTR_CURRENT]);
    m_VampireInfo.setMaxHP(m_HP[ATTR_MAX]);
    m_VampireInfo.setAttackSpeed(m_AttackSpeed[ATTR_CURRENT]);
    m_VampireInfo.setAlignment(m_Alignment);
    m_VampireInfo.setGuildID(m_GuildID);

    // by sigi. 2002.9.10
    m_VampireInfo.setRank(getRank());

    /*
    Item* pItem = m_pWearItem[WEAR_BODY];
    if (pItem!=NULL)
    {
        m_VampireInfo.setCoatType( pItem->getItemType() );
    }
    else
    {
        m_VampireInfo.setCoatType( 0 );
    }
    */


    if (m_Flag.test(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF)) {
        m_VampireInfo.setShape(SHAPE_WOLF);
    } else if (m_Flag.test(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)) {
        m_VampireInfo.setShape(SHAPE_BAT);
    } else if (m_Flag.test(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF)) {
        m_VampireInfo.setShape(SHAPE_WERWOLF);
    } else {
        m_VampireInfo.setShape(SHAPE_NORMAL);
    }

    
    m_VampireInfo.setBatColor(m_BatColor);
    m_VampireInfo.setSkinColor(m_SkinColor);
    m_VampireInfo.setMasterEffectColor(m_MasterEffectColor);

    GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion(m_GuildID);
    if (pUnion == NULL)
        m_VampireInfo.setUnionID(0);
    else
        m_VampireInfo.setUnionID(pUnion->getUnionID());

    m_VampireInfo.setAdvancementLevel(getAdvancementClassLevel());

    return m_VampireInfo;

    __END_DEBUG
    __END_CATCH
}

//----------------------------------------------------------------------
//
// get Extra Info
//
//----------------------------------------------------------------------
ExtraInfo* Vampire::getExtraInfo() const

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    BYTE ItemCount = 0;

    ExtraInfo* pExtraInfo = new ExtraInfo();

    Item* pItem = m_pExtraInventorySlot->getItem();

    if (pItem != NULL) {
        //		Item::ItemClass IClass = pItem->getItemClass();

        ExtraSlotInfo* pExtraSlotInfo = new ExtraSlotInfo();
        pItem->makePCItemInfo(*pExtraSlotInfo);

         

        pExtraInfo->addListElement(pExtraSlotInfo);

        ItemCount++;
    }

    pExtraInfo->setListNum(ItemCount);

    return pExtraInfo;

    __END_DEBUG
    __END_CATCH
}

//----------------------------------------------------------------------
//
// get Gear Info
//
//----------------------------------------------------------------------
GearInfo* Vampire::getGearInfo() const

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    int ItemCount = 0;

    GearInfo* pGearInfo = new GearInfo();

    for (int i = 0; i < VAMPIRE_WEAR_MAX; i++) {
        Item* pItem = m_pWearItem[i];

        if (pItem != NULL) {
            // Item::ItemClass IClass = pItem->getItemClass();

            GearSlotInfo* pGearSlotInfo = new GearSlotInfo();
            pItem->makePCItemInfo(*pGearSlotInfo);

            /*			pGearSlotInfo->setObjectID(pItem->getObjectID());
                        pGearSlotInfo->setItemClass(pItem->getItemClass());
                        pGearSlotInfo->setItemType(pItem->getItemType());
                        pGearSlotInfo->setOptionType(pItem->getOptionTypeList());
                        pGearSlotInfo->setDurability(pItem->getDurability());
                        pGearSlotInfo->setSilver(pItem->getSilver());
                        pGearSlotInfo->setEnchantLevel(pItem->getEnchantLevel());*/

             

            pGearSlotInfo->setSlotID(i);

            
            //			pGearSlotInfo->setMainColor(0);

            pGearInfo->addListElement(pGearSlotInfo);

            ItemCount++;
        }
    }

    pGearInfo->setListNum(ItemCount);

    return pGearInfo;

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get Inventory Info
//////////////////////////////////////////////////////////////////////////////
InventoryInfo* Vampire::getInventoryInfo() const

{
    __BEGIN_TRY
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
    __END_CATCH
}

//----------------------------------------------------------------------
// getSkillInfo
//----------------------------------------------------------------------
void Vampire::sendVampireSkillInfo()

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    VampireSkillInfo* pVampireSkillInfo = new VampireSkillInfo();

    BYTE SkillCount = 0;

    
    Timeval currentTime;
    getCurrentTime(currentTime);

    unordered_map<SkillType_t, VampireSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
    for (; itr != m_SkillSlot.end(); itr++) {
        VampireSkillSlot* pVampireSkillSlot = itr->second;
        Assert(pVampireSkillSlot != NULL);

        
        if (pVampireSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT) {
            SubVampireSkillInfo* pSubVampireSkillInfo = new SubVampireSkillInfo();
            pSubVampireSkillInfo->setSkillType(pVampireSkillSlot->getSkillType());
            pSubVampireSkillInfo->setSkillTurn(pVampireSkillSlot->getInterval());
            
            // pSubVampireSkillInfo->setCastingTime(pVampireSkillSlot->getCastingTime());
            pSubVampireSkillInfo->setCastingTime(pVampireSkillSlot->getRemainTurn(currentTime));

            pVampireSkillInfo->addListElement(pSubVampireSkillInfo);

            SkillCount++;
        }
    }

    GCSkillInfo gcSkillInfo;
    gcSkillInfo.setPCType(PC_VAMPIRE);
    SkillType_t LearnSkillType = g_pSkillInfoManager->getSkillTypeByLevel(SKILL_DOMAIN_VAMPIRE, m_Level);

    
    if (LearnSkillType != 0) {
        
        if (hasSkill(LearnSkillType) == NULL) {
            pVampireSkillInfo->setLearnNewSkill(true);
        }
    }

    pVampireSkillInfo->setListNum(SkillCount);

    gcSkillInfo.addListElement(pVampireSkillInfo);

    m_pPlayer->sendPacket(&gcSkillInfo);

    __END_DEBUG
    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
//
//

//
//
////////////////////////////////////////////////////////////////////////////////
void Vampire::setGold(Gold_t gold)

{
    __BEGIN_TRY

    
    // 2003.1.8  by bezz.
    m_Gold = min((Gold_t)MAX_MONEY, gold);

    __END_CATCH
}

void Vampire::setGoldEx(Gold_t gold)

{
    __BEGIN_TRY

    setGold(gold);

    /*
    StringStream sql;
    sql << "Gold = " << (int)m_Gold;

    tinysave(sql.toString());
    */

    // by sigi. 2002.5.15
    char pField[128];
    sprintf(pField, "Gold=%ld", m_Gold);
    tinysave(pField);

    __END_CATCH
}

void Vampire::increaseGoldEx(Gold_t gold)

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
        PreparedStatement increaseGoldStmt(pConn, "UPDATE Vampire SET Gold=Gold+? WHERE NAME=?");
        increaseGoldStmt.bindUInt(1, gold);
        increaseGoldStmt.bindString(2, m_Name);
        increaseGoldStmt.execute();
    }
    END_DB(pStmt)


    __END_DEBUG
    __END_CATCH
}

void Vampire::decreaseGoldEx(Gold_t gold)

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
        PreparedStatement decreaseGoldStmt(pConn, "UPDATE Vampire SET Gold=Gold-? WHERE NAME=?");
        decreaseGoldStmt.bindUInt(1, gold);
        decreaseGoldStmt.bindString(2, m_Name);
        decreaseGoldStmt.execute();
    }
    END_DB(pStmt)

    __END_DEBUG
    __END_CATCH
}

bool Vampire::checkGoldIntegrity() {
    __BEGIN_TRY

    Statement* pStmt = NULL;
    bool ret = false;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectGoldStmt(pConn, "SELECT Gold FROM Vampire WHERE NAME=?");
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

bool Vampire::checkStashGoldIntegrity() {
    __BEGIN_TRY

    Statement* pStmt = NULL;
    bool ret = false;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement selectStashGoldStmt(pConn, "SELECT StashGold FROM Vampire WHERE NAME=?");
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


void Vampire::saveSilverDamage(Silver_t damage)

{
    __BEGIN_TRY

    setSilverDamage(damage);

    /*
    StringStream sql;
    sql << "SilverDamage = " << (int)m_SilverDamage;
    tinysave(sql.toString());
    */

    // by sigi. 2002.5.15
    char pField[128];
    sprintf(pField, "SilverDamage=%d", m_SilverDamage);
    tinysave(pField);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void Vampire::heartbeat(const Timeval& currentTime)

{
    __BEGIN_TRY
    __BEGIN_DEBUG

    /*#ifdef __ACTIVE_QUEST__
        // by sigi. 2002.12.3
        if (m_pQuestManager!=NULL)
        {
            m_pQuestManager->heartbeat();
        }
    #endif*/

    PlayerCreature::heartbeat(currentTime);

    
    if (m_HPRegenTime < currentTime) {
        Timeval diffTime = timediff(currentTime, m_HPRegenTime);

        if (diffTime.tv_sec > 0) {
            
            
            
            if (isAlive() && !isFlag(Effect::EFFECT_CLASS_COMA) &&
                (!isFlag(Effect::EFFECT_CLASS_MEPHISTO) || isFlag(Effect::EFFECT_CLASS_CASKET))) {
                // by sigi. 2002.6.19
                bool bInCasket = isFlag(Effect::EFFECT_CLASS_CASKET);

                HP_t CurHP = m_HP[ATTR_CURRENT];
                HP_t NewHP = 0;

                
                
                if (bInCasket && m_SilverDamage > 0) {
                    NewHP = (10 + m_HPRegenBonus) * diffTime.tv_sec;
                    if (isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE))
                        NewHP /= 2;

                    int remainSilver = (int)m_SilverDamage - (int)NewHP;

                    
                    if (remainSilver < 0) {
                        m_SilverDamage = 0;
                        NewHP = -remainSilver;

                        HP_t MaxHP = m_HP[ATTR_MAX];
                        m_HP[ATTR_CURRENT] = min((int)MaxHP, (int)(CurHP + NewHP));
                    }
                    
                    else {
                        m_SilverDamage = remainSilver;
                    }
                } else {
                    HP_t MaxHP = m_HP[ATTR_MAX] - getSilverDamage();

                    // Normal       : 2
                    // Burrow(Hide) : 4
                    // Casket       : 6
                    
                    // Bat          : 0
                    if (isFlag(Effect::EFFECT_CLASS_HIDE)) {
                        NewHP = (4 + m_HPRegenBonus) * diffTime.tv_sec;
                    } else if (isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT)) {
                        NewHP = 0;
                    }
                    // by sigi. 2002.6.19
                    else if (isFlag(Effect::EFFECT_CLASS_CASKET)) {
                        NewHP = (10 + m_HPRegenBonus) * diffTime.tv_sec;
                    } else {
                        NewHP = (2 + m_HPRegenBonus) * diffTime.tv_sec;
                    }

                    if (isFlag(Effect::EFFECT_CLASS_HAS_BLOOD_BIBLE))
                        NewHP /= 2;
                    m_HP[ATTR_CURRENT] = min((int)MaxHP, (int)(CurHP + NewHP));
                }
            }

            m_HPRegenTime.tv_sec = m_HPRegenTime.tv_sec + diffTime.tv_sec;
            m_HPRegenTime.tv_usec = m_HPRegenTime.tv_usec;
        }
    }

     

    __END_DEBUG
    __END_CATCH
}

void Vampire::getVampireRecord(VAMPIRE_RECORD& record) const

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

    record.pDamage[0] = m_Damage[0];
    record.pDamage[1] = m_Damage[1];

    record.Rank = getRank();

    record.Defense = m_Defense[0];
    record.ToHit = m_ToHit[0];
    record.Protection = m_Protection[0];
    record.AttackSpeed = m_AttackSpeed[0];

    __END_CATCH
}

void Vampire::setResurrectZoneIDEx(ZoneID_t id)

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

void Vampire::saveAlignment(Alignment_t alignment)

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


//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string Vampire::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "Vampire("
        //<< "ObjectID:"   << (int)getObjectID()
        << ",Name:" << m_Name << ",BatColor:" << (int)m_BatColor << ",SkinColor:" << (int)m_SkinColor
        << ",STR:" << (int)m_STR[ATTR_CURRENT] << "/" << (int)m_STR[ATTR_MAX] << ",DEX:" << (int)m_DEX[ATTR_CURRENT]
        << "/" << (int)m_DEX[ATTR_MAX] << ",INT:" << (int)m_INT[ATTR_CURRENT] << "/" << (int)m_INT[ATTR_MAX]
        << ",HP:" << (int)m_HP[ATTR_CURRENT] << "/" << (int)m_HP[ATTR_MAX] << ",Fame:"
        << (int)m_Fame
        //		<< ",Exp:"       << (int)m_Exp
        //		<< ",ExpOffset:" << (int)m_ExpOffset
        << ",Rank:" << (int)getRank() << ",RankGoalExp:" << (int)getRankGoalExp() << ",Level:" << (int)m_Level
        << ",Bonus:"
        << (int)m_Bonus
        //<< ",InMagics:'" << ??? << "'"
        << ",Gold:" << (int)m_Gold << ",ZoneID:" << (int)getZoneID() << ",XCoord:" << (int)m_X << ",YCoord:" << (int)m_Y
        << ",Sight:" << (int)m_Sight << ")";

    return msg.toString();

    __END_CATCH
}

void Vampire::saveSkills(void) const

{
    __BEGIN_TRY

    unordered_map<SkillType_t, VampireSkillSlot*>::const_iterator itr = m_SkillSlot.begin();
    for (; itr != m_SkillSlot.end(); itr++) {
        VampireSkillSlot* pVampireSkillSlot = itr->second;
        Assert(pVampireSkillSlot != NULL);

        
        if (pVampireSkillSlot->getSkillType() >= SKILL_DOUBLE_IMPACT) {
            pVampireSkillSlot->save(m_Name);
        }
    }

    __END_CATCH
}

IP_t Vampire::getIP(void) const {
    Assert(m_pPlayer != NULL);
    Socket* pSocket = m_pPlayer->getSocket();
    Assert(pSocket != NULL);
    return pSocket->getHostIP();
}

void Vampire::saveGears(void) const

{
    __BEGIN_TRY

    
    char pField[128];

    for (int i = 0; i < Vampire::VAMPIRE_WEAR_MAX; i++) {
        Item* pItem = m_pWearItem[i];
        if (pItem != NULL) {
            Durability_t maxDurability = computeMaxDurability(pItem);
            if (pItem->getDurability() < maxDurability) {
                // pItem->save(m_Name, STORAGE_GEAR, 0, i, 0);
                
                sprintf(pField, "Durability=%d", pItem->getDurability());
                pItem->tinysave(pField);
            }
        }
    }

    __END_CATCH
}


void Vampire::saveExps(void) const

{
    __BEGIN_TRY

    
    
    
    
    /*
    StringStream sql;
    sql << "UPDATE Vampire SET "
        << "Alignment = " << m_Alignment
        << ",Fame = " << m_Fame
        << ",Exp = " << m_Exp
        << ",GoalExp = " << m_GoalExp;

    if (m_SilverDamage != 0)
    {
        sql << ",SilverDamage = " << m_SilverDamage;
    }

    sql << " WHERE Name = '" << m_Name << "'";
    */

    Statement* pStmt = NULL;

    // The old code built the SilverDamage clause as a raw sprintf'd SQL
    // fragment (",SilverDamage = %d" or "") spliced into the middle of the
    // SET list via %s. PreparedStatement can't bind a fragment that changes
    // the statement's shape, so the conditional is hoisted to two static,
    // fully-parameterised query texts instead of splicing the value in.
    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        if (m_SilverDamage != 0) {
            PreparedStatement saveExpsVampireStmt(pConn,
                                                   "UPDATE Vampire SET Alignment=?, Fame=?, GoalExp=?, "
                                                   "SilverDamage=?, `Rank`=?, RankGoalExp=?, AdvancementClass=?, "
                                                   "AdvancementGoalExp=? WHERE Name=?");
            saveExpsVampireStmt.bindInt(1, m_Alignment);
            saveExpsVampireStmt.bindInt(2, m_Fame);
            saveExpsVampireStmt.bindULong(3, m_GoalExp);
            saveExpsVampireStmt.bindInt(4, m_SilverDamage);
            saveExpsVampireStmt.bindInt(5, getRank());
            saveExpsVampireStmt.bindULong(6, getRankGoalExp());
            saveExpsVampireStmt.bindUInt(7, getAdvancementClassLevel());
            saveExpsVampireStmt.bindInt(8, getAdvancementClassGoalExp());
            saveExpsVampireStmt.bindString(9, m_Name);
            saveExpsVampireStmt.execute();
        } else {
            PreparedStatement saveExpsVampireStmt(pConn,
                                                   "UPDATE Vampire SET Alignment=?, Fame=?, GoalExp=?, `Rank`=?, "
                                                   "RankGoalExp=?, AdvancementClass=?, AdvancementGoalExp=? "
                                                   "WHERE Name=?");
            saveExpsVampireStmt.bindInt(1, m_Alignment);
            saveExpsVampireStmt.bindInt(2, m_Fame);
            saveExpsVampireStmt.bindULong(3, m_GoalExp);
            saveExpsVampireStmt.bindInt(4, getRank());
            saveExpsVampireStmt.bindULong(5, getRankGoalExp());
            saveExpsVampireStmt.bindUInt(6, getAdvancementClassLevel());
            saveExpsVampireStmt.bindInt(7, getAdvancementClassGoalExp());
            saveExpsVampireStmt.bindString(8, m_Name);
            saveExpsVampireStmt.execute();
        }
    }
    END_DB(pStmt)

    __END_CATCH
}


//----------------------------------------------------------------------
// getShapeInfo
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//


//

//----------------------------------------------------------------------
void Vampire::getShapeInfo(DWORD& flag, Color_t colors[PCVampireInfo::VAMPIRE_COLOR_MAX]) const
//
{
    __BEGIN_DEBUG

    Item* pItem;
    // OptionInfo* 				pOptionInfo;
    int vampireBit;
    int vampireColor;
    WearPart Part;

    
    flag = 0;

    //-----------------------------------------------------------------
    
    //-----------------------------------------------------------------
    Part = WEAR_BODY;
    pItem = m_pWearItem[Part];
    vampireBit = 0;
    vampireColor = 0;

    if (pItem != NULL && m_pRealWearingCheck[Part]) {
        ItemType_t IType = pItem->getItemType();

        colors[vampireColor] = getItemShapeColor(pItem);

        // colors[vampireColor] = pItem->getOptionType();
        // flag |= (getVampireCoatType(IType) << vampireBit);

        
        flag = IType;
    } else {
        colors[vampireColor] = 377;
        // flag |= (VAMPIRE_COAT_BASIC << vampireBit);
        
        flag = (m_Sex ? 0 : 1);
    }

    __END_DEBUG
}


//----------------------------------------------------------------------
// save InitialRank
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void Vampire::saveInitialRank(void)

{
    VAMPIRE_RECORD prev;
    getVampireRecord(prev);

    int curRank = max(1, (m_Level + 3) / 4);
    m_pRank->SET_LEVEL(curRank);

    /*	RankExp_t accumExp = 0;

        if (curRank!=1)
        {
            RankEXPInfo* pBeforeExpInfo = g_pRankEXPInfoManager[RANK_TYPE_VAMPIRE]->getRankEXPInfo(curRank-1);
            accumExp = pBeforeExpInfo->getAccumExp();
        }

        RankEXPInfo* pNextExpInfo = g_pRankEXPInfoManager[RANK_TYPE_VAMPIRE]->getRankEXPInfo(curRank);
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

bool Vampire::addShape(Item::ItemClass IClass, ItemType_t IType, Color_t color) {
    bool bisChange = false;

    switch (IClass) {
    case Item::ITEM_CLASS_VAMPIRE_COAT: {
        bisChange = true;

        m_VampireInfo.setCoatColor(color);
        m_VampireInfo.setCoatType(IType);
    } break;

    default:
        break;
    }

    return bisChange;
}


bool Vampire::removeShape(Item::ItemClass IClass, bool bSendPacket) {
    bool bisChange = false;

    switch (IClass) {
    case Item::ITEM_CLASS_VAMPIRE_COAT: {
        m_VampireInfo.setCoatColor(377);
        m_VampireInfo.setCoatType(0);

        if (bSendPacket) // by sigi. 2002.11.6
        {
            GCTakeOff pkt;
            pkt.setObjectID(getObjectID());
            pkt.setSlotID((SlotID_t)ADDON_COAT);
            m_pZone->broadcastPacket(getX(), getY(), &pkt, this);
        }
    } break;

    default:
        return false;
    }

    return bisChange;
}

Color_t Vampire::getItemShapeColor(Item* pItem, OptionInfo* pOptionInfo) const {
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

bool Vampire::canPlayFree()

{
    __BEGIN_TRY

    return m_Level <= g_pVariableManager->getVariable(FREE_PLAY_VAMPIRE_LEVEL);

    __END_CATCH
}


bool Vampire::isPayPlayAvaiable()

{
    __BEGIN_TRY

    if (m_pPlayer == NULL)
        return false;

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);

#ifdef __CONNECT_BILLING_SYSTEM__
    if (pGamePlayer->isPayPlaying()) {
        
        if (pGamePlayer->getPayType() == PAY_TYPE_FREE)
            return true;

        
        if (m_Level <= g_pVariableManager->getVariable(FREE_PLAY_VAMPIRE_LEVEL)) {
            return true;
        }
    }

    return false;


#elif defined(__PAY_SYSTEM_FREE_LIMIT__)

    if (!pGamePlayer->isPayPlaying()) {
        
        if (m_Level <= g_pVariableManager->getVariable(FREE_PLAY_VAMPIRE_LEVEL)) {
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

void Vampire::initPetQuestTarget() {
    int minClass = 1, maxClass = 1;

    if (getLevel() <= 50) {
        minClass = 8;
        maxClass = 9;
    } else if (getLevel() <= 60) {
        minClass = maxClass = 9;
    } else if (getLevel() <= 70) {
        minClass = maxClass = 10;
    } else if (getLevel() <= 80) {
        minClass = 10;
        maxClass = 11;
    } else if (getLevel() <= 90) {
        minClass = 10;
        maxClass = 11;
    } else if (getLevel() <= 110) {
        minClass = 11;
        maxClass = 12;
    } else if (getLevel() <= 130) {
        minClass = 11;
        maxClass = 12;
    } else {
        minClass = 12;
        maxClass = 13;
    }

    m_TargetMonster = g_pMonsterInfoManager->getRandomMonsterByClass(minClass, maxClass);
    m_TargetNum = 80;
    m_TimeLimit = 3600;
}
