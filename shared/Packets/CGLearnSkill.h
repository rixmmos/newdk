//////////////////////////////////////////////////////////////////////////////
// Filename    : CGLearnSkill.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_LEARN_SKILL_H__
#define __CG_LEARN_SKILL_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGLearnSkill : public Packet {
public:
    CGLearnSkill() throw() {}
    ~CGLearnSkill() throw() {}

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() { return PACKET_CG_LEARN_SKILL; }
    PacketSize_t getPacketSize() const throw() { return szSkillType + szSkillDomainType; }
    string getPacketName() const throw() { return "CGLearnSkill"; }
    string toString() const throw();

    SkillType_t getSkillType() const throw() { return m_SkillType; }
    void setSkillType(SkillType_t skillType) throw() { m_SkillType = skillType; }

    SkillDomainType_t getSkillDomainType() const throw() { return m_DomainType; }
    void setSkillDomainType(SkillDomainType_t domainType) throw() { m_DomainType = domainType; }

private:
    SkillType_t m_SkillType;
    SkillDomainType_t m_DomainType;
};

class CGLearnSkillFactory : public PacketFactory {
public:
    CGLearnSkillFactory() throw() {}
    virtual ~CGLearnSkillFactory() throw() {}

    Packet* createPacket() throw() { return new CGLearnSkill(); }
    string getPacketName() const throw() { return "CGLearnSkill"; }
    PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_LEARN_SKILL; }
    PacketSize_t getPacketMaxSize() const throw() { return szSkillType + szSkillDomainType; }
};

class CGLearnSkillHandler {
public:
    static void execute(CGLearnSkill* pCGLearnSkill, Player* pPlayer) throw(ProtocolException, Error);
    static void executeSlayerSkill(CGLearnSkill* pCGLearnSkill, Player* pPlayer) throw(ProtocolException, Error);
    static void executeVampireSkill(CGLearnSkill* pCGLearnSkill, Player* pPlayer) throw(ProtocolException, Error);
    static void executeOustersSkill(CGLearnSkill* pCGLearnSkill, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
