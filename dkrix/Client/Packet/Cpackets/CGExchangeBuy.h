//////////////////////////////////////////////////////////////////////////////
// Filename    : CGExchangeBuy.h
// Written By  : Exchange System
// Description : Client buys an item from the exchange
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_EXCHANGE_BUY_H__
#define __CG_EXCHANGE_BUY_H__

#include "../Packet.h"
#include "../PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGExchangeBuy;
//////////////////////////////////////////////////////////////////////////////

// Maximum idempotency key length on the wire. Must stay equal to the cap in
// dkrixserver/src/Core/CGExchangeBuy.cpp, which in turn matches USERINFO.sql's
// IdempotencyKey VARCHAR(64).
const uint szMaxIdempotencyKey = 64;

class CGExchangeBuy : public Packet
{
public:
	CGExchangeBuy();
	virtual ~CGExchangeBuy();

	void read(SocketInputStream& iStream);
	void write(SocketOutputStream& oStream) const;
	void execute(Player* pPlayer);

    // uint64 listingID + BYTE key length + key bytes.
    PacketSize_t getPacketSize() const {
        return sizeof(m_ListingID) + szBYTE + m_IdempotencyKey.size();
    }
    PacketID_t getPacketID() const {
        return PACKET_CG_EXCHANGE_BUY;
    }
    string getPacketName() const {
        return "CGExchangeBuy";
    }
    string toString() const;

	// Getters
    int64_t getListingID() const {
        return m_ListingID;
    }
    const string& getIdempotencyKey() const {
        return m_IdempotencyKey;
    }

    // Setters
    void setListingID(int64_t listingID) {
        m_ListingID = listingID;
    }
    void setIdempotencyKey(const string& key) {
        m_IdempotencyKey = key;
    }

private:
    int64_t m_ListingID;
    string m_IdempotencyKey; // empty means "server, generate one"
};

//////////////////////////////////////////////////////////////////////////////
// class CGExchangeBuyFactory;
//////////////////////////////////////////////////////////////////////////////

class CGExchangeBuyFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new CGExchangeBuy(); }
	string getPacketName() const throw() { return "CGExchangeBuy"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_EXCHANGE_BUY; }
    PacketSize_t getPacketMaxSize() const throw() {
        return sizeof(int64_t) + szBYTE + szMaxIdempotencyKey;
    }
};

#endif
