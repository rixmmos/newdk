//////////////////////////////////////////////////////////////////////
//
// Filename    : CGBloodDrain
// Written By  : crazydog
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_BLOOD_DRAIN_H__
#define __CG_BLOOD_DRAIN_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

#ifdef __LINUX__
// #include "GCGlobalHandler.h"
#endif // __LINUX__


//////////////////////////////////////////////////////////////////////
//
// class CGBloodDrain;
//
//////////////////////////////////////////////////////////////////////

class CGBloodDrain : public Packet {
public:
    // constructor
    CGBloodDrain();

    // destructor
    ~CGBloodDrain();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_BLOOD_DRAIN;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szObjectID;
    }

    // get packet name
    string getPacketName() const {
        return "CGBloodDrain";
    }

    // get packet's debug string
    string toString() const;
    /*
        // get/set X Coordicate
        Coord_t getX() const  { return m_X; }
        void setX(Coord_t x)  { m_X = x; }

        // get/set Y Coordicate
        Coord_t getY() const  { return m_Y; }
        void setY(Coord_t y)  { m_Y = y; }

        // get/set Direction
        Dir_t getDir() const  { return m_Dir; }
        void setDir(Dir_t dir)  { m_Dir = dir; }
    */
    // get/set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

private:
    ObjectID_t m_ObjectID; // ObjectID
     
};


//////////////////////////////////////////////////////////////////////
//
// class CGBloodDrainFactory;
//
// Factory for CGBloodDrain
//
//////////////////////////////////////////////////////////////////////

class CGBloodDrainFactory : public PacketFactory {
public:
    // constructor
    CGBloodDrainFactory() {}

    // destructor
    virtual ~CGBloodDrainFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new CGBloodDrain();
    }

    // get packet name
    string getPacketName() const {
        return "CGBloodDrain";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_BLOOD_DRAIN;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return szObjectID;
    }
};

//////////////////////////////////////////////////////////////////////
//
// class CGBloodDrainHandler;
//
//////////////////////////////////////////////////////////////////////

class CGBloodDrainHandler {
public:
    // execute packet's handler
    static void execute(CGBloodDrain* pCGBloodDrain, Player* player);
};

#endif
