//////////////////////////////////////////////////////////////////////////////
// Filename    : GCExecuteElement.h
// Written By  : elca@ewestsoft.com
// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_EXECUTE_ELEMENT_H__
#define __GC_EXECUTE_ELEMENT_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class GCExecuteElement;

//////////////////////////////////////////////////////////////////////////////

class GCExecuteElement : public Packet {
public:
    GCExecuteElement();
    ~GCExecuteElement();

public:
    void read(SocketInputStream& iStream) {
        iStream.read(m_QuestID);
        iStream.read(m_Condition);
        iStream.read(m_Index);
    }
    void write(SocketOutputStream& oStream) const {
        oStream.write(m_QuestID);
        oStream.write(m_Condition);
        oStream.write(m_Index);
    }
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_EXECUTE_ELEMENT;
    }
    PacketSize_t getPacketSize() const {
        return szDWORD + szBYTE + szWORD;
    }
    string getPacketName() const {
        return "GCExecuteElement";
    }
    string toString() const;

public:
    DWORD getQuestID() const {
        return m_QuestID;
    }
    void setQuestID(DWORD id) {
        m_QuestID = id;
    }

    BYTE getCondition() const {
        return m_Condition;
    }
    void setCondition(BYTE cond) {
        m_Condition = cond;
    }

    WORD getIndex() const {
        return m_Index;
    }
    void setIndex(WORD idx) {
        m_Index = idx;
    }

private:
    DWORD m_QuestID;
    BYTE m_Condition; 
    WORD m_Index;     
};


//////////////////////////////////////////////////////////////////////////////
// class GCExecuteElementFactory;
//////////////////////////////////////////////////////////////////////////////

class GCExecuteElementFactory : public PacketFactory {
public:
    GCExecuteElementFactory() {}
    virtual ~GCExecuteElementFactory() {}

public:
    Packet* createPacket() {
        return new GCExecuteElement();
    }
    string getPacketName() const {
        return "GCExecuteElement";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_EXECUTE_ELEMENT;
    }
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + szWORD;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class GCExecuteElementHandler;
//////////////////////////////////////////////////////////////////////////////

class GCExecuteElementHandler {
public:
    static void execute(GCExecuteElement* pGCExecuteElement, Player* pPlayer);
};

#endif
