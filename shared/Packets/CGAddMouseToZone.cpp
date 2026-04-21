//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAddMouseToZone.cpp
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGAddMouseToZone.h"

#include "SocketEncryptInputStream.h"
#include "SocketEncryptOutputStream.h"


CGAddMouseToZone::CGAddMouseToZone()

    {__BEGIN_TRY __END_CATCH}

CGAddMouseToZone::~CGAddMouseToZone()

{
    __BEGIN_TRY
    __END_CATCH
}

void CGAddMouseToZone::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

#ifdef __USE_ENCRYPTER__
    SocketEncryptInputStream* pEIStream = dynamic_cast<SocketEncryptInputStream*>(&iStream);

    if (pEIStream != NULL && pEIStream->getEncryptCode() != 0) {
        pEIStream->readEncrypt(m_ObjectID);
    } else
#endif
    {
        iStream.read(m_ObjectID);
    }

    __END_CATCH
}

void CGAddMouseToZone::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

#ifdef __USE_ENCRYPTER__
    SocketEncryptOutputStream* pEOStream = dynamic_cast<SocketEncryptOutputStream*>(&oStream);

    if (pEOStream != NULL && pEOStream->getEncryptCode() != 0) {
        pEOStream->writeEncrypt(m_ObjectID);
    } else
#endif
    {
        oStream.write(m_ObjectID);
    }

    __END_CATCH
}

void CGAddMouseToZone::execute(Player* pPlayer)

{
    __BEGIN_TRY

#ifndef __GAME_CLIENT__
    CGAddMouseToZoneHandler::execute(this, pPlayer);
#endif

    __END_CATCH
}

string CGAddMouseToZone::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "CGAddMouseToZone(ObjectID : " << (int)m_ObjectID << ")";
    return msg.toString();

    __END_CATCH
}
