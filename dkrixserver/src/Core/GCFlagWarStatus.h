//////////////////////////////////////////////////////////////////////////////
// Filename    : GCFlagWarStatus.h
// Written By  : elca@ewestsoft.com
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_FLAG_WAR_STATUS_H__
#define __GC_FLAG_WAR_STATUS_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class GCFlagWarStatus;

//////////////////////////////////////////////////////////////////////////////

class GCFlagWarStatus : public Packet {
public:
    GCFlagWarStatus();
    ~GCFlagWarStatus();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_FLAG_WAR_STATUS;
    }
    PacketSize_t getPacketSize() const {
        return szWORD + szBYTE * 3;
    }
    string getPacketName() const {
        return "GCFlagWarStatus";
    }
    string toString() const;

public:
    WORD getTimeRemain() const {
        return m_TimeRemain;
    }
    void setTimeRemain(WORD remainTime) {
        m_TimeRemain = remainTime;
    }

    BYTE getFlagCount(Race_t race) const {
        return m_FlagCount[(int)race];
    }
    void setFlagCount(Race_t race, BYTE count) {
        m_FlagCount[(int)race] = count;
    }

private:
    WORD m_TimeRemain;
    BYTE m_FlagCount[3];
};


//////////////////////////////////////////////////////////////////////////////
// class GCFlagWarStatusFactory;
//////////////////////////////////////////////////////////////////////////////

class GCFlagWarStatusFactory : public PacketFactory {
public:
    GCFlagWarStatusFactory() {}
    virtual ~GCFlagWarStatusFactory() {}

public:
    Packet* createPacket() {
        return new GCFlagWarStatus();
    }
    string getPacketName() const {
        return "GCFlagWarStatus";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_FLAG_WAR_STATUS;
    }
    PacketSize_t getPacketMaxSize() const {
        return szWORD + szBYTE * 3;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class GCFlagWarStatusHandler;
//////////////////////////////////////////////////////////////////////////////

class GCFlagWarStatusHandler {
public:
    static void execute(GCFlagWarStatus* pGCFlagWarStatus, Player* pPlayer);
};

#endif
