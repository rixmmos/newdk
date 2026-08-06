//////////////////////////////////////////////////////////////////////
//
// Filename    : GCKnocksTargetBackOK5.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_KNOCKS_TARGET_BACK_OK_5_H__
#define __GC_KNOCKS_TARGET_BACK_OK_5_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCKnocksTargetBackOK5;
//

//
//////////////////////////////////////////////////////////////////////

class GCKnocksTargetBackOK5 : public Packet {
public:
    // constructor
    GCKnocksTargetBackOK5();

    // destructor
    ~GCKnocksTargetBackOK5();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_KNOCKS_TARGET_BACK_OK_5;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return szObjectID + szObjectID + szbool + szSkillType + szCoord * 2 + szDir;
    }

    // get packet's name
    string getPacketName() const {
        return "GCKnocksTargetBackOK5";
    }

    // get packet's debug string
    string toString() const;

    // get / set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    // get / set ObjectID
    ObjectID_t getTargetObjectID() const {
        return m_TargetObjectID;
    }
    void setTargetObjectID(ObjectID_t TargetObjectID) {
        m_TargetObjectID = TargetObjectID;
    }
    //	Coord_t getTargetX() const  { return m_X;}
    //	Coord_t getTargetY() const  { return m_Y;}
    //	void setTargetXY(Coord_t X, Coord_t Y)  { m_X = X; m_Y = Y;}

    // get / set success
    bool getSkillSuccess() const {
        return m_bSuccess;
    }
    void setSkillSuccess(bool bSuccess) {
        m_bSuccess = bSuccess;
    }

    void setXYDir(Coord_t x, Coord_t y, Coord_t dir) {
        m_X = x;
        m_Y = y;
        m_dir = dir;
    }
    Coord_t getX() const {
        return m_X;
    }
    Coord_t getY() const {
        return m_Y;
    }
    Dir_t getDir() const {
        return m_dir;
    }

    SkillType_t getSkillType() const {
        return m_SkillType;
    }
    void setSkillType(SkillType_t s) {
        m_SkillType = s;
    }

private:
    // ObjectID
    ObjectID_t m_ObjectID;
    //	Coord_t m_X, m_Y;

    // TargetObjectID
    ObjectID_t m_TargetObjectID;

    // bool
    bool m_bSuccess;

    Coord_t m_X, m_Y; 
    Dir_t m_dir;      
    SkillType_t m_SkillType;
};


//////////////////////////////////////////////////////////////////////
//
// class GCKnocksTargetBackOK5Factory;
//
// Factory for GCKnocksTargetBackOK5
//
//////////////////////////////////////////////////////////////////////

class GCKnocksTargetBackOK5Factory : public PacketFactory {
public:
    // constructor
    GCKnocksTargetBackOK5Factory() {}

    // destructor
    virtual ~GCKnocksTargetBackOK5Factory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCKnocksTargetBackOK5();
    }

    // get packet name
    string getPacketName() const {
        return "GCKnocksTargetBackOK5";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_KNOCKS_TARGET_BACK_OK_5;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szObjectID + szbool + szSkillType + szCoord * 2 + szDir;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCKnocksTargetBackOK5Handler;
//
//////////////////////////////////////////////////////////////////////

class GCKnocksTargetBackOK5Handler {
public:
    // execute packet's handler
    static void execute(GCKnocksTargetBackOK5* pGCKnocksTargetBackOK5, Player* pPlayer);
};

#endif
