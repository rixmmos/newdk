//////////////////////////////////////////////////////////////////////
//
// Filename    : ServerGroupInfo.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __SERVER_GROUP_INFO_H__
#define __SERVER_GROUP_INFO_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class ServerGroupInfo;
//

//
//////////////////////////////////////////////////////////////////////

class ServerGroupInfo {
public:
    // constructor
    ServerGroupInfo();

    // destructor
    ~ServerGroupInfo() noexcept;

public:
    
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // get packet's body size
    
    PacketSize_t getSize();

    static uint getMaxSize() {
        return (szBYTE + szBYTE + 20 + szBYTE) * 37;
    }

    // get packet's debug string
    string toString() const;

    // get / set GroupID
    BYTE getGroupID() const {
        return m_GroupID;
    }
    void setGroupID(ServerGroupID_t GroupID) {
        m_GroupID = GroupID;
    }

    // get / set GroupName
    string getGroupName() const {
        return m_GroupName;
    }
    void setGroupName(string GroupName) {
        m_GroupName = GroupName;
    }

    // get / set Group Stat
    BYTE getStat() const {
        return m_Stat;
    }
    void setStat(BYTE Stat) {
        m_Stat = Stat;
    }

private:
    
    ServerGroupID_t m_GroupID;

    
    string m_GroupName;

    
    BYTE m_Stat;
};

#endif
