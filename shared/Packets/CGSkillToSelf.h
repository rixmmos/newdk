//////////////////////////////////////////////////////////////////////
// Filename    : CGSkillToSelf.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SKILL_TO_SELF_H__
#define __CG_SKILL_TO_SELF_H__

#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

class CGSkillToSelf : public Packet {
public:
    CGSkillToSelf() throw();
    ~CGSkillToSelf() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_SKILL_TO_SELF;
    }
    PacketSize_t getPacketSize() const throw() {
        return szSkillType + szCEffectID;
    }
    string getPacketName() const throw() {
        return "CGSkillToSelf";
    }
    string toString() const throw();

    SkillType_t getSkillType() const throw() {
        return m_SkillType;
    }
    void setSkillType(SkillType_t skillType) throw() {
        m_SkillType = skillType;
    }

    CEffectID_t getCEffectID() const throw() {
        return m_CEffectID;
    }
    void setCEffectID(CEffectID_t cEffectID) throw() {
        m_CEffectID = cEffectID;
    }

private:
    SkillType_t m_SkillType;
    CEffectID_t m_CEffectID;
};

class CGSkillToSelfFactory : public PacketFactory {
public:
    CGSkillToSelfFactory() throw() {}
    virtual ~CGSkillToSelfFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGSkillToSelf();
    }
    string getPacketName() const throw() {
        return "CGSkillToSelf";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SKILL_TO_SELF;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szSkillType + szCEffectID;
    }
};

class CGSkillToSelfHandler {
public:
    static void execute(CGSkillToSelf* pCGSkillToSelf, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
