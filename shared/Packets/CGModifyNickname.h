//////////////////////////////////////////////////////////////////////////////
// Filename    : CGModifyNickname.h
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_MODIFY_NICKNAME_H__
#define __CG_MODIFY_NICKNAME_H__

#include "NicknameInfo.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGModifyNickname;
//////////////////////////////////////////////////////////////////////////////

class CGModifyNickname : public Packet {
public:
    CGModifyNickname();
    ~CGModifyNickname();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_MODIFY_NICKNAME;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE + m_Nickname.size();
    }
    string getPacketName() const {
        return "CGModifyNickname";
    }
    string toString() const;

public:
    ObjectID_t getItemObjectID() const {
        return m_ItemObjectID;
    }
    // Parameter widened from the server copy's WORD to ObjectID_t
    // (Phase 12, 2026-08-09). The member has always been ObjectID_t
    // (DWORD) and both sides read/write it at that width, so the WORD
    // parameter was a latent 32->16-bit truncation. It never bit the
    // server, which only ever read()s this request packet and calls the
    // setter nowhere; the client is the sole caller and passes
    // MItem::GetID(), a TYPE_OBJECTID (unsigned int). Adopting the
    // server's WORD here would have truncated every item object ID
    // above 65535.
    void setItemObjectID(ObjectID_t id) {
        m_ItemObjectID = id;
    }

    string getNickname() const {
        return m_Nickname;
    }
    void setNickname(const string& name) {
        m_Nickname = name;
    }

private:
    ObjectID_t m_ItemObjectID;
    string m_Nickname;
};

//////////////////////////////////////////////////////////////////////////////
// class CGModifyNicknameFactory;
//////////////////////////////////////////////////////////////////////////////

class CGModifyNicknameFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGModifyNickname();
    }
    string getPacketName() const throw() {
        return "CGModifyNickname";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_MODIFY_NICKNAME;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szBYTE + MAX_NICKNAME_SIZE;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGModifyNicknameHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGModifyNicknameHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGModifyNicknameHandler {
public:
    static void execute(CGModifyNickname* pPacket, Player* player);
};
#endif

#endif
