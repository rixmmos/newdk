/////////////////////////////////////////////////////////////////////////////
// DynamicZoneGroup.cpp
/////////////////////////////////////////////////////////////////////////////

// include files
#include "DynamicZoneGroup.h"

#include "Assert.h"
#include "DynamicZone.h"
#include "DynamicZoneFactoryManager.h"
#include "DynamicZoneInfo.h"
#include "DynamicZoneManager.h"

///////////////////////////////////////////////////////////
// class DynamicZoneGroup
///////////////////////////////////////////////////////////
DynamicZoneGroup::DynamicZoneGroup() {
    m_MaxSize = 50;
}

DynamicZoneGroup::~DynamicZoneGroup() {
    clear();
}

void DynamicZoneGroup::clear() {
    HashMapDynamicZoneItor itr = m_DynamicZones.begin();
    HashMapDynamicZoneItor endItr = m_DynamicZones.end();

    for (; itr != endItr; ++itr) {
        SAFE_DELETE(itr->second);
    }

    m_DynamicZones.clear();
}

void DynamicZoneGroup::addDynamicZone(DynamicZone* pDynamicZone) {
    HashMapDynamicZoneItor itr = m_DynamicZones.find(pDynamicZone->getZoneID());

    if (itr != m_DynamicZones.end()) {
        cerr << "Duplicated zoneID. DynamicZoneGroup::addDynamicZone" << endl;
        Assert(false);
    }

    m_DynamicZones[pDynamicZone->getZoneID()] = pDynamicZone;
}

bool DynamicZoneGroup::canEnter() {
    
    HashMapDynamicZoneItor itr = m_DynamicZones.begin();
    HashMapDynamicZoneItor endItr = m_DynamicZones.end();

    for (; itr != endItr; ++itr) {
        if (itr->second->getStatus() == DYNAMIC_ZONE_STATUS_READY) {
            return true;
        }
    }

    return getSize() < m_MaxSize;
}

DynamicZone* DynamicZoneGroup::getAvailableDynamicZone() {
    
    HashMapDynamicZoneItor itr = m_DynamicZones.begin();
    HashMapDynamicZoneItor endItr = m_DynamicZones.end();

    for (; itr != endItr; ++itr) {
        if (itr->second->getStatus() == DYNAMIC_ZONE_STATUS_READY) {
            itr->second->setStatus(DYNAMIC_ZONE_STATUS_RUNNING);
            itr->second->init();
            return itr->second;
        }
    }

    
    
    DynamicZone* pDynamicZone = NULL;

    pDynamicZone = g_pDynamicZoneFactoryManager->createDynamicZone(m_DynamicZoneType);

    pDynamicZone->setTemplateZoneID(m_TemplateZoneID);
    pDynamicZone->setZoneID(g_pDynamicZoneManager->getNewDynamicZoneID());
    pDynamicZone->setStatus(DYNAMIC_ZONE_STATUS_RUNNING);
    pDynamicZone->makeDynamicZone();
    addDynamicZone(pDynamicZone);

    return pDynamicZone;
}
