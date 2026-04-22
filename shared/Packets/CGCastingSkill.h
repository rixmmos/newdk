//////////////////////////////////////////////////////////////////////
// Filename    : CGCastingSkill.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_CASTING_SKILL_H__
#define __CG_CASTING_SKILL_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGCastingSkill : public Packet {
public:
    CGCastingSkill() throw();
    ~CGCastingSkill() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() { return PACKET_CG_CASTING_SKILL; }
    PacketSize_t getPacketSize() const throw() { return szSkillType; }
    string getPacketName() const throw() { return "CGCastingSkill"; }
    string toString() const throw();

    SkillType_t getSkillType() const throw() { return m_SkillType; }
    void setObjectID(SkillType_t skillType) throw() { m_SkillType = skillType; }

private:
    SkillType_t m_SkillType;
};

class CGCastingSkillFactory : public PacketFactory {
public:
    CGCastingSkillFactory() throw() {}
    virtual ~CGCastingSkillFactory() throw() {}

    Packet* createPacket() throw() { return new CGCastingSkill(); }
    string getPacketName() const throw() { return "CGCastingSkill"; }
    PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_CASTING_SKILL; }
    PacketSize_t getPacketMaxSize() const throw() { return szSkillType; }
};

class CGCastingSkillHandler {
public:
    static void execute(CGCastingSkill* pCGCastingSkill, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
