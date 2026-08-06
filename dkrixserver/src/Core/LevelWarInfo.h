//--------------------------------------------------------------------------------
//
// Filename    : LevelWarInfo.h
// Written By  :
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __LEVEL_WAR_LIST_H__
#define __LEVEL_WAR_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"
#include "WarInfo.h"

//--------------------------------------------------------------------------------
//
// class WarInfo;
//
//--------------------------------------------------------------------------------

class LevelWarInfo : public WarInfo {
public:
    LevelWarInfo() {}
    ~LevelWarInfo() {}

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    PacketSize_t getSize() const {
        return WarInfo::getSize() + szint;
    }

    static PacketSize_t getMaxSize() {
        return WarInfo::getMaxSize() + szint;
    }

    // get packet's debug string
    string toString() const;

public:
    WarType_t getWarType() const {
        return WAR_LEVEL;
    }

    int getLevel() {
        return m_Level;
    }
    void setLevel(int level) {
        m_Level = level;
    }

    void operator=(const LevelWarInfo& RWI) {
        m_StartTime = RWI.m_StartTime;
        m_RemainTime = RWI.m_RemainTime;
        m_Level = RWI.m_Level;
    }

private:
    int m_Level; 
};

#endif
