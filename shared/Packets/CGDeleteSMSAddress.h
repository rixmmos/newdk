//////////////////////////////////////////////////////////////////////////////
// Filename    : CGDeleteSMSAddress.h
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_DELETE_SMS_ADDRESS_H__
#define __CG_DELETE_SMS_ADDRESS_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGDeleteSMSAddress;
//////////////////////////////////////////////////////////////////////////////

class CGDeleteSMSAddress : public Packet {
public:
    CGDeleteSMSAddress() throw();
    ~CGDeleteSMSAddress() throw();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_DELETE_SMS_ADDRESS;
    }
    PacketSize_t getPacketSize() const throw() {
        return szDWORD;
    }
    string getPacketName() const throw() {
        return "CGDeleteSMSAddress";
    }
    string toString() const;

public:
    DWORD getElementID() const throw() {
        return m_ElementID;
    }
    void setElementID(DWORD eID) throw() {
        m_ElementID = eID;
    }

private:
    DWORD m_ElementID;
};

//////////////////////////////////////////////////////////////////////////////
// class CGDeleteSMSAddressFactory;
//////////////////////////////////////////////////////////////////////////////

class CGDeleteSMSAddressFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGDeleteSMSAddress();
    }
    string getPacketName() const throw() {
        return "CGDeleteSMSAddress";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_DELETE_SMS_ADDRESS;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szDWORD;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGDeleteSMSAddressHandler;
//////////////////////////////////////////////////////////////////////////////

class CGDeleteSMSAddressHandler {
public:
    static void execute(CGDeleteSMSAddress* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
