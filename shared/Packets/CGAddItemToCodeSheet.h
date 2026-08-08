//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAddItemToCodeSheet.h
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_ITEM_TO_CODE_SHEET_H__
#define __CG_ADD_ITEM_TO_CODE_SHEET_H__

#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////////////
// class CGAddItemToCodeSheet;
//////////////////////////////////////////////////////////////////////////////

class CGAddItemToCodeSheet : public Packet {
public:
    CGAddItemToCodeSheet() {};
    ~CGAddItemToCodeSheet() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_ADD_ITEM_TO_CODE_SHEET;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szCoordInven + szCoordInven;
    }
    string getPacketName() const {
        return "CGAddItemToCodeSheet";
    }
    string toString() const;

public:
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    CoordInven_t getX() const {
        return m_X;
    }
    void setX(Coord_t X) {
        m_X = X;
    }

    CoordInven_t getY() const {
        return m_Y;
    }
    void setY(Coord_t Y) {
        m_Y = Y;
    }

private:
    ObjectID_t m_ObjectID; // ObjectID
    CoordInven_t m_X;      // Coord X
    CoordInven_t m_Y;      // Coord Y
};

//////////////////////////////////////////////////////////////////////////////
// class CGAddItemToCodeSheetFactory;
//////////////////////////////////////////////////////////////////////////////

class CGAddItemToCodeSheetFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGAddItemToCodeSheet();
    }
    string getPacketName() const throw() {
        return "CGAddItemToCodeSheet";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_ADD_ITEM_TO_CODE_SHEET;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szCoordInven + szCoordInven;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGAddItemToCodeSheetHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGAddItemToCodeSheetHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGAddItemToCodeSheetHandler {
public:
    static void execute(CGAddItemToCodeSheet* pCGAddItemToCodeSheet, Player* pPlayer);
};
#endif

#endif
