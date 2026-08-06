//--------------------------------------------------------------------------------
//
// Filename    : CLLogin.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __CL_LOGIN_H__
#define __CL_LOGIN_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CLLogin;
//


//
//--------------------------------------------------------------------------------


#define MAX_LENGTH_MAC 6


class CLLogin : public Packet {
public:
    CLLogin() : m_LoginMode(LOGIN_MODE_NORMAL) {
        m_strMacAddress = "";
    }
    virtual ~CLLogin() {};

public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CL_LOGIN;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const;

    // get packet name
    string getPacketName() const {
        return "CLLogin";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set player's id
    string getID() const {
        return m_ID;
    }
    void setID(string id) {
        m_ID = id;
    }

    // get/set player's password
    string getPassword() const {
        return m_Password;
    }
    void setPassword(string password) {
        m_Password = password;
    }

    // get/set Cpsso imformation
    bool isNetmarble() const {
        return m_bNetmarble;
    }
    void setNetmarble(bool netmarble) {
        m_bNetmarble = netmarble;
    }

    bool isAdult() const {
        return m_bAdult;
    }
    void setAdult(bool adult) {
        m_bAdult = adult;
    }

    // add - inthesky
    bool checkMacAddress(string currentMac) const;
    string getMacAddress() const {
        return m_strMacAddress;
    }

    const BYTE* getRareMacAddress() const {
        return m_cMacAddress;
    }

    void setWebLogin() {
        m_LoginMode = LOGIN_MODE_WEBLOGIN;
    }
    bool isWebLogin() const {
        return m_LoginMode == LOGIN_MODE_WEBLOGIN;
    }

private:
    
    string m_ID;

    
    string m_Password;

    
    
    bool m_bNetmarble;
    bool m_bAdult;

    BYTE m_cMacAddress[6];
    string m_strMacAddress;

    BYTE m_LoginMode;
};


//--------------------------------------------------------------------------------
//
// class CLLoginFactory;
//
// Factory for CLLogin
//
//--------------------------------------------------------------------------------

class CLLoginFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CLLogin();
    }

    // get packet name
    string getPacketName() const {
        return "CLLogin";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CL_LOGIN;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szint + 2048 + szBYTE + 30 + 6 + szBYTE;
    }
};


//--------------------------------------------------------------------------------
//
// class CLLoginHandler;
//
//--------------------------------------------------------------------------------

class CLLoginHandler {
public:
    // execute packet's handler
    static void execute(CLLogin* pPacket, Player* pPlayer);

private:
    static bool checkFreePass(CLLogin* pPacket, Player* pPlayer);
    static bool checkNetMarbleClient(CLLogin* pPacket, Player* pPlayer);
    static bool checkWebLogin(CLLogin* pPacket, Player* pPlayer);
#ifdef __THAILAND_SERVER__
    static bool onChildGuardTimeArea(int pm, int am, string enable);
#endif
};

#endif
