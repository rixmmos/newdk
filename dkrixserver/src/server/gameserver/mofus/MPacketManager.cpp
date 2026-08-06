/////////////////////////////////////////////////////////////////////////////
// Filename	: MPacketManager.cpp
/////////////////////////////////////////////////////////////////////////////

// include files
#include "MPacketManager.h"

#include "Assert.h"
#include "MPacketHandler.h"
#include "MPacketID.h"
#include "Mofus.h"


#include "PKTConnectAsk.h"
#include "PKTLogout.h"
#include "PKTReceiveOK.h"
#include "PKTResult.h"
#include "PKTSError.h"
#include "PKTUserInfo.h"


#include "PKTConnectAccept.h"
#include "PKTConnectAcceptHandler.h"
#include "PKTError.h"
#include "PKTErrorHandler.h"
#include "PKTPowerPoint.h"
#include "PKTPowerPointHandler.h"



#define REGISTER_SEND_PACKET_ID(PACKET, PACKET_ID) \
    MPacketID_t PACKET::getID() const {            \
        return PACKET_ID;                          \
    }


#define REGISTER_RECV_PACKET_ID(PACKET, PACKET_ID) \
    MPacketID_t PACKET::getID() const {            \
        return PACKET_ID;                          \
    }                                              \
    MPacketID_t PACKET##Handler::getID() const {   \
        return PACKET_ID;                          \
    }


REGISTER_SEND_PACKET_ID(PKTConnectAsk, PTC_CONNECT_ASK)
REGISTER_SEND_PACKET_ID(PKTLogout, PTC_LOGOUT)
REGISTER_SEND_PACKET_ID(PKTUserInfo, PTC_USERINFO)
REGISTER_SEND_PACKET_ID(PKTReceiveOK, PTC_RECEIVE_OK)
REGISTER_SEND_PACKET_ID(PKTResult, PTC_RESULT)
REGISTER_SEND_PACKET_ID(PKTSError, PTC_ERROR)


REGISTER_RECV_PACKET_ID(PKTConnectAccept, PTS_CONNECT_ACCEPT)
REGISTER_RECV_PACKET_ID(PKTPowerPoint, PTS_POWERPOINT)
REGISTER_RECV_PACKET_ID(PKTError, PTS_ERROR)



struct MPacketManager::IMPL {
    MPacket* pCreators[PTC_MAX];
    MPacketHandler* pHandlers[PTC_MAX];

    IMPL();
    ~IMPL();

    
    void addCreator(MPacket* pPacket);

    
    void addHandler(MPacketHandler* pHandler);
};



MPacketManager::MPacketManager() : m_pImpl(new IMPL) {
    Assert(m_pImpl != NULL);
}


MPacketManager::~MPacketManager() {
    SAFE_DELETE(m_pImpl);
}


void MPacketManager::init() {
    
    m_pImpl->addCreator(new PKTConnectAccept);
    m_pImpl->addHandler(new PKTConnectAcceptHandler);
    m_pImpl->addCreator(new PKTPowerPoint);
    m_pImpl->addHandler(new PKTPowerPointHandler);
    m_pImpl->addCreator(new PKTError);
    m_pImpl->addHandler(new PKTErrorHandler);
}


void MPacketManager::addCreator(MPacket* pPacket) {
    Assert(pPacket != NULL);
    m_pImpl->addCreator(pPacket);
}


void MPacketManager::addHandler(MPacketHandler* pHandler) {
    Assert(pHandler != NULL);
    m_pImpl->addHandler(pHandler);
}


MPacket* MPacketManager::createPacket(MPacketID_t ID) const {
    if (ID < 0 || ID >= PTC_MAX) {
        filelog(MOFUS_ERROR_FILE, "MPacketManager::createPacket() out of ID");
        Assert(false);
    }

    return m_pImpl->pCreators[ID]->create();
}


void MPacketManager::execute(MPlayer* pPlayer, MPacket* pPacket) {
    Assert(pPlayer != NULL);
    Assert(pPacket != NULL);

    MPacketID_t ID = pPacket->getID();

    if (ID < 0 || ID >= PTC_MAX) {
        filelog(MOFUS_ERROR_FILE, "MPacketManager::createPacket() out of ID");
        Assert(false);
    }

    if (m_pImpl->pHandlers[ID] == NULL) {
        filelog(MOFUS_ERROR_FILE, "MPacketManager::execute() Handler is NULL");
        Assert(false);
    }

    m_pImpl->pHandlers[ID]->execute(pPlayer, pPacket);
}


bool MPacketManager::hasHandler(MPacketID_t ID) const {
    
    if (ID < 0 || ID >= PTC_MAX) {
        return false;
    }

    
    if (m_pImpl->pHandlers[ID] == NULL) {
        return false;
    }

    return true;
}


MPacketSize_t MPacketManager::getPacketSize(MPacketID_t ID) const {
    if (ID < 0 || ID >= PTC_MAX) {
        filelog(MOFUS_ERROR_FILE, "MPacketManager::createPacket() out of ID");
        Assert(false);
    }

    return m_pImpl->pCreators[ID]->getSize();
}


MPacketManager::IMPL::IMPL() {
    
    for (MPacketID_t i = 0; i < PTC_MAX; ++i) {
        pCreators[i] = NULL;
        pHandlers[i] = NULL;
    }
}


MPacketManager::IMPL::~IMPL() {
    
    for (MPacketID_t i = 0; i < PTC_MAX; ++i) {
        SAFE_DELETE(pCreators[i]);
        SAFE_DELETE(pHandlers[i]);
    }
}


void MPacketManager::IMPL::addCreator(MPacket* pPacket) {
    Assert(pPacket != NULL);

    
    if (pCreators[pPacket->getID()] != NULL) {
        filelog(MOFUS_ERROR_FILE, "MPacketManager::IMPL::addCreator() dup creator");
        Assert(false);
    }

    
    pCreators[pPacket->getID()] = pPacket;
}


void MPacketManager::IMPL::addHandler(MPacketHandler* pHandler) {
    Assert(pHandler != NULL);

    
    if (pHandlers[pHandler->getID()] != NULL) {
        filelog(MOFUS_ERROR_FILE, "MPacketManager::IMPL::addHandler() dup handler");
        Assert(false);
    }

    
    pHandlers[pHandler->getID()] = pHandler;
}

// global variable
MPacketManager* g_pMPacketManager = NULL;
