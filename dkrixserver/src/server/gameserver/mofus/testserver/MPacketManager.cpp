/////////////////////////////////////////////////////////////////////////////
// Filename	: MPacketManager.cpp
/////////////////////////////////////////////////////////////////////////////

// include files
#include "MPacketManager.h"

#include "../MPacketID.h"
#include "../Mofus.h"
#include "Assert.h"
#include "MPacketHandler.h"


#include "../PKTConnectAsk.h"
#include "../PKTLogout.h"
#include "../PKTReceiveOK.h"
#include "../PKTResult.h"
#include "../PKTUserInfo.h"
#include "PKTConnectAskHandler.h"
#include "PKTLogoutHandler.h"
#include "PKTReceiveOKHandler.h"
#include "PKTResultHandler.h"
#include "PKTUserInfoHandler.h"


#include "../PKTConnectAccept.h"
#include "../PKTError.h"
#include "../PKTPowerPoint.h"



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


REGISTER_RECV_PACKET_ID(PKTConnectAsk, PTC_CONNECT_ASK)
REGISTER_RECV_PACKET_ID(PKTLogout, PTC_LOGOUT)
REGISTER_RECV_PACKET_ID(PKTUserInfo, PTC_USERINFO)
REGISTER_RECV_PACKET_ID(PKTReceiveOK, PTC_RECEIVE_OK)
REGISTER_RECV_PACKET_ID(PKTResult, PTC_RESULT)


REGISTER_SEND_PACKET_ID(PKTConnectAccept, PTS_CONNECT_ACCEPT)
REGISTER_SEND_PACKET_ID(PKTPowerPoint, PTS_POWERPOINT)
REGISTER_SEND_PACKET_ID(PKTError, PTS_ERROR)



struct MPacketManager::IMPL {
    MPacket* pCreators[PTC_SEND_MAX];
    MPacketHandler* pHandlers[PTC_SEND_MAX];

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
    
    m_pImpl->addCreator(new PKTConnectAsk);
    m_pImpl->addHandler(new PKTConnectAskHandler);
    m_pImpl->addCreator(new PKTLogout);
    m_pImpl->addHandler(new PKTLogoutHandler);
    m_pImpl->addCreator(new PKTUserInfo);
    m_pImpl->addHandler(new PKTUserInfoHandler);
    m_pImpl->addCreator(new PKTReceiveOK);
    m_pImpl->addHandler(new PKTReceiveOKHandler);
    m_pImpl->addCreator(new PKTResult);
    m_pImpl->addHandler(new PKTResultHandler);
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
    if (ID < 0 || ID >= PTC_SEND_MAX) {
        filelog(MOFUS_ERROR_FILE, "MPacketManager::createPacket() out of ID");
        Assert(false);
    }

    return m_pImpl->pCreators[ID]->create();
}


void MPacketManager::execute(GameServerPlayer* pPlayer, MPacket* pPacket) {
    Assert(pPlayer != NULL);
    Assert(pPacket != NULL);

    MPacketID_t ID = pPacket->getID();

    if (ID < 0 || ID >= PTC_SEND_MAX) {
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
    
    if (ID < 0 || ID >= PTC_SEND_MAX) {
        return false;
    }

    
    if (m_pImpl->pHandlers[ID] == NULL) {
        return false;
    }

    return true;
}


MPacketSize_t MPacketManager::getPacketSize(MPacketID_t ID) const {
    if (ID < 0 || ID >= PTC_SEND_MAX) {
        filelog(MOFUS_ERROR_FILE, "MPacketManager::createPacket() out of ID");
        Assert(false);
    }

    return m_pImpl->pCreators[ID]->getSize();
}


MPacketManager::IMPL::IMPL() {
    
    for (MPacketID_t i = 0; i < PTC_SEND_MAX; ++i) {
        pCreators[i] = NULL;
        pHandlers[i] = NULL;
    }
}


MPacketManager::IMPL::~IMPL() {
    
    for (MPacketID_t i = 0; i < PTC_SEND_MAX; ++i) {
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
