//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddItemToItemVerify.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_ADD_ITEM_TO_ITEM_VERIFY_H__
#define __GC_ADD_ITEM_TO_ITEM_VERIFY_H__

#include "Packet.h"
#include "PacketFactory.h"

enum {
    ADD_ITEM_TO_ITEM_VERIFY_ERROR, // error

    ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE,            
    ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_CRASH,            
    ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE,         
    ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK,                    
    ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_IMPOSSIBLE_NO_PREMIUM, 

    ADD_ITEM_TO_ITEM_VERIFY_TRANS_OK,
    ADD_ITEM_TO_ITEM_VERIFY_TRANS_IMPOSSIBLE,

    ADD_ITEM_TO_ITEM_VERIFY_MIXING_OK,
    ADD_ITEM_TO_ITEM_VERIFY_DETACHING_OK,

    ADD_ITEM_TO_ITEM_VERIFY_MIXING_FAILED_SAME_OPTION_GROUP, 
    ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL,                    

    ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK, 

    ADD_ITEM_TO_ITEM_VERIFY_CLEAR_OPTION_OK, 
    ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK,     

    // add by Coffee 2006.11.3
    ADD_ITEM_TO_ITEM_VERIFY_THREE_ENCHANT_OK,
    //
    ADD_ITEM_TO_ITEM_VERIFY_MAX
};

//////////////////////////////////////////////////////////////////////////////
// class GCAddItemToItemVerify
//////////////////////////////////////////////////////////////////////////////

class GCAddItemToItemVerify : public Packet {
public:
    GCAddItemToItemVerify() {
        m_Code = ADD_ITEM_TO_ITEM_VERIFY_MAX;
        m_Parameter = 0;
    }
    virtual ~GCAddItemToItemVerify() {}

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);

    PacketID_t getPacketID() const {
        return PACKET_GC_ADD_ITEM_TO_ITEM_VERIFY;
    }
    PacketSize_t getPacketSize() const;
    string getPacketName() const {
        return "GCAddItemToItemVerify";
    }
    string toString() const;

public:
    BYTE getCode(void) const {
        return m_Code;
    }
    void setCode(BYTE code) {
        m_Code = code;
    }

    uint getParameter(void) const {
        return m_Parameter;
    }

    void setParameter(uint parameter) {
        m_Parameter = parameter;
    }
    // add by Coffee 2006.11.3
    uint getParameter2(void) const {
        return m_Parameter2;
    }
    void setParameter2(uint parameter2) {
        m_Parameter2 = parameter2;
    }

private:
    BYTE m_Code;
    uint m_Parameter;
    
    uint m_Parameter2;
};


//////////////////////////////////////////////////////////////////////////////
// class GCAddItemToItemVerifyFactory;
//////////////////////////////////////////////////////////////////////////////

class GCAddItemToItemVerifyFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCAddItemToItemVerify();
    }
    string getPacketName() const {
        return "GCAddItemToItemVerify";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_ADD_ITEM_TO_ITEM_VERIFY;
    }
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + szuint + szuint;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class GCAddItemToItemVerifyHandler
//////////////////////////////////////////////////////////////////////////////

class GCAddItemToItemVerifyHandler {
public:
    static void execute(GCAddItemToItemVerify* pPacket, Player* pPlayer);
};

#endif
