//////////////////////////////////////////////////////////////////////////////
// Filename    : ZoneInfoManager.cpp
// Written By  : reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ZoneInfoManager.h"

#include "DB.h"
#include "SystemAvailabilitiesManager.h"
#include "ZoneUtil.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
ZoneInfoManager::ZoneInfoManager()

    {__BEGIN_TRY __END_CATCH}


//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
ZoneInfoManager::~ZoneInfoManager()

{
    __BEGIN_TRY

    unordered_map<ZoneID_t, ZoneInfo*>::iterator itr = m_ZoneInfos.begin();
    for (; itr != m_ZoneInfos.end(); itr++) {
        ZoneInfo* pInfo = itr->second;
        SAFE_DELETE(pInfo);
    }

    
    m_ZoneInfos.clear();

    __END_CATCH_NO_RETHROW
}


//////////////////////////////////////////////////////////////////////////////
// initialize zone info manager
//////////////////////////////////////////////////////////////////////////////
void ZoneInfoManager::init()

{
    __BEGIN_TRY

    // init == load
    load();

    __END_CATCH
}


// void testMaxMemory();

//////////////////////////////////////////////////////////////////////////////
// load from database
//////////////////////////////////////////////////////////////////////////////
void ZoneInfoManager::load()

{
    __BEGIN_TRY

    bool bReload = !m_ZoneInfos.empty();

    Statement* pStmt = NULL;

    BEGIN_DB {
        // create statement
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery("SELECT ZoneID, ZoneGroupID, Type, Level, AccessMode, OwnerID, "
                                              "PayPlayZone, PremiumZone, PKZone, NoPortalZone, HolyLand, Available, "
                                              "OpenLevel, SMPFilename, SSIFilename, FullName, ShortName FROM ZoneInfo");

        while (pResult->next()) {
            uint i = 0;

            ZoneID_t zoneID = pResult->getInt(++i);

            //			cout << "load ZoneInfo = " << zoneID << endl;

            ZoneInfo* pZoneInfo = NULL;
            bool bExistInfo = false;

            if (bReload) {
                unordered_map<ZoneID_t, ZoneInfo*>::iterator itr = m_ZoneInfos.find(zoneID);

                if (itr != m_ZoneInfos.end()) {
                    pZoneInfo = itr->second;
                    bExistInfo = true;
                } else {
                    pZoneInfo = new ZoneInfo();
                }
            } else {
                pZoneInfo = new ZoneInfo();
            }
            // cout << "new OK" << endl;

            // if (zoneID!=31 && zoneID!=21)
            {
                pZoneInfo->setZoneID(zoneID);
                pZoneInfo->setZoneGroupID(pResult->getInt(++i));
                pZoneInfo->setZoneType(pResult->getString(++i));
                pZoneInfo->setZoneLevel(pResult->getInt(++i));
                pZoneInfo->setZoneAccessMode(pResult->getString(++i));
                pZoneInfo->setZoneOwnerID(pResult->getString(++i));
                pZoneInfo->setPayPlay(pResult->getInt(++i) != 0);
                pZoneInfo->setPremiumZone(pResult->getInt(++i) != 0);
                pZoneInfo->setPKZone(pResult->getInt(++i) != 0);
                pZoneInfo->setNoPortalZone(pResult->getInt(++i) != 0);
                pZoneInfo->setHolyLand(pResult->getInt(++i) != 0);
                pZoneInfo->setAvailable(pResult->getInt(++i) != 0);
                pZoneInfo->setOpenLevel(pResult->getInt(++i));
                pZoneInfo->setSMPFilename(pResult->getString(++i));
                pZoneInfo->setSSIFilename(pResult->getString(++i));
                pZoneInfo->setFullName(pResult->getString(++i));
                pZoneInfo->setShortName(pResult->getString(++i));

                pZoneInfo->setAvailable(pZoneInfo->isAvailable() &&
                                        pZoneInfo->getOpenLevel() <
                                            SystemAvailabilitiesManager::getInstance()->getZoneOpenDegree());

                if (!bExistInfo) {
                    addZoneInfo(pZoneInfo);
                }

                /*
                if (zoneID==22)
                {
                    testMaxMemory();
                }
                */

                // cout << "load ZoneInfo = " << zoneID << endl;
                // cout << "ZoneInfo = " << pZoneInfo->toString().c_str() << endl << endl;
            }
            /*
            else
            {
                cout << "skip load ZoneID = " << i << endl << endl;
            }
            */
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// add zone info to zone info manager
//////////////////////////////////////////////////////////////////////////////
void ZoneInfoManager::addZoneInfo(ZoneInfo* pZoneInfo)

{
    __BEGIN_TRY

    
    unordered_map<ZoneID_t, ZoneInfo*>::iterator itr = m_ZoneInfos.find(pZoneInfo->getZoneID());

    if (itr != m_ZoneInfos.end())
        
        throw Error("duplicated zone id");

    m_ZoneInfos[pZoneInfo->getZoneID()] = pZoneInfo;

    
    
    unordered_map<string, ZoneInfo*>::iterator fitr = m_FullNameMap.find(pZoneInfo->getFullName());
    if (fitr != m_FullNameMap.end()) {
        cerr << "Duplicated Zone Full Name:" << pZoneInfo->getFullName() << endl;
        throw Error("Duplicated Zone Full Name");
    }

    m_FullNameMap[pZoneInfo->getFullName()] = pZoneInfo;

    
    
    unordered_map<string, ZoneInfo*>::iterator sitr = m_ShortNameMap.find(pZoneInfo->getShortName());
    if (sitr != m_ShortNameMap.end()) {
        cerr << "Duplicated Zone Short Name" << endl;
        throw Error("Duplicated Zone Short Name");
    }

    m_ShortNameMap[pZoneInfo->getShortName()] = pZoneInfo;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// Delete zone info from zone info manager
//////////////////////////////////////////////////////////////////////////////
void ZoneInfoManager::deleteZoneInfo(ZoneID_t zoneID) {
    __BEGIN_TRY

    unordered_map<ZoneID_t, ZoneInfo*>::iterator itr = m_ZoneInfos.find(zoneID);

    if (itr != m_ZoneInfos.end()) {
        
        SAFE_DELETE(itr->second);

        
        m_ZoneInfos.erase(itr);
    } else {
        
        StringStream msg;
        msg << "ZoneID : " << zoneID;
        throw NoSuchElementException(msg.toString());
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// get zone from zone info manager
//////////////////////////////////////////////////////////////////////////////
ZoneInfo* ZoneInfoManager::getZoneInfo(ZoneID_t zoneID) {
    __BEGIN_TRY

    ZoneInfo* pZoneInfo = NULL;

    unordered_map<ZoneID_t, ZoneInfo*>::iterator itr = m_ZoneInfos.find(zoneID);

    if (itr != m_ZoneInfos.end()) {
        pZoneInfo = itr->second;

    } else {
        
        StringStream msg;
        msg << "ZoneID : " << zoneID;
        throw NoSuchElementException(msg.toString());
    }

    return pZoneInfo;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get zone from zone info manager
//////////////////////////////////////////////////////////////////////////////
ZoneInfo* ZoneInfoManager::getZoneInfoByName(const string& ZoneName) {
    
    unordered_map<string, ZoneInfo*>::const_iterator short_itr = m_ShortNameMap.find(ZoneName);
    if (short_itr != m_ShortNameMap.end()) {
        return short_itr->second;
    }

    
    unordered_map<string, ZoneInfo*>::const_iterator full_itr = m_FullNameMap.find(ZoneName);
    if (full_itr != m_FullNameMap.end()) {
        return full_itr->second;
    }

    
    return NULL;
}

vector<Zone*> ZoneInfoManager::getNormalFields() const {
    vector<Zone*> ret;

    //	unordered_map< ZoneID_t , ZoneInfo *>::const_iterator itr = m_ZoneInfos.begin();
    //	unordered_map< ZoneID_t , ZoneInfo *>::const_iterator endItr = m_ZoneInfos.end();
    //
    //	for ( ; itr != endItr ; ++itr )
    //	{
    //		if ( itr->second->getZoneType() == ZONE_NORMAL_FIELD ) ret.push_back( getZoneByZoneID( itr->first ) );
    //	}

    ret.push_back(getZoneByZoneID(13));

    return ret;
}


//////////////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////////////
string ZoneInfoManager::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "ZoneInfoManager(";

    if (m_ZoneInfos.empty())
        msg << "EMPTY";
    else {
        for (unordered_map<ZoneID_t, ZoneInfo*>::const_iterator itr = m_ZoneInfos.begin(); itr != m_ZoneInfos.end();
             itr++) {
            msg << itr->second->toString();
        }
    }

    msg << ")";

    return msg.toString();

    __END_CATCH
}


// global variable definition
ZoneInfoManager* g_pZoneInfoManager = NULL;
