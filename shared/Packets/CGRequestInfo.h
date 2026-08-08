//--------------------------------------------------------------------------------
//
// Filename    : CGRequestInfo.h

//
//--------------------------------------------------------------------------------

#ifndef __CG_REQUEST_INFO_H__
#define __CG_REQUEST_INFO_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//--------------------------------------------------------------------------------
//
// class CGRequestInfo;
//
//--------------------------------------------------------------------------------

class CGRequestInfo : public Packet {
public:
    enum REQUEST_INFO_CODE {
        REQUEST_CHARACTER_INFO,

        REQUEST_INFO_MAX
    };

public:
    CGRequestInfo() {};
    virtual ~CGRequestInfo() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_REQUEST_INFO;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szBYTE + szuint;
    }

    // get packet name
    string getPacketName() const {
        return "CGRequestInfo";
    }

    // get packet's debug string
    string toString() const;

public:
    // get / set Code
    BYTE getCode() const {
        return m_Code;
    }
    void setCode(BYTE code) {
        m_Code = code;
    }

    // get / set Code
    uint getValue() const {
        return m_Value;
    }
    void setValue(uint value) {
        m_Value = value;
    }

private:
    // Code
    BYTE m_Code;
    uint m_Value;
};


//--------------------------------------------------------------------------------
//
// class CGRequestInfoFactory;
//
// Factory for CGRequestInfo
//
//--------------------------------------------------------------------------------

class CGRequestInfoFactory : public PacketFactory {
public:
    // create packet
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGRequestInfo();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGRequestInfo";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_REQUEST_INFO;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*

    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + szuint;
    }
};


//--------------------------------------------------------------------------------
//
// class CGRequestInfoHandler;
//
//--------------------------------------------------------------------------------

// Server-only: CGRequestInfoHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGRequestInfoHandler {
public:
    // execute packet's handler
    static void execute(CGRequestInfo* pPacket, Player* player);
};
#endif

#endif
