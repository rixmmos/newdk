//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMove.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGMove.h"

#include "Assert1.h"
#include "SocketEncryptInputStream.h"
#include "SocketEncryptOutputStream.h"


void CGMove::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

#ifdef __USE_ENCRYPTER__
    SocketEncryptInputStream* pEIStream = dynamic_cast<SocketEncryptInputStream*>(&iStream);
    Assert(pEIStream != NULL);

    if (pEIStream->getEncryptCode() != 0) {
        SHUFFLE_STATEMENT_3(pEIStream->getEncryptCode(), pEIStream->readEncrypt(m_X), pEIStream->readEncrypt(m_Y),
                            pEIStream->readEncrypt(m_Dir));
    } else
#endif
    {
        iStream.read(m_Dir);
        iStream.read(m_X);
        iStream.read(m_Y);
    }

    __END_CATCH
}

void CGMove::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

#ifdef __USE_ENCRYPTER__
    SocketEncryptOutputStream* pEOStream = dynamic_cast<SocketEncryptOutputStream*>(&oStream);
    Assert(pEOStream != NULL);

    if (pEOStream->getEncryptCode() != 0) {
        SHUFFLE_STATEMENT_3(pEOStream->getEncryptCode(), pEOStream->writeEncrypt(m_X), pEOStream->writeEncrypt(m_Y),
                            pEOStream->writeEncrypt(m_Dir));
    } else
#endif
    {
        oStream.write(m_Dir);
        oStream.write(m_X);
        oStream.write(m_Y);
    }

    __END_CATCH
}

void CGMove::execute(Player* pPlayer)

{
    __BEGIN_TRY

    CGMoveHandler::execute(this, pPlayer);

    __END_CATCH
}

string CGMove::toString() const

{
    __BEGIN_TRY

    // SECURITY: Dir2String[] holds eight entries (LEFT..LEFTUP) but m_Dir is a
    // raw wire BYTE, so 0-255 reaches this lookup -- and toString() is called by
    // SocketInputStream::readPacket() on every packet received, before any
    // handler runs. Bounded here rather than rejected in read(): Zone::movePC
    // treats dir >= DIR_MAX as a recoverable error (it answers GCMoveError and
    // returns, Zone.cpp:2426), so an out-of-range direction is an anticipated
    // value with defined behaviour, not a protocol violation. "UNKNOWN" matches
    // CGConnect::toString().
    string dir = "UNKNOWN";
    if (m_Dir < DIR_MAX)
        dir = Dir2String[m_Dir];

    StringStream msg;
    msg << "CGMove("
        << "X:" << (int)m_X << ",Y:" << (int)m_Y << ",Dir:" << dir << ")";
    return msg.toString();

    __END_CATCH
}
