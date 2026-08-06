//////////////////////////////////////////////////////////////////////
//
// Filename    : GCWhisperFailed.h
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_WHISPER_FAILED_H__
#define __GC_WHISPER_FAILED_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCWhisperFailed;
//


//
//////////////////////////////////////////////////////////////////////

class GCWhisperFailed : public Packet {
public:
    // constructor
    GCWhisperFailed();

    // destructor
    ~GCWhisperFailed();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_WHISPER_FAILED;
    }

    // get packet's body size
    
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet's name
    string getPacketName() const {
        return "GCWhisperFailed";
    }

    // get packet's debug string
    string toString() const;

private:
};

//////////////////////////////////////////////////////////////////////
//
// class GCWhisperFailedFactory;
//
// Factory for GCWhisperFailed
//
//////////////////////////////////////////////////////////////////////

class GCWhisperFailedFactory : public PacketFactory {
public:
    // constructor
    GCWhisperFailedFactory() {}

    // destructor
    virtual ~GCWhisperFailedFactory() {}


public:
    // create packet
    Packet* createPacket() {
        return new GCWhisperFailed();
    }

    // get packet name
    string getPacketName() const {
        return "GCWhisperFailed";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_WHISPER_FAILED;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCWhisperFailedHandler;
//
//////////////////////////////////////////////////////////////////////

class GCWhisperFailedHandler {
public:
    // execute packet's handler
    static void execute(GCWhisperFailed* pGCWhisperFailed, Player* pPlayer);
};

#endif
