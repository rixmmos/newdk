//----------------------------------------------------------------------
//
// Filename    : ZoneInfo.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __ZONE_INFO_H__
#define __ZONE_INFO_H__

// include files
#include "Exception.h"
#include "Types.h"

//----------------------------------------------------------------------
//
// class ZoneInfo;
//

//

//
//----------------------------------------------------------------------

class ZoneInfo {
public:
    // get/set zone id
    ZoneID_t getZoneID() const throw() {
        return m_ZoneID;
    }
    void setZoneID(ZoneID_t zoneID) throw() {
        m_ZoneID = zoneID;
    }

    // get/set zone group id
    ZoneGroupID_t getZoneGroupID() const throw() {
        return m_ZoneGroupID;
    }
    void setZoneGroupID(ZoneGroupID_t zoneGroupID) throw() {
        m_ZoneGroupID = zoneGroupID;
    }

    // get debug string
    string toString() const throw() {
        StringStream msg;
        msg << "ZoneInfo(ZoneID:" << m_ZoneID << ",ZoneGroupID:" << m_ZoneGroupID << ")";
        return msg.toString();
    }

private:
    
    ZoneID_t m_ZoneID;

    
    ZoneGroupID_t m_ZoneGroupID;
};

#endif
