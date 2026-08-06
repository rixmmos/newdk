//--------------------------------------------------------------------------------
//
// Filename    : RaceWarInfo.h
// Written By  :
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __RACE_WAR_LIST_H__
#define __RACE_WAR_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"
#include "WarInfo.h"

//--------------------------------------------------------------------------------
//
// class WarInfo;
//

//
//--------------------------------------------------------------------------------

class RaceWarInfo : public WarInfo {
public:
    typedef ValueList<ZoneID_t> ZoneIDList;

public:
    RaceWarInfo() {}
    ~RaceWarInfo() {}

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    PacketSize_t getSize() const {
        return WarInfo::getSize() + m_CastleIDs.getPacketSize();
    }

    static PacketSize_t getMaxSize() {
        return WarInfo::getMaxSize() + ZoneIDList::getPacketMaxSize();
    }

    // get packet's debug string
    string toString() const;

public:
    WarType_t getWarType() const {
        return WAR_RACE;
    }

    ZoneIDList& getCastleIDs() {
        return m_CastleIDs;
    }
    void addCastleID(ZoneID_t zid) {
        m_CastleIDs.addValue(zid);
    }

    void operator=(const RaceWarInfo& RWI) {
        m_StartTime = RWI.m_StartTime;
        m_RemainTime = RWI.m_RemainTime;
        m_CastleIDs = RWI.m_CastleIDs;
    }

private:
    ZoneIDList m_CastleIDs; 
};

#endif
