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

    // load() sizes m_pItemInfos to MAX(ItemType) + 1 from this class's own table and
    // addItemInfo() fills it by ItemType, so a table whose ItemType column has a hole
    // leaves a NULL slot here. Every shipped table happens to be dense from 0
    // [measured 2026-08-11, 88 tables], which is the only reason the unguarded
    // dereference below has never turned one bad DB row into a startup SIGSEGV.
    if (m_pItemInfos == NULL) {
        filelog("ItemError.log", "InfoClassManager::init() : itemClass %d loaded no item info table.",
                (int)getItemClass());
        return;
    }

    for (uint i = 0; i <= m_InfoCount; i++) {
        if (m_pItemInfos[i] == NULL) {
            filelog("ItemError.log",
                    "InfoClassManager::init() : itemClass %d has no item info for itemType %u; skipped.",
                    (int)getItemClass(), (uint)i);
            continue;
        }

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

    // getRandomItemType() reads slot 0 unconditionally as the "fail" ratio, so a
    // missing itemType 0 has to disable the random draw rather than fault inside it.
    // This was a bare Assert(m_pItemInfos[0] != NULL), which indexed the array in
    // order to test it and aborted startup instead of degrading.
    if (m_pItemInfos[0] == NULL) {
        filelog("ItemError.log",
                "InfoClassManager::init() : itemClass %d has no item info for itemType 0; random draw disabled.",
                (int)getItemClass());
        m_TotalRatio = 0;
    }


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

    // The bound here was Item::ITEM_CLASS_MAX -- the number of item *classes*
    // (90), used to bound an item *type*. Wrong dimension, and only coincidentally
    // large enough: load() sizes m_pItemInfos to MAX(ItemType) + 1 from each
    // class's own table, and the largest of those in the shipped data is SkullInfo
    // at ItemType 75 [measured 2026-08-11, initdb/DARKEDEN.sql, 88 tables]. A class
    // that grows past 89 types would start failing this Assert at startup while
    // still fitting its own array. Bounded against what load() actually allocated.
    if (m_pItemInfos == NULL || pItemInfo->getItemType() > m_InfoCount)
        throw OutOfBoundException("InfoClassManager::addItemInfo: item type out of range");

    Assert(m_pItemInfos[pItemInfo->getItemType()] == NULL);

    m_pItemInfos[pItemInfo->getItemType()] = pItemInfo;

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get item info
//--------------------------------------------------------------------------------
// The only bounds check this ever had was commented out, and it was against the
// wrong dimension anyway (item classes, not item types -- see addItemInfo). With
// it gone the surviving `Assert(m_pItemInfos[itemType] != NULL)` *is* the
// out-of-bounds read: it indexes the array in order to test the element, so a
// stray itemType faults inside the guard rather than being caught by it. Same
// shape as SkillDomainInfoManager::getDomainInfo in 00a5372.
//
// load() allocates m_InfoCount + 1 entries from each class's own
// SELECT MAX(ItemType), so m_InfoCount is the real last valid index.
// ItemInfoManager::isPossibleItem already gates on getInfoCount() before calling
// here, which is the idiom this restores.
//
// Throws rather than returning NULL: every caller dereferences the result
// immediately (getVolumeWidth/getWeight/getPrice/... on the returned pointer), so
// a NULL sentinel would only move the fault one line down.
ItemInfo* InfoClassManager::getItemInfo(ItemType_t itemType) const {
    __BEGIN_TRY

    if (m_pItemInfos == NULL || itemType > m_InfoCount)
        throw OutOfBoundException("InfoClassManager::getItemInfo: item type out of range");

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
