//////////////////////////////////////////////////////////////////////////////
// Filename    : GCTeachSkillInfo.h

// Description :



//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_TEACH_SKILL_INFO_H__
#define __GC_TEACH_SKILL_INFO_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class GCTeachSkillInfo;
//////////////////////////////////////////////////////////////////////////////

class GCTeachSkillInfo : public Packet {
public:
    GCTeachSkillInfo() {};
    ~GCTeachSkillInfo() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_TEACH_SKILL_INFO;
    }
    PacketSize_t getPacketSize() const {
        return szSkillDomainType + szSkillLevel;
    }
    string getPacketName() const {
        return "GCTeachSkillInfo";
    }
    string toString() const;

public:
    SkillDomainType_t getDomainType(void) const {
        return m_DomainType;
    }
    void setDomainType(SkillDomainType_t type) {
        m_DomainType = type;
    }

    SkillLevel_t getTargetLevel(void) const {
        return m_TargetLevel;
    }
    void setTargetLevel(SkillLevel_t level) {
        m_TargetLevel = level;
    }

private:
    SkillDomainType_t m_DomainType; 
    SkillLevel_t m_TargetLevel;     
};

//////////////////////////////////////////////////////////////////////////////
// class GCTeachSkillInfoFactory;
//////////////////////////////////////////////////////////////////////////////

class GCTeachSkillInfoFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCTeachSkillInfo();
    }
    string getPacketName() const {
        return "GCTeachSkillInfo";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_TEACH_SKILL_INFO;
    }
    PacketSize_t getPacketMaxSize() const {
        return szSkillDomainType + szSkillLevel;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class GCTeachSkillInfoHandler;
//////////////////////////////////////////////////////////////////////////////

class GCTeachSkillInfoHandler {
public:
    static void execute(GCTeachSkillInfo* pPacket, Player* pPlayer);
};

#endif
