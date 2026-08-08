//////////////////////////////////////////////////////////////////////////////
// Filename    : ActionPrepareShop.cpp
// Written By  :
// Description :


//////////////////////////////////////////////////////////////////////////////

#include "ActionPrepareShop.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "Creature.h"
#include "DB.h"
#include "GamePlayer.h"
#include "ItemFactoryManager.h"
#include "LogClient.h"
#include "NPC.h"
#include "OptionInfo.h"
#include "PreparedStatement.h"
#include "ShopTemplate.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
ActionPrepareShop::ActionPrepareShop()

{
    __BEGIN_TRY

    m_ListNum = 0;
    m_MarketCondBuy = 100;
    m_MarketCondSell = 100;
    m_TaxingCastleZoneID = 0;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
ActionPrepareShop::~ActionPrepareShop()

{
    __BEGIN_TRY

    clearList();

    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ActionPrepareShop::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY

    try {
        int NPCID = propertyBuffer.getPropertyInt("NPCID");
        Statement* pStmt = NULL;

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
            PreparedStatement selectShopTemplateStmt(pConn, "SELECT ID from ShopTemplate where NPCID = ?");
            selectShopTemplateStmt.bindInt(1, NPCID);
            Result* pResult = selectShopTemplateStmt.execute();
            while (pResult->next()) {
                ShopTemplateID_t id = pResult->getInt(1);
                addListElement(id);
            }
        }
        END_DB(pStmt)

        m_MarketCondBuy = propertyBuffer.getPropertyInt("MarketConditionBuy");
        m_MarketCondSell = propertyBuffer.getPropertyInt("MarketConditionSell");

        //		propertyBuffer.getPropertyInt("CastleZoneID", (int&)m_TaxingCastleZoneID);
    } catch (NoSuchElementException& nsee) {
        throw Error(nsee.toString());
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
void ActionPrepareShop::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature1->isNPC());

    NPC* pNPC = dynamic_cast<NPC*>(pCreature1);


    list<ShopTemplateID_t> IDList[SHOP_RACK_TYPE_MAX];
    int combi[SHOP_RACK_TYPE_MAX] = {0, 0, 0};
    int count[SHOP_RACK_TYPE_MAX] = {0, 0, 0};
    int trialMax = 0;
    ShopTemplate* pTemplate = NULL;
    ShopRackType_t shopType;
    int itemClass;
    ItemType_t minItemType, maxItemType, itemType;
    uint minOptionLevel, maxOptionLevel;
    OptionType_t optionType;


    for (list<ShopTemplateID_t>::const_iterator itr = m_List.begin(); itr != m_List.end(); itr++) {
        pTemplate = g_pShopTemplateManager->getTemplate((*itr));

        Assert(pTemplate != NULL);

        shopType = pTemplate->getShopType();
        itemClass = pTemplate->getItemClass();
        minItemType = pTemplate->getMinItemType();
        maxItemType = pTemplate->getMaxItemType();


        IDList[shopType].push_back(*itr);
        combi[shopType] += (maxItemType - minItemType + 1);
    }


    for (ShopRackType_t i = 0; i < SHOP_RACK_TYPE_MAX; i++) {
        if (combi[i] == 0)
            trialMax = 0;
        else
            trialMax = (int)(floor(SHOP_RACK_INDEX_MAX / combi[i]));


        if (i == SHOP_RACK_NORMAL || i == SHOP_RACK_MYSTERIOUS)
            trialMax = 1;


        for (list<ShopTemplateID_t>::const_iterator itr = IDList[i].begin(); itr != IDList[i].end(); itr++) {
            pTemplate = g_pShopTemplateManager->getTemplate((*itr));
            itemClass = pTemplate->getItemClass();
            minItemType = pTemplate->getMinItemType();
            maxItemType = pTemplate->getMaxItemType();
            minOptionLevel = pTemplate->getMinOptionLevel();
            maxOptionLevel = pTemplate->getMaxOptionLevel();


            vector<OptionType_t> optionVector = g_pOptionInfoManager->getPossibleOptionVector(
                (Item::ItemClass)itemClass, minOptionLevel, maxOptionLevel);

            for (ItemType_t type = minItemType; type <= maxItemType; type++) {
                for (int tc = 0; tc < trialMax; tc++) {
                    itemType = type;


                    Item::ItemClass IClass = Item::ItemClass(itemClass);
                    list<OptionType_t> optionTypes;
                    if (i != SHOP_RACK_MYSTERIOUS && optionVector.size() > 0) {
                        optionType = optionVector[(rand() % optionVector.size())];

                        if (optionType != 0)
                            optionTypes.push_back(optionType);
                    }


                    Item* pItem = g_pItemFactoryManager->createItem(IClass, itemType, optionTypes);
                    Assert(pItem != NULL);


                    (pNPC->getZone()->getObjectRegistry()).registerObject(pItem);


                    if (count[i] < SHOP_RACK_INDEX_MAX) {
                        pNPC->insertShopItem(i, count[i], pItem);

                        count[i] = count[i] + 1;

                        log(LOG_SHOP_CREATE_ITEM, pNPC->getName(), "", pItem->toString());
                    }
                }
            }
        }
    }

    pNPC->setMarketCondBuy(m_MarketCondBuy);
    pNPC->setMarketCondSell(m_MarketCondSell);
    //	pNPC->setTaxingCastleZoneID(m_TaxingCastleZoneID);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ActionPrepareShop::addListElement(ShopTemplateID_t id)

{
    __BEGIN_TRY

    m_List.push_back(id);
    m_ListNum++;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string ActionPrepareShop::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    int i = 0;

    msg << "ActionPrepareShop(";
    list<ShopTemplateID_t>::const_iterator itr = m_List.begin();
    for (; itr != m_List.end(); itr++)
        msg << "Item" << i++ << ":" << (int)(*itr) << ",";
    msg << ")";

    return msg.toString();

    __END_CATCH
}
