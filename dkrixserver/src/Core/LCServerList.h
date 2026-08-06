//////////////////////////////////////////////////////////////////////
//
// Filename    : LCServerList.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LC_SERVER_LIST_H__
#define __LC_SERVER_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"
#include "ServerGroupInfo.h"

//////////////////////////////////////////////////////////////////////
//
// class LCServerList;
//
//////////////////////////////////////////////////////////////////////

class LCServerList : public Packet {
public:
    // constructor
    
    LCServerList();

    // destructor
    
    ~LCServerList() noexcept;

    
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LC_SERVER_LIST;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const;

    // get packet's name
    string getPacketName() const {
        return "LCServerList";
    }

    // get packet's debug string
    string toString() const;

public:
    
    ServerGroupID_t getCurrentServerGroupID() const {
        return m_CurrentServerGroupID;
    }
    void setCurrentServerGroupID(ServerGroupID_t ServerGroupID) {
        m_CurrentServerGroupID = ServerGroupID;
    }

    BYTE getListNum() const {
        return m_ServerGroupInfoList.size();
    }

    // add / delete / clear S List
    void addListElement(ServerGroupInfo* pServerGroupInfo) {
        m_ServerGroupInfoList.push_back(pServerGroupInfo);
    }

    // ClearList
    void clearList() {
        m_ServerGroupInfoList.clear();
    }

    // pop front Element in Status List
    ServerGroupInfo* popFrontListElement() {
        ServerGroupInfo* TempServerGroupInfo = m_ServerGroupInfoList.front();
        m_ServerGroupInfoList.pop_front();
        return TempServerGroupInfo;
    }

private:
    
    ServerGroupID_t m_CurrentServerGroupID;

    
    list<ServerGroupInfo*> m_ServerGroupInfoList;
};

//////////////////////////////////////////////////////////////////////
//
// class LCServerListFactory;
//
// Factory for LCServerList
//
//////////////////////////////////////////////////////////////////////

class LCServerListFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LCServerList();
    }

    // get packet name
    string getPacketName() const {
        return "LCServerList";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LC_SERVER_LIST;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        
        
        return szServerGroupID + ServerGroupInfo::getMaxSize();
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCServerListHandler;
//
//////////////////////////////////////////////////////////////////////

class LCServerListHandler {
public:
    // execute packet's handler
    static void execute(LCServerList* pPacket, Player* pPlayer);
};

#endif
