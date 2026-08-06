//----------------------------------------------------------------------
//
// Filename    : GGCommand.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __GG_COMMAND_H__
#define __GG_COMMAND_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class GGCommand;
//



//
// *CAUTION*
//






//
//----------------------------------------------------------------------

class GGCommand : public DatagramPacket {
public:
    GGCommand() {};
    ~GGCommand() {};
    
    void read(Datagram& iDatagram);

    
    void write(Datagram& oDatagram) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GG_COMMAND;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Command.size();
    }

    // get packet name
    string getPacketName() const {
        return "GGCommand";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set playerID
    const string& getCommand() const {
        return m_Command;
    }
    void setCommand(const string& command) {
        m_Command = command;
    }

private:
    string m_Command;
};


//////////////////////////////////////////////////////////////////////
//
// class GGCommandFactory;
//
// Factory for GGCommand
//
//////////////////////////////////////////////////////////////////////

class GGCommandFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GGCommand();
    }

    // get packet name
    string getPacketName() const {
        return "GGCommand";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GG_COMMAND;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 80;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GGCommandHandler;
//
//////////////////////////////////////////////////////////////////////

class GGCommandHandler {
public:
    // execute packet's handler
    static void execute(GGCommand* pPacket);
};

#endif
