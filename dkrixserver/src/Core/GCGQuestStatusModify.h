//////////////////////////////////////////////////////////////////////////////
// Filename    : GCGQuestStatusModify.h
// Written By  : elca@ewestsoft.com
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_GQUEST_STATUS_MODIFY_H__
#define __GC_GQUEST_STATUS_MODIFY_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "QuestStatusInfo.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class GCGQuestStatusModify;

//////////////////////////////////////////////////////////////////////////////

class GCGQuestStatusModify : public Packet {
public:
    enum ModifyType {
        NO_MODIFY, 
        CURRENT,   
        SUCCESS,   
        FAIL,      
    };

    GCGQuestStatusModify();
    ~GCGQuestStatusModify();

public:
    void read(SocketInputStream& iStream) {
        iStream.read(m_Type);
        m_pInfo = new QuestStatusInfo(0);
        m_pInfo->read(iStream);
    }
    void write(SocketOutputStream& oStream) const {
        oStream.write(m_Type);
        m_pInfo->write(oStream);
    }
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_GQUEST_STATUS_MODIFY;
    }
    PacketSize_t getPacketSize() const {
        return szBYTE + m_pInfo->getSize();
    }
    string getPacketName() const {
        return "GCGQuestStatusModify";
    }
    string toString() const;

public:
    BYTE getType() const {
        return m_Type;
    }
    void setType(BYTE type) {
        m_Type = type;
    }

    QuestStatusInfo* getInfo() const {
        return m_pInfo;
    }
    void setInfo(QuestStatusInfo* pInfo) {
        m_pInfo = pInfo;
    }

private:
    BYTE m_Type;
    QuestStatusInfo* m_pInfo;
};


//////////////////////////////////////////////////////////////////////////////
// class GCGQuestStatusModifyFactory;
//////////////////////////////////////////////////////////////////////////////

class GCGQuestStatusModifyFactory : public PacketFactory {
public:
    GCGQuestStatusModifyFactory() {}
    virtual ~GCGQuestStatusModifyFactory() {}

public:
    Packet* createPacket() {
        return new GCGQuestStatusModify();
    }
    string getPacketName() const {
        return "GCGQuestStatusModify";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_GQUEST_STATUS_MODIFY;
    }
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + QuestStatusInfo::getMaxSize();
    }
};

//////////////////////////////////////////////////////////////////////////////
// class GCGQuestStatusModifyHandler;
//////////////////////////////////////////////////////////////////////////////

class GCGQuestStatusModifyHandler {
public:
    static void execute(GCGQuestStatusModify* pGCGQuestStatusModify, Player* pPlayer);
};

#endif
