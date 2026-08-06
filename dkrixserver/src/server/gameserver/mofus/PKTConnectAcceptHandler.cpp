/////////////////////////////////////////////////////////////////////////////
// Filename : PKTConnectAcceptHandler.cpp
// Desc		:
/////////////////////////////////////////////////////////////////////////////

// include files
#include "PKTConnectAcceptHandler.h"

#include "MJob.h"
#include "MPlayer.h"
#include "Mofus.h"



void PKTConnectAcceptHandler::execute(MPlayer* pPlayer, MPacket* pPacket) {
    cout << "--------------------------------------------------" << endl;
    cout << "RECV [" << pPlayer->getJob()->getName() << "] ConnectAccept" << endl;
    cout << "--------------------------------------------------" << endl;

    filelog(MOFUS_LOG_FILE, "RECV [%s] ConnectAccept", pPlayer->getJob()->getName().c_str());
    filelog(MOFUS_PACKET_FILE, "RECV : [%s] %s", pPlayer->getJob()->getName().c_str(), pPacket->toString().c_str());

    
    pPlayer->sendUserInfo();
}
