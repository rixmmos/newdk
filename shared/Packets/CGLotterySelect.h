//////////////////////////////////////////////////////////////////////
//
// Filename    : CGLotterySelect.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_LOTTERY_SELECT_H__
#define __CG_LOTTERY_SELECT_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

enum {
    TYPE_SELECT_LOTTERY = 0, 
    TYPE_FINISH_SCRATCH,     
    TYPE_OVER_ENDING,        

    TYPE_MAX,
};


//////////////////////////////////////////////////////////////////////
//
// class CGLotterySelect;
//
//////////////////////////////////////////////////////////////////////

class CGLotterySelect : public Packet {
public:
    CGLotterySelect() {};
    ~CGLotterySelect() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_LOTTERY_SELECT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + szDWORD + szDWORD;
    }

    // get packet name
    string getPacketName() const {
        return "CGLotterySelect";
    }

    // get packet's debug string
    string toString() const;

    BYTE getType() const {
        return m_Type;
    }
    void setType(BYTE type) {
        m_Type = type;
    }

    DWORD getGiftID() const {
        return m_GiftID;
    }
    void setGiftID(DWORD GiftID) {
        m_GiftID = GiftID;
    }

    DWORD getQuestLevel() const {
        return m_QuestLevel;
    }
    void setQuestLevel(DWORD QuestLevel) {
        m_QuestLevel = QuestLevel;
    }

private:
    BYTE m_Type;
    DWORD m_QuestLevel;
    DWORD m_GiftID;
};


//////////////////////////////////////////////////////////////////////
//
// class CGLotterySelectFactory;
//
// Factory for CGLotterySelect
//
//////////////////////////////////////////////////////////////////////

class CGLotterySelectFactory : public PacketFactory {
public:
    // constructor
    CGLotterySelectFactory() {}

    // destructor
    virtual ~CGLotterySelectFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGLotterySelect();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGLotterySelect";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_LOTTERY_SELECT;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + szDWORD + szDWORD;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGLotterySelectHandler;
//
//////////////////////////////////////////////////////////////////////

// Unlike most Phase 12 pairs, this Handler class is NOT guarded by
// #ifndef __GAME_CLIENT__: dkrix/Client/CGHandlersStub.cpp already
// provides an empty-body client-side definition of
// CGLotterySelectHandler::execute (the "CGStoreOpen precedent"). Keeping
// it unconditional here means CGHandlersStub.cpp needs no structural
// change — only its #include and exception spec were updated.
class CGLotterySelectHandler {
public:
    // execute packet's handler
    static void execute(CGLotterySelect* pCGLotterySelect, Player* pPlayer);
};

#endif
