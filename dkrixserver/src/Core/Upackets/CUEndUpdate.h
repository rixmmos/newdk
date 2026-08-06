//--------------------------------------------------------------------------------
//
// Filename    : CUEndUpdate.h
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __CU_END_UPDATE_H__
#define __CU_END_UPDATE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CUEndUpdate;
//
//
//--------------------------------------------------------------------------------

class CUEndUpdate : public Packet {
public:
    
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);

    
    void read(Socket* pSocket) throw(ProtocolException, Error);

    
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);

    // execute packet's handler
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    // get packet id
    PacketID_t getPacketID() const throw() {
        return PACKET_CU_END_UPDATE;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const throw() {
        return 0;
    }

    //
    static PacketSize_t getPacketMaxSize() throw() {
        return 0;
    }

    // get packet name
    string getPacketName() const throw() {
        return "CUEndUpdate";
    }

    // get packet's debug string
    string toString() const throw();
};


//--------------------------------------------------------------------------------
//
// class CUEndUpdateFactory;
//
// Factory for CUEndUpdate
//
//--------------------------------------------------------------------------------

class CUEndUpdateFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() throw() {
        return new CUEndUpdate();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CUEndUpdate";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CU_END_UPDATE;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const throw() {
        return 0;
    }
};


//--------------------------------------------------------------------------------
//
// class CUEndUpdateHandler;
//
//--------------------------------------------------------------------------------

class CUEndUpdateHandler {
public:
    // execute packet's handler
    static void execute(CUEndUpdate* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
