//----------------------------------------------------------------------
//
// Filename    : ReconnectLoginInfo.h
// Written by  : reiot@ewestsoft.com
// Description :
//
//----------------------------------------------------------------------

#ifndef __RECONNECT_LOGIN_INFO_H__
#define __RECONNECT_LOGIN_INFO_H__

// include files
#include "Exception.h"
#include "StringStream.h"
#include "Timeval.h"
#include "Types.h"

//----------------------------------------------------------------------
//
// class ReconnectLoginInfo;
//

//
//----------------------------------------------------------------------

class ReconnectLoginInfo {
public:
    // constructor
    ReconnectLoginInfo() throw() : m_Key(0) {
        m_ExpireTime.tv_sec = 0;
        m_ExpireTime.tv_usec = 0;
    }

    // destructor
    ~ReconnectLoginInfo() throw() {}

    // get/set client ip
    string getClientIP() const throw() {
        return m_ClientIP;
    }
    void setClientIP(const string& clientIP) throw() {
        m_ClientIP = clientIP;
    }

    // get/set PlayerID
    string getPlayerID() const throw() {
        return m_PlayerID;
    }
    void setPlayerID(const string& PlayerID) throw() {
        m_PlayerID = PlayerID;
    }

    // get/set expire time
    Timeval getExpireTime() const throw() {
        return m_ExpireTime;
    }
    void setExpireTime(Timeval tv) throw() {
        m_ExpireTime = tv;
    }

    // get/set validation key
    DWORD getKey() const throw() {
        return m_Key;
    }
    void setKey(DWORD key) throw() {
        m_Key = key;
    }

    // get debug string
    string toString() const throw() {
        StringStream msg;
        msg << "ReconnectLoginInfo(ClientIP:" << m_ClientIP << ",ExpireTime:" << m_ExpireTime.tv_sec << "."
            << m_ExpireTime.tv_usec << ",KEY: " << m_Key << ")";
        return msg.toString();
    }

private:
    // valid client's ip
    string m_ClientIP;

    // PlayerID
    string m_PlayerID;

    
    Timeval m_ExpireTime;

    // validation key
    DWORD m_Key;
};

//----------------------------------------------------------------------
//
// CompareReconnectLoginInfo
//

//
// *CAUTION*
//



//
//----------------------------------------------------------------------
class CompareReconnectLoginInfo {
public:
    // compare which is more recent
    bool operator()(const ReconnectLoginInfo& left, const ReconnectLoginInfo& right) {
        // Ascending Order
        return left.getExpireTime() > right.getExpireTime();

        // Descending Order
        // return left.getExpireTime() < right.getExpireTime();
    }
};

#endif
