////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionRegenShop.cpp
// Written By  :
// Description :


////////////////////////////////////////////////////////////////////////////////

#include "ActionRegenShop.h"

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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ActionRegenShop::ActionRegenShop()

{
    __BEGIN_TRY

    m_Period.tv_sec = 0;
    m_Period.tv_usec = 0;
    m_NextRegen.tv_sec = 0;
    m_NextRegen.tv_usec = 0;

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ActionRegenShop::~ActionRegenShop()

{
    __BEGIN_TRY

    clearList();

    __END_CATCH_NO_RETHROW
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ActionRegenShop::read(PropertyBuffer& propertyBuffer)

{
    __BEGIN_TRY

    try {
        // read NPC id
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


        int nSecond = propertyBuffer.getPropertyInt("Period");
        m_Period.tv_sec = nSecond;


        Timeval currentTime;
        getCurrentTime(currentTime);
        m_NextRegen = currentTime;
    } catch (NoSuchElementException& nsee) {
        throw Error(nsee.toString());
    }

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void ActionRegenShop::execute(Creature* pCreature1, Creature* pCreature2)

{
    __BEGIN_TRY

    Assert(pCreature1 != NULL);
    Assert(pCreature1->isNPC());

    NPC* pNPC = dynamic_cast<NPC*>(pCreature1);
    Assert(pNPC != NULL);

    Zone* pZone = pNPC->getZone();
    Assert(pZone != NULL);


    Timeval currentTime;
    getCurrentTime(currentTime);


    if (currentTime < m_NextRegen)
        return;


    VSRect rect(0, 0, pZone->getWidth() - 1, pZone->getHeight() - 1);
    int centerX = pNPC->getX();
    int centerY = pNPC->getY();
    try {
        for (int zx = centerX - 5; zx <= centerX + 5; zx++) {
            for (int zy = centerY - 5; zy <= centerY + 5; zy++) {
                if (!rect.ptInRect(zx, zy)) {
                    continue;
                }

                Tile& tile = pZone->getTile(zx, zy);


                if (tile.hasCreature(Creature::MOVE_MODE_WALKING)) {
                    Creature* pNearCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
                    Assert(pNearCreature != NULL);

                    if (pNearCreature->isPC()) {
                        return;
                    }
                }

                if (tile.hasCreature(Creature::MOVE_MODE_FLYING)) {
                    Creature* pNearCreature = tile.getCreature(Creature::MOVE_MODE_FLYING);
                    Assert(pNearCreature != NULL);

                    if (pNearCreature->isPC()) {
                        return;
                    }
                }
            }
        }
    } catch (Throwable& t) {
        filelog("shopbug.txt", "%s", t.toString().c_str());
        return;
    }

    try {
        pNPC->clearShopItem();

        for (int i = 0; i < SHOP_RACK_TYPE_MAX; i++)
            pNPC->increaseShopVersion(i);


        list<ShopTemplateID_t> IDList[SHOP_RACK_TYPE_MAX];
        int combi[SHOP_RACK_TYPE_MAX] = {0, 0, 0};
        int count[SHOP_RACK_TYPE_MAX] = {0, 0, 0};
        int trialMax = 0;
        ShopTemplate* pTemplate = NULL;
        ShopRackType_t shopType;
        int itemClass;
        ItemType_t minItemType, maxItemType, itemType;
        uint minOptionLevel, maxOptionLevel;
        OptionType_t optionType = 0;


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


                        if (i != SHOP_RACK_MYSTERIOUS && optionVector.size() > 0) {
                            int randValue = rand();
                            int size = optionVector.size();
                            int index = randValue % size;

                            optionType = optionVector[index];
                        } else
                            optionType = 0;


                        Item::ItemClass IClass = Item::ItemClass(itemClass);
                        list<OptionType_t> optionTypes;

                        if (optionType != 0)
                            optionTypes.push_back(optionType);

                        Item* pItem = g_pItemFactoryManager->createItem(IClass, itemType, optionTypes);
                        Assert(pItem != NULL);


                        (pZone->getObjectRegistry()).registerObject(pItem);


                        if (count[i] < SHOP_RACK_INDEX_MAX) {
                            pNPC->insertShopItem(i, count[i], pItem);

                            count[i] = count[i] + 1;
                        }
                    }
                }
            }
        }

    } catch (Error& t) {
        filelog("regenShopBug.txt", "%s", t.toString().c_str());
        throw;
    } catch (Throwable& t) {
        filelog("regenShopBug.txt", "%s", t.toString().c_str());
    }


    m_NextRegen = m_NextRegen + m_Period;

    __END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionRegenShop::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    int i = 0;

    msg << "ActionRegenShop(";

    list<ShopTemplateID_t>::const_iterator itr = m_List.begin();
    for (; itr != m_List.end(); itr++)
        msg << "Item" << i++ << ":" << (int)(*itr) << ",";

    msg << ")";

    return msg.toString();

    __END_CATCH
}
