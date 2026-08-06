/////////////////////////////////////////////////////////////////////////////
// Filename : PKTPowerPointHandler.cpp
// Desc		:
/////////////////////////////////////////////////////////////////////////////

// include files
#include "PKTPowerPointHandler.h"

#include "Assert.h"
#include "MJob.h"
#include "MPlayer.h"
#include "Mofus.h"
#include "PKTError.h"
#include "PKTPowerPoint.h"
#include "PKTSError.h"
#include "Properties.h"


void PKTPowerPointHandler::execute(MPlayer* pPlayer, MPacket* pPacket) {
    PKTPowerPoint* pPowerPoint = dynamic_cast<PKTPowerPoint*>(pPacket);
    Assert(pPowerPoint != NULL);

    cout << "--------------------------------------------------" << endl;
    cout << "RECV [" << pPlayer->getJob()->getName() << "] PowerPoint (name:" << pPowerPoint->sCharName
         << ",point:" << pPowerPoint->nPowerPoint << ")" << endl;
    cout << "--------------------------------------------------" << endl;

    filelog(MOFUS_LOG_FILE, "RECV [%s] PowerPoint (name:%s,point:%d)", pPlayer->getJob()->getName().c_str(),
            pPowerPoint->sCharName, pPowerPoint->nPowerPoint);
    filelog(MOFUS_PACKET_FILE, "RECV : [%s] %s", pPlayer->getJob()->getName().c_str(), pPacket->toString().c_str());

    //////////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////
    static int MofusServerCode = g_pConfig->getPropertyInt("MofusServerCode");
    
    bool bCheckGameCode = (pPowerPoint->getGameCode() == 1);
    
    bool bCheckGameServerCode = (pPowerPoint->getGameServerCode() == MofusServerCode);
    
    bool bCheckCharacterName = (strcasecmp(pPlayer->getJob()->getName().c_str(), pPowerPoint->getCharacterName()) == 0);

    if (!bCheckGameCode || !bCheckGameServerCode || !bCheckCharacterName) {
        cout << "--------------------------------------------------" << endl;
        cout << "ERROR CHECK (name:" << pPlayer->getJob()->getName() << ",mofusname:" << pPowerPoint->getCharacterName()
             << ",gameservercode:" << MofusServerCode << ",mofusgameservercode:" << pPowerPoint->getGameServerCode()
             << ",gamecode:" << pPowerPoint->getGameCode() << ")" << endl;
        cout << "--------------------------------------------------" << endl;

        filelog(MOFUS_LOG_FILE, "ERROR (name:%s,mofusname:%s,gameservercode:%d,mofusgameservercode:%d)",
                pPlayer->getJob()->getName().c_str(), pPowerPoint->getCharacterName(), MofusServerCode,
                pPowerPoint->getGameServerCode());

        
        
        pPlayer->sendSError(MSERR_MATCH);

        
        pPlayer->setErrorCode(MERR_MATCHING);

        
        pPlayer->setEnd();

        return;
    }
    //////////////////////////////////////////////////////////////////////

    
    
    
    
    
    
    
    static int MaxPowerPoint = 40;

    
    int savepowerpoint = min(pPowerPoint->getPowerPoint(), MaxPowerPoint);

    
    savePowerPoint(pPlayer->getJob()->getName(), savepowerpoint);

    
    filelog(MOFUS_LOG_FILE, "SAVE PowerPoint (name:%s,savepoint:%d,recvpoint:%d)", pPlayer->getJob()->getName().c_str(),
            savepowerpoint, pPowerPoint->getPowerPoint());

    logPowerPoint(pPlayer->getJob()->getName(), pPowerPoint->getPowerPoint(), savepowerpoint);

    
    pPlayer->addPowerPoint(savepowerpoint);

    
    if (pPowerPoint->isContinue()) {
        
        pPlayer->sendReceiveOK();
    } else {
        pPlayer->sendResult();
        pPlayer->sendLogout();

        
        pPlayer->setEnd();
    }
}
