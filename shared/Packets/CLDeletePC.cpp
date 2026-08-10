//////////////////////////////////////////////////////////////////////////////
// Filename    : CLDeletePC.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLDeletePC.h"

void CLDeletePC::read(SocketInputStream& iStream)

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
    // 0-255 arrives here. CLDeletePCHandler indexes the three-entry
    // Slot2String[] with the unvalidated getSlot() at six sites (:73, :80,
    // :105, :111, :123, :129) and binds the result into SQL. Reject it at the
    // wire boundary, the same way the name length above is. This is the twin
    // of CLCreatePC's check (18-T), which did not cover this packet.
    if (slot >= SLOT_MAX)
        throw InvalidProtocolException("invalid slot");

    m_Slot = Slot(slot);

    BYTE szSSN;
    iStream.read(szSSN);

    if (szSSN == 0)
        throw InvalidProtocolException("szSSN == 0");

    if (szSSN > 14)
        throw InvalidProtocolException("too long name length");

    iStream.read(m_SSN, szSSN);

    __END_CATCH
}

void CLDeletePC::write(SocketOutputStream& oStream) const

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

    BYTE szSSN = m_SSN.size();

    if (szSSN == 0)
        throw InvalidProtocolException("szSSN == 0");

    if (szSSN > 14)
        throw InvalidProtocolException("too long name length");

    oStream.write(szSSN);
    oStream.write(m_SSN);

    __END_CATCH
}

void CLDeletePC::execute(Player* pPlayer)

{
    __BEGIN_TRY

#ifndef __GAME_CLIENT__
    CLDeletePCHandler::execute(this, pPlayer);
#endif

    __END_CATCH
}

string CLDeletePC::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "CLDeletePC(Name:" << m_Name << ",Slot:" << (int)m_Slot << ",SSN:" << m_SSN << ")";
    return msg.toString();

    __END_CATCH
}
