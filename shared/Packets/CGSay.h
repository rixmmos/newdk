//////////////////////////////////////////////////////////////////////
//
// Filename    : CGSay.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SAY_H__
#define __CG_SAY_H__

#include "Packet.h"
#include "PacketFactory.h"

class Player;
class Creature;
class GamePlayer;

class CGSay : public Packet {
public:
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_SAY;
    }

    PacketSize_t getPacketSize() const throw() {
        return szuint + szBYTE + m_Message.size();
    }

    std::string getPacketName() const throw() {
        return "CGSay";
    }

    std::string toString() const throw();

    uint getColor() const throw() {
        return m_Color;
    }

    void setColor(uint color) throw() {
        m_Color = color;
    }

    const std::string& getMessage() const throw() {
        return m_Message;
    }

    void setMessage(const std::string& msg) throw() {
        m_Message = msg;
    }

private:
    uint m_Color;
    std::string m_Message;
};

class CGSayFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGSay();
    }

    std::string getPacketName() const throw() {
        return "CGSay";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SAY;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szuint + szBYTE + 128;
    }
};

#ifndef __GAME_CLIENT__
class CGSayHandler {
public:
    static void execute(CGSay* pPacket, Player* pPlayer) throw(ProtocolException, Error);

#ifdef __GAME_SERVER__
    static void opExecute(Creature* pCreature, GamePlayer* pPlayer, std::string msg, int i);
    static void opzone(std::string msg, int i);
    static void opguild(std::string msg, int i, Creature* pCreature);
    static void opcombat(GamePlayer* pPlayer, std::string msg, int i);
    static void opset(GamePlayer* pPlayer, std::string msg, int i);
    static void opview(GamePlayer* pPlayer, std::string msg, int i);
    static void opsave(GamePlayer* pPlayer, std::string msg, int i);
    static void opwall(GamePlayer* pPlayer, std::string msg, int i);
    static void opshutdown(GamePlayer* pPlayer, std::string msg, int i);
    static void opkick(GamePlayer* pPlayer, std::string msg, int i);
    static void opmute(GamePlayer* pPlayer, std::string msg, int i);
    static void opfreezing(GamePlayer* pPlayer, std::string msg, int i);
    static void opdeny(GamePlayer* pPlayer, std::string msg, int i);
    static void opinfo(GamePlayer* pPlayer, std::string msg, int i);
    static void optrace(GamePlayer* pPlayer, std::string msg, int i);
    static void opwarp(GamePlayer* pPlayer, std::string msg, int i);
    static void opcreate(GamePlayer* pPlayer, std::string msg, int i);
    static void opgrant(GamePlayer* pPlayer, std::string msg, int i);
    static void oprecall(GamePlayer* pPlayer, std::string msg, int i);
    static void opmrecall(GamePlayer* pPlayer, std::string msg, int i);
    static void opuser(GamePlayer* pPlayer, std::string msg, int i);
    static void opsummon(GamePlayer* pPlayer, std::string msg, int i);
    static void opnotice(GamePlayer* pPlayer, std::string msg, int i);
    static void oppay(GamePlayer* pPlayer, std::string msg, int i);
    static void opworld(GamePlayer* pPlayer, std::string msg, int i, bool bSameWorldOnly);
    static void opcommand(GamePlayer* pPlayer, std::string msg, int i);
    static void opfind(GamePlayer* pPlayer, std::string msg, int i);
    static void opload(GamePlayer* pPlayer, std::string msg, int i);
    static void opsoulchain(GamePlayer* pPlayer, std::string msg, int i);
    static void opcredit(GamePlayer* pPlayer, std::string msg, int i);
    static void opfun(GamePlayer* pPlayer, std::string msg, int i);
    static void oplog(GamePlayer* pPlayer, std::string msg, int i);
    static void opdenychat(GamePlayer* pPlayer, std::string msg, int i);
    static void oprefreshguildunion(GamePlayer* pPlayer, std::string msg, int i, bool bSameWorldOnly);
    static void opmodifyunioninfo(GamePlayer* pPlayer, std::string msg, int i, bool bSameWorldOnly);
#endif
};
#endif

#endif
