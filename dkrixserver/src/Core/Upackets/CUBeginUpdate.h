//--------------------------------------------------------------------------------
//
// Filename    : CUBeginUpdate.h
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __CU_BEGIN_UPDATE_H__
#define __CU_BEGIN_UPDATE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"
#include "UpdateManager.h"

//--------------------------------------------------------------------------------
//
// class CUBeginUpdate;
//


//
//--------------------------------------------------------------------------------

class CUBeginUpdate : public Packet {
public:
    enum TYPE { OLD_UPDATE, ONE_VERSION_KEY_BACK, TWO_VERSION_KEY_FRONT };

public:
    
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);

    
    void read(Socket* pSocket) throw(ProtocolException, Error);

    
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);

    // execute packet's handler
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    // get packet id
    PacketID_t getPacketID() const throw() {
        return PACKET_CU_BEGIN_UPDATE;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const throw() {
        return szWORD + szWORD + szWORD;
    }

    //
    static PacketSize_t getPacketMaxSize() throw() {
        return szWORD + szWORD + szWORD;
    }

    // get packet name
    string getPacketName() const throw() {
        return "CUBeginUpdate";
    }

    // get packet's debug string
    string toString() const throw();

public:
    // get/set client version
    WORD getVersion() const throw() {
        return m_Version;
    }
    void setVersion(WORD version) throw() {
        m_Version = version;
    }

    // get/set client version
    WORD getGuildVersion() const throw() {
        return m_GuildVersion;
    }
    void setGuildVersion(WORD version) throw() {
        m_GuildVersion = version;
    }

    WORD getInfoVersion() const throw() {
        return m_InfoVersion;
    }
    void setInfoVersion(WORD version) throw() {
        m_InfoVersion = version;
    }

    TYPE getType() const {
        return m_Type;
    }

private:
    
    WORD m_Version;
    
    WORD m_GuildVersion;
    
    WORD m_InfoVersion;

    TYPE m_Type;
};


//--------------------------------------------------------------------------------
//
// class CUBeginUpdateFactory;
//
// Factory for CUBeginUpdate
//
//--------------------------------------------------------------------------------

class CUBeginUpdateFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() throw() {
        return new CUBeginUpdate();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CUBeginUpdate";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CU_BEGIN_UPDATE;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const throw() {
        return szWORD + szWORD + szWORD;
    }
};


//--------------------------------------------------------------------------------
//
// class CUBeginUpdateHandler;
//
//--------------------------------------------------------------------------------

class CUBeginUpdateHandler {
public:
    // execute packet's handler
    static void execute(CUBeginUpdate* pPacket, Player* pPlayer) throw(ProtocolException, Error);
    static void scan_Dir(const string Directory, CUBeginUpdate* pPacket, UpdateManager* pUpdateManager, bool bHttpPatch,
                         bool bUpdaterPatch) throw(ProtocolException, Error);
};

#endif
