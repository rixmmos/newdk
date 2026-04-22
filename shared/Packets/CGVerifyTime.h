//////////////////////////////////////////////////////////////////////
//
// Filename    : CGVerifyTime.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_VERIFY_TIME_H__
#define __CG_VERIFY_TIME_H__

#include "Packet.h"
#include "PacketFactory.h"

class Player;
class GamePlayer;

class CGVerifyTime : public Packet {
public:
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_VERIFY_TIME;
    }

    PacketSize_t getPacketSize() const throw() {
        return 0;
    }

    std::string getPacketName() const throw() {
        return "CGVerifyTime";
    }

    std::string toString() const throw();
};

class CGVerifyTimeFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGVerifyTime();
    }

    std::string getPacketName() const throw() {
        return "CGVerifyTime";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_VERIFY_TIME;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return 0;
    }
};

#ifndef __GAME_CLIENT__
class CGVerifyTimeHandler {
public:
    static void execute(CGVerifyTime* pPacket, Player* pPlayer) throw(ProtocolException, Error);

#ifdef __GAME_SERVER__
    static void saveSpeedHackPlayer(Player* pPlayer) throw(ProtocolException, Error);
#endif
};
#endif

#endif
