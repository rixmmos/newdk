//----------------------------------------------------------------------
//
// Filename    : ZoneGroupInfo.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __ZONE_GROUP_INFO_H__
#define __ZONE_GROUP_INFO_H__

// include files
#include "Exception.h"
#include "Types.h"

//----------------------------------------------------------------------
//
// class ZoneGroupInfo;
//

//

//
//----------------------------------------------------------------------

class ZoneGroupInfo {
public:
    // get/set zone group id
    ZoneGroupID_t getZoneGroupID() const throw() {
        return m_ZoneGroupID;
    }
    void setZoneGroupID(ZoneGroupID_t zoneGroupID) throw() {
        m_ZoneGroupID = zoneGroupID;
    }

    // get/set game server's nick name
    ServerID_t getServerID() const throw() {
        return m_ServerID;
    }
    void setServerID(const ServerID_t ServerID) throw() {
        m_ServerID = ServerID;
    }

    // get debug string
    string toString() const throw() {
        StringStream msg;

        msg << "ZoneGroupInfo("
            << "ZoneGroupID:" << m_ZoneGroupID << ",ServerID:" << m_ServerID << ")";

        return msg.toString();
    }

private:
    
    ZoneGroupID_t m_ZoneGroupID;

    
    ServerID_t m_ServerID;
};

#endif
