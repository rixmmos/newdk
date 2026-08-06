//----------------------------------------------------------------------
//
// Filename    : CGPortCheck.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __CG_PORT_CHECK_H__
#define __CG_PORT_CHECK_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class CGPortCheck;
//



//
// *CAUTION*
//






//
//----------------------------------------------------------------------

class CGPortCheck : public DatagramPacket {
public:
    CGPortCheck() {};
    ~CGPortCheck() {};
    
    void read(Datagram& iDatagram);

    
    void write(Datagram& oDatagram) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_PORT_CHECK;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_PCName.size(); // PC name
    }

    // get packet name
    string getPacketName() const {
        return "CGPortCheck";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set pcName
    string getPCName() const {
        return m_PCName;
    }
    void setPCName(const string& pcName) {
        m_PCName = pcName;
    }

private:
    // PC name
    string m_PCName;
};


//////////////////////////////////////////////////////////////////////
//
// class CGPortCheckFactory;
//
// Factory for CGPortCheck
//
//////////////////////////////////////////////////////////////////////

class CGPortCheckFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGPortCheck();
    }

    // get packet name
    string getPacketName() const {
        return "CGPortCheck";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_PORT_CHECK;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 20; // PC name
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGPortCheckHandler;
//
//////////////////////////////////////////////////////////////////////

class CGPortCheckHandler {
public:
    // execute packet's handler
    static void execute(CGPortCheck* pPacket);
};

#endif
