//--------------------------------------------------------------------
//
// Filename    : InfoClassManager.cpp
// Written By  : Elca
//
//--------------------------------------------------------------------

// include files
#include "InfoClassManager.h"

#include "ItemInfo.h"
#include "VariableManager.h"

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
InfoClassManager::InfoClassManager()

    : m_InfoCount(0), m_pItemInfos(NULL), m_TotalRatio(0), m_AveragePrice(0) {}


//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
InfoClassManager::~InfoClassManager()

{
    if (m_pItemInfos != NULL) {
        for (uint i = 0; i <= m_InfoCount; i++)
            SAFE_DELETE(m_pItemInfos[i]);

        SAFE_DELETE_ARRAY(m_pItemInfos);
    }
}


//--------------------------------------------------------------------------------
// init
//--------------------------------------------------------------------------------
void InfoClassManager::init()

{
    __BEGIN_TRY

    load();

    // Assert(m_InfoCount>0);

    m_TotalRatio = 0;
    m_AveragePrice = 0;
    int count = 0;

    for (uint i = 0; i <= m_InfoCount; i++) {
        Ratio_t itemRatio = m_pItemInfos[i]->getRatio();

        if (itemRatio > 0) {
            
            m_TotalRatio += itemRatio;

            
            m_AveragePrice += m_pItemInfos[i]->getPrice();

            count++;
        }
    }

    
    if (count > 1) {
        m_AveragePrice /= count;
    }

    Assert(m_pItemInfos[0] != NULL);

    
    m_AveragePrice /= 1000;
    m_AveragePrice *= 100;

    __END_CATCH
}

//--------------------------------------------------------------------------------
// reload item infos
//--------------------------------------------------------------------------------
void InfoClassManager::reload()

{
    __BEGIN_TRY

    removeAllItemInfo();

    init();

    __END_CATCH
}

//--------------------------------------------------------------------------------
// add item info
//--------------------------------------------------------------------------------
void InfoClassManager::addItemInfo(ItemInfo* pItemInfo)

{
    __BEGIN_TRY

    Assert(pItemInfo != NULL);
    Assert(pItemInfo->getItemType() < Item::ITEM_CLASS_MAX);
    Assert(m_pItemInfos[pItemInfo->getItemType()] == NULL);

    m_pItemInfos[pItemInfo->getItemType()] = pItemInfo;

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get item info
//--------------------------------------------------------------------------------
ItemInfo* InfoClassManager::getItemInfo(ItemType_t itemType) const {
    __BEGIN_TRY

    // Assert(itemType < Item::ITEM_CLASS_MAX);
    Assert(m_pItemInfos[itemType] != NULL);

    return m_pItemInfos[itemType];

    __END_CATCH
}

//--------------------------------------------------------------------------------
// remove all item infos
//--------------------------------------------------------------------------------
void InfoClassManager::removeAllItemInfo()

{
    __BEGIN_TRY

    if (m_pItemInfos != NULL) {
        for (uint i = 0; i <= m_InfoCount; i++)
            SAFE_DELETE(m_pItemInfos[i]);
    }

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get Random ItemType
//--------------------------------------------------------------------------------
ItemType_t InfoClassManager::getRandomItemType() const

{
    __BEGIN_TRY

    
    if (m_TotalRatio == 0 || m_InfoCount == 0)
        return 0;

    int gambleRatio = g_pVariableManager->getGambleItemTypeRatio(); // 200%
    int failRatio = m_pItemInfos[0]->getRatio();                    
    int succeedRatio = m_TotalRatio - failRatio;                    
    int newTotalRatio = failRatio + getPercentValue(succeedRatio, gambleRatio); 
    int itemTypeRatio = rand() % newTotalRatio;
    int ratio;
    int ratioSum = 0;

    /*
    cout << "m_TotalRatio = " << m_TotalRatio
        << ", fail/succeed = " << failRatio<< "/" << succeedRatio
        << ", newTotalRatio = " << newTotalRatio
        << ", select = " << itemTypeRatio << endl;
    */

    
    for (uint i = 0; i <= m_InfoCount; i++) {
        ItemInfo* pInfo = m_pItemInfos[i];
        ratio = pInfo->getRatio();

        
        
        if (i != 0) {
            // cout << "[" << i << "] " << ratio;
            ratio = getPercentValue(ratio, gambleRatio);

            // cout << " --> " << ratio;
        } else {
            // cout << "[" << i << "] " << ratio;
        }

        ratioSum += ratio;

        // cout << " , ratioSum/Select = " << ratioSum << "/" << itemTypeRatio << endl;

        if (itemTypeRatio < ratioSum) {
            
            return pInfo->getItemType();
        }
    }

    
    
    return 0;

    __END_CATCH
}

//--------------------------------------------------------------------------------
// toString for debug
//--------------------------------------------------------------------------------
string InfoClassManager::toString() const

{
    StringStream msg;

    msg << "InfoClassManager(";

    for (uint i = 0; i <= m_InfoCount; i++) {
        msg << m_pItemInfos[i]->toString();
    }

    msg << ")";

    return msg.toString();
}
