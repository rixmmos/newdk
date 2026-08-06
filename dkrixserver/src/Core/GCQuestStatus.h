//////////////////////////////////////////////////////////////////////////////
// Filename    : GCQuestStatus.h
// Written By  : elca@ewestsoft.com
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_QUEST_STATUS_H__
#define __GC_QUEST_STATUS_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class GCQuestStatus;

//////////////////////////////////////////////////////////////////////////////

class GCQuestStatus : public Packet {
public:
    GCQuestStatus();
    ~GCQuestStatus();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_QUEST_STATUS;
    }
    PacketSize_t getPacketSize() const {
        return szWORD + szWORD + szDWORD;
    }
    string getPacketName() const {
        return "GCQuestStatus";
    }
    string toString() const;

public:
    WORD getQuestID() const {
        return m_QuestID;
    }
    void setQuestID(WORD e) {
        m_QuestID = e;
    }

    WORD getCurrentNum() const {
        return m_CurrentNum;
    }
    void setCurrentNum(WORD n) {
        m_CurrentNum = n;
    }

    DWORD getRemainTime() const {
        return m_Time;
    }
    void setRemainTime(DWORD d) {
        m_Time = d;
    }

private:
    WORD m_QuestID;
    WORD m_CurrentNum;
    DWORD m_Time;
};


//////////////////////////////////////////////////////////////////////////////
// class GCQuestStatusFactory;
//////////////////////////////////////////////////////////////////////////////

class GCQuestStatusFactory : public PacketFactory {
public:
    GCQuestStatusFactory() {}
    virtual ~GCQuestStatusFactory() {}

public:
    Packet* createPacket() {
        return new GCQuestStatus();
    }
    string getPacketName() const {
        return "GCQuestStatus";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_QUEST_STATUS;
    }
    PacketSize_t getPacketMaxSize() const {
        return szWORD + szWORD + szDWORD;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class GCQuestStatusHandler;
//////////////////////////////////////////////////////////////////////////////

class GCQuestStatusHandler {
public:
    static void execute(GCQuestStatus* pGCQuestStatus, Player* pPlayer);
};

#endif
