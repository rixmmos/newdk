//////////////////////////////////////////////////////////////////////
// Filename    : CGSkillToNamed.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SKILL_TO_NAMED_H__
#define __CG_SKILL_TO_NAMED_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGSkillToNamed : public Packet {
public:
    CGSkillToNamed() throw();
    ~CGSkillToNamed() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_SKILL_TO_NAMED;
    }
    PacketSize_t getPacketSize() const throw() {
        return szSkillType + szCEffectID + szBYTE + m_TargetName.size();
    }
    string getPacketName() const throw() {
        return "CGSkillToNamed";
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

    const string& getTargetName() const throw() {
        return m_TargetName;
    }
    void setTargetName(const string& targetName) throw() {
        m_TargetName = targetName;
    }

private:
    SkillType_t m_SkillType;
    CEffectID_t m_CEffectID;
    string m_TargetName;
};

class CGSkillToNamedFactory : public PacketFactory {
public:
    CGSkillToNamedFactory() throw() {}
    virtual ~CGSkillToNamedFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGSkillToNamed();
    }
    string getPacketName() const throw() {
        return "CGSkillToNamed";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SKILL_TO_NAMED;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szSkillType + szCEffectID + szBYTE + 20;
    }
};

class CGSkillToNamedHandler {
public:
    static void execute(CGSkillToNamed* pCGSkillToNamed, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
