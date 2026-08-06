//////////////////////////////////////////////////////////////////////
//
// Filename    : GCReconnect.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_RECONNECT_H__
#define __GC_RECONNECT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCReconnect;
//



//
//////////////////////////////////////////////////////////////////////

class GCReconnect : public Packet {
public:
    GCReconnect() {};
    ~GCReconnect() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_RECONNECT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Name.size()       
               + szPCType                   
               + szBYTE + m_ServerIP.size() 
               + szDWORD;                   
    }

    // get packet name
    string getPacketName() const {
        return "GCReconnect";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set creature name
    string getName() const {
        return m_Name;
    }
    void setName(const string& name) {
        m_Name = name;
    }

    // get/set pc type
    PCType getPCType() const {
        return m_PCType;
    }
    void setPCType(PCType pcType) {
        m_PCType = pcType;
    }

    // get/set server ip
    string getServerIP() const {
        return m_ServerIP;
    }
    void setServerIP(const string& serverIP) {
        m_ServerIP = serverIP;
    }

    // get/set key
    DWORD getKey() const {
        return m_Key;
    }
    void setKey(DWORD key) {
        m_Key = key;
    }

private:
    // creature name
    string m_Name;

    // pc type
    PCType m_PCType;

    // New Server IP
    string m_ServerIP;

    // authentication key
    DWORD m_Key;
};


//////////////////////////////////////////////////////////////////////
//
// class GCReconnectFactory;
//
// Factory for GCReconnect
//
//////////////////////////////////////////////////////////////////////

class GCReconnectFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCReconnect();
    }

    // get packet name
    string getPacketName() const {
        return "GCReconnect";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_RECONNECT;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 20   
               + szPCType    
               + szBYTE + 15 
               + szDWORD;    
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCReconnectHandler;
//
//////////////////////////////////////////////////////////////////////

class GCReconnectHandler {
public:
    // execute packet's handler
    static void execute(GCReconnect* pPacket, Player* pPlayer);
};

#endif
