//////////////////////////////////////////////////////////////////////////////
// Filename    : CGExchangeBuy.cpp
// Written By  : Exchange System
// Description : Client buys an item from the exchange
//////////////////////////////////////////////////////////////////////////////

#include "CGExchangeBuy.h"
#include "../SocketInputStream.h"
#include "../SocketOutputStream.h"

#include "../Player.h"

// Wire format, shared with dkrixserver/src/Core/CGExchangeBuy.cpp:
//
//   uint64  listingID
//   BYTE    keyLength      (0..64; 0 means "server, generate one")
//   char[]  idempotencyKey (keyLength bytes, no terminator)
//
// The client previously sent a bare 4-byte uint listing ID and no key at all,
// so the two sides never agreed and this packet could not have worked. The
// server side was hardened first (18-H / 18-M); this is the matching half.
//
// SocketInputStream/SocketOutputStream carry no 64-bit overload on this side,
// so the listing ID goes through the raw byte form. That is byte-for-byte what
// the server's read<T>()/write<T>() templates do -- they copy sizeof(T) bytes
// in host order -- on every platform either tree targets.

CGExchangeBuy::CGExchangeBuy()
{
	m_ListingID = 0;
}

CGExchangeBuy::~CGExchangeBuy()
{
}

void CGExchangeBuy::read(SocketInputStream& iStream)
{
	__BEGIN_TRY

    uint64_t listingID = 0;
    iStream.read((char*)&listingID, (uint)sizeof(listingID));
    m_ListingID = (int64_t)listingID;

    BYTE szKey;
    iStream.read(szKey);

    if ((uint)szKey > szMaxIdempotencyKey)
        throw InvalidProtocolException("CGExchangeBuy: idempotency key too long");

    m_IdempotencyKey.clear();

    if (szKey > 0)
        iStream.read(m_IdempotencyKey, szKey);

    __END_CATCH
}

void CGExchangeBuy::write(SocketOutputStream& oStream) const
{
	__BEGIN_TRY

    // Check the real size before narrowing to BYTE. Narrowing first would let a
    // 256-byte key truncate to 0, sail past the cap, and leave write() emitting
    // one byte while getPacketSize() reports 1 + 256.
    if (m_IdempotencyKey.size() > szMaxIdempotencyKey)
        throw InvalidProtocolException("CGExchangeBuy: idempotency key too long");

    const uint64_t listingID = (uint64_t)m_ListingID;
    oStream.write((const char*)&listingID, (uint)sizeof(listingID));

    BYTE szKey = (BYTE)m_IdempotencyKey.size();

    oStream.write(szKey);

    if (szKey > 0)
        oStream.write(m_IdempotencyKey);

    __END_CATCH
}

string CGExchangeBuy::toString() const
{
	StringStream msg;
    // StringStream has no 64-bit overload; the cast mirrors the server's
    // toString() and only affects the debug text, not the wire.
    msg << "CGExchangeBuy("
        << "ListingID:" << (int)m_ListingID << ",IdempotencyKey:" << m_IdempotencyKey << ")";
    return msg.toString();
}

void CGExchangeBuy::execute(Player* pPlayer)
{
	__BEGIN_TRY

	// This is handled by the server's packet handler
	// The client doesn't execute this packet

	__END_CATCH
}
