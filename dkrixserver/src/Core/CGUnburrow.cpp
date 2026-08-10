//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUnburrow.cpp
// Written By  : crazydog
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGUnburrow.h"

void CGUnburrow::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    iStream.read(m_X);
    iStream.read(m_Y);
    iStream.read(m_Dir);

    __END_CATCH
}

void CGUnburrow::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    oStream.write(m_X);
    oStream.write(m_Y);
    oStream.write(m_Dir);

    __END_CATCH
}

void CGUnburrow::execute(Player* pPlayer)

{
    __BEGIN_TRY

    CGUnburrowHandler::execute(this, pPlayer);

    __END_CATCH
}

string CGUnburrow::toString() const

{
    __BEGIN_TRY

    // SECURITY: Dir2String[] holds eight entries (LEFT..LEFTUP) but m_Dir is a
    // raw wire BYTE, so 0-255 reaches this lookup -- and toString() is called by
    // SocketInputStream::readPacket() on every packet received, before any
    // handler runs. Only the lookup is bounded, not read(): DIR_NONE aliases
    // DIR_MAX, so 8 is a live sentinel elsewhere in the tree and the client's
    // direction range could not be proved to exclude it. See the note in
    // MODERNIZATION.md -- the unburrow path itself does not bound dir either
    // (addUnburrowCreature -> Creature::setXYDir stores it verbatim), so this
    // closes the readPacket() fault but not the persistence. "UNKNOWN" matches
    // CGConnect::toString().
    string dir = "UNKNOWN";
    if (m_Dir < DIR_MAX)
        dir = Dir2String[m_Dir];

    StringStream msg;
    msg << "CGUnburrow("
        << "X:" << (int)m_X << ",Y:" << (int)m_Y << ",Dir:" << dir << ")";
    return msg.toString();

    __END_CATCH
}
