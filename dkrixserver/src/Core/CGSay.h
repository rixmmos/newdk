//////////////////////////////////////////////////////////////////////
//
// Filename    : CGSay.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SAY_H__
#define __CG_SAY_H__

// include files

// #ifdef __GAME_SERVER__
// #include "GamePlayer.h"
// #endif

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGSay;
//


//
//////////////////////////////////////////////////////////////////////

class Player;
class Creature;
class GamePlayer;

class CGSay : public Packet {
public:
    CGSay() {};
    virtual ~CGSay() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_SAY;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szuint + szBYTE + m_Message.size();
    }

    // get packet name
    string getPacketName() const {
        return "CGSay";
    }

    // get packet's debug string
    string toString() const;

    // get/set text color
    uint getColor() const {
        return m_Color;
    }
    void setColor(uint color) {
        m_Color = color;
    }

    // get/set chatting message
    const string& getMessage() const {
        return m_Message;
    }
    void setMessage(const string& msg) {
        m_Message = msg;
    }


private:
    // text color
    uint m_Color;

    // chatting message
    string m_Message;
};


//////////////////////////////////////////////////////////////////////
//
// class CGSayFactory;
//
// Factory for CGSay
//
//////////////////////////////////////////////////////////////////////

class CGSayFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGSay();
    }

    // get packet name
    string getPacketName() const {
        return "CGSay";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_SAY;
    }

    // get packet's max body size
    
    PacketSize_t getPacketMaxSize() const {
        return szuint + szBYTE + 128;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGSayHandler;
//
//////////////////////////////////////////////////////////////////////

class CGSayHandler {
public:
    // execute packet's handler
    static void execute(CGSay* pPacket, Player* pPlayer);

    static void opExecute(Creature* pCreature, GamePlayer* pPlayer, string msg, int i);

    // for guild test
    static void opzone(string msg, int i);
    static void opguild(string msg, int i, Creature* pCreature);

    
    static void opcombat(GamePlayer* pPlayer, string msg, int i);

    
    static void opset(GamePlayer* pPlayer, string msg, int i);

    static void opview(GamePlayer* pPlayer, string msg, int i);

    // save
    static void opsave(GamePlayer* pPlayer, string msg, int i);

    // wall
    static void opwall(GamePlayer* pPlayer, string msg, int i);

    // Shutdown
    static void opshutdown(GamePlayer* pPlayer, string msg, int i);

    // kick
    static void opkick(GamePlayer* pPlayer, string msg, int i);

    // mute
    static void opmute(GamePlayer* pPlayer, string msg, int i);

    // freezing
    static void opfreezing(GamePlayer* pPlayer, string msg, int i);

    // deny
    static void opdeny(GamePlayer* pPlayer, string msg, int i);

    // info
    static void opinfo(GamePlayer* pPlayer, string msg, int i);

    // trace
    static void optrace(GamePlayer* pPlayer, string msg, int i);

    // warp
    static void opwarp(GamePlayer* pPlayer, string msg, int i);

    // create
    static void opcreate(GamePlayer* pPlayer, string msg, int i);

    // grant
    static void opgrant(GamePlayer* pPlayer, string msg, int i);

    // recall
    static void oprecall(GamePlayer* pPlayer, string msg, int i);

    // mrecall
    static void opmrecall(GamePlayer* pPlayer, string msg, int i);

    // user
    static void opuser(GamePlayer* pPlayer, string msg, int i);

    // summon
    static void opsummon(GamePlayer* pPlayer, string msg, int i);

    // notice
    static void opnotice(GamePlayer* pPlayer, string msg, int i);

    // pay
    static void oppay(GamePlayer* pPlayer, string msg, int i);

    // world
    static void opworld(GamePlayer* pPlayer, string msg, int i, bool bSameWorldOnly);

    // command
    static void opcommand(GamePlayer* pPlayer, string msg, int i);

    // find
    static void opfind(GamePlayer* pPlayer, string msg, int i);

    // refresh info
    static void opload(GamePlayer* pPlayer, string msg, int i);

    
    static void opsoulchain(GamePlayer* pPlayer, string msg, int i);

    // credit
    static void opcredit(GamePlayer* pPlayer, string msg, int i);

    // fun
    static void opfun(GamePlayer* pPlayer, string msg, int i);

    // log
    static void oplog(GamePlayer* pPlayer, string msg, int i);

    // denychat
    static void opdenychat(GamePlayer* pPlayer, string msg, int i);
    // world
    static void oprefreshguildunion(GamePlayer* pPlayer, string msg, int i, bool bSameWorldOnly);
    static void opmodifyunioninfo(GamePlayer* pPlayer, string msg, int i, bool bSameWorldOnly);
};

#endif
