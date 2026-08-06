//////////////////////////////////////////////////////////////////////
//
// Filename    :  GCDownSkillFailed.h
// Written By  :  elca@ewestsoft.com
// Description :
//
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_DOWN_SKILL_FAILED_H__
#define __GC_DOWN_SKILL_FAILED_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class  GCDownSkillFailed;
//
//////////////////////////////////////////////////////////////////////

class GCDownSkillFailed : public Packet {
public:
    GCDownSkillFailed();
    virtual ~GCDownSkillFailed();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_DOWN_SKILL_FAILED;
    }

    // get packet size
    PacketSize_t getPacketSize() const {
        return szSkillType + szBYTE;
    }

    // get packet's name
    string getPacketName() const {
        return "GCDownSkillFailed";
    }

    // get packet's debug string
    string toString() const;

    // get/set skill type
    SkillType_t getSkillType(void) const {
        return m_SkillType;
    }
    void setSkillType(SkillType_t SkillType) {
        m_SkillType = SkillType;
    }

    // get/set description
    BYTE getDesc(void) const {
        return m_Desc;
    }
    void setDesc(BYTE desc) {
        m_Desc = desc;
    }

private:
    SkillType_t m_SkillType;
    BYTE m_Desc; 
                 
};


//////////////////////////////////////////////////////////////////////
//
// class  GCDownSkillFailedFactory;
//
// Factory for  GCDownSkillFailed
//
//////////////////////////////////////////////////////////////////////

class GCDownSkillFailedFactory : public PacketFactory {
public:
    // constructor
    GCDownSkillFailedFactory() {}

    // destructor
    virtual ~GCDownSkillFailedFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCDownSkillFailed();
    }

    // get packet name
    string getPacketName() const {
        return "GCDownSkillFailed";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_DOWN_SKILL_FAILED;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szSkillType + szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class  GCDownSkillFailedHandler;
//
//////////////////////////////////////////////////////////////////////

class GCDownSkillFailedHandler {
public:
    // execute packet's handler
    static void execute(GCDownSkillFailed* pGCDownSkillFailed, Player* pPlayer);
};

#endif // __GC_DOWN_SKILL_FAILED_H__
