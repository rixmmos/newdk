//////////////////////////////////////////////////////////////////////////////
// Filename    : CLCreatePC.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLCreatePC.h"

namespace {

// Bounds-safe table lookup for toString(). SocketInputStream::readPacket()
// calls toString() on every packet it receives, so toString() must never be
// able to fault -- independently of what read() rejects, and independently of
// how a locally built packet was populated. "UNKNOWN" for an out-of-domain
// value matches the existing idiom in CGLearnSkill::toString().
template <size_t N> string enumName(const string (&table)[N], size_t index) {
    if (index >= N)
        return "UNKNOWN";

    return table[index];
}

} // namespace

void CLCreatePC::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    BYTE szName;

    iStream.read(szName);

    if (szName == 0)
        throw InvalidProtocolException("szName == 0");

    if (szName > 20)
        throw InvalidProtocolException("too long name length");

    iStream.read(m_Name, szName);

    BYTE slot;
    iStream.read(slot);

    // SECURITY: Slot defines three values but this is a raw wire BYTE, so
    // 0-255 arrives here and is indexed into the three-entry Slot2String[] by
    // toString() and by CLCreatePCHandler when it builds its SQL. Reject it at
    // the wire boundary, the same way the name length above is.
    if (slot >= SLOT_MAX)
        throw InvalidProtocolException("invalid slot");

    m_Slot = Slot(slot);

    BYTE flags;
    iStream.read(flags);

    // SECURITY: bit 0 carries the sex, bits 1-2 the hair style. getHairStyle()
    // masks those two bits and so yields 0-3, while HairStyle defines only
    // three styles and HairStyle2String[] holds three entries. There is no
    // fourth hair style -- the client picks from a three-element table
    // (UIMessageManager.cpp) and the DB decoders only know HAIR_STYLE1..3 --
    // so the fourth encoding is rejected here rather than the table widened.
    if (((flags >> SLAYER_BIT_HAIRSTYLE) & 3) > HAIR_STYLE3)
        throw InvalidProtocolException("invalid hair style");

    m_BitSet = flags;

    for (uint i = 0; i < SLAYER_COLOR_MAX; i++)
        iStream.read(m_Colors[i]);

    iStream.read(m_STR);
    iStream.read(m_DEX);
    iStream.read(m_INT);

    iStream.read(m_Race);

    __END_CATCH
}

void CLCreatePC::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    BYTE szName = m_Name.size();

    if (szName == 0)
        throw InvalidProtocolException("szName == 0");

    if (szName > 20)
        throw InvalidProtocolException("too long name length");

    oStream.write(szName);
    oStream.write(m_Name);

    oStream.write((BYTE)m_Slot);

    oStream.write((BYTE)m_BitSet.to_ulong());

    for (uint i = 0; i < SLAYER_COLOR_MAX; i++)
        oStream.write(m_Colors[i]);

    oStream.write(m_STR);
    oStream.write(m_DEX);
    oStream.write(m_INT);

    oStream.write(m_Race);

    __END_CATCH
}

void CLCreatePC::execute(Player* pPlayer)

{
    __BEGIN_TRY

    CLCreatePCHandler::execute(this, pPlayer);

    __END_CATCH
}

string CLCreatePC::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "CLCreatePC(Name: " << m_Name << ",Slot:" << enumName(Slot2String, m_Slot) << ",Sex:" << Sex2String[getSex()]
        << ",HairStyle:" << enumName(HairStyle2String, getHairStyle()) << ",HairColor:" << (int)getHairColor()
        << ",SkinColor:" << (int)getSkinColor() << ",STR:" << (int)m_STR << ",DEX:" << (int)m_DEX
        << ",INT:" << (int)m_INT << ",Race:" << (int)m_Race << ")";
    return msg.toString();

    __END_CATCH
}
