//--------------------------------------------------------------------------------
//
// Filename    : CURequestHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "CURequest.h"

#ifdef __UPDATE_SERVER__
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/sendfile.h>
#include <sys/types.h>

#include "Properties.h"
#include "Resource.h"
#include "Upackets/UCUpdate.h"
#include "UpdateServerPlayer.h"
#endif

//--------------------------------------------------------------------------------
//


//
//--------------------------------------------------------------------------------
void CURequestHandler::execute(CURequest* pPacket, Player* pPlayer) throw(ProtocolException, Error) {
    __BEGIN_TRY

#ifdef __UPDATE_SERVER__

    UpdateServerPlayer* pUpdateServerPlayer = dynamic_cast<UpdateServerPlayer*>(pPlayer);

    Socket* pSocket = pUpdateServerPlayer->getSocket();

    const Resource& resource = pPacket->getResource();

    Version_t ClientVersion = 0;

    if (resource.getVersion() < 2000) {
        ClientVersion = 1;
    }

    char filename[maxFilename + 1];
    sprintf(filename, "%s/%s/v%05d/%s", g_pConfig->getProperty("HomeDir").c_str(),
            g_pConfig->getProperty("PatchDir").c_str(),
            //		resource.getVersion() ,
            ClientVersion, resource.getFilename().c_str());

    // cout << "Try to open " << filename << "..." << endl;

    ifstream ifile(filename, ios::in | ios::binary);

    if (!ifile) {
        
        
        throw InvalidProtocolException(filename);
    }

    
    ifile.seekg(0, ios::end);
    FileSize_t filesize = ifile.tellg();

    // cout << "Original FileSize  : " << filesize << endl;
    // cout << "Requested FileSize : " << resource.getFileSize() << endl;

    
    if (resource.getFileSize() != filesize) {
        throw InvalidProtocolException("invalid filesize");
    }

    //--------------------------------------------------------------------------------
    
    
    //--------------------------------------------------------------------------------
    UCUpdate ucUpdate;
    ucUpdate.setResource(resource);

    pUpdateServerPlayer->sendPacket(&ucUpdate);

    //--------------------------------------------------------------------------------
    // write file content
    //--------------------------------------------------------------------------------
    
    ifile.seekg(0);
    long offset = 0;

    // cout << "=======================================================================" << endl;
    // cout << resource.getFilename() << " (" << filesize << "bytes) : ";

    // TODO: make fd() work??
    
    // DWORD nSent = sendfile( pSocket->getSOCKET() , ifile.rdbuf()->fd() , &offset, filesize );

    // cout << nSent << " bytes sent to client." << endl;
    // cout << "=======================================================================" << endl;

    // if ( nSent != filesize ) {
    
    // }

    
    ifile.close();

    pUpdateServerPlayer->setPlayerStatus(USPS_AFTER_SENDING_UC_UPDATE);

#endif

    __END_CATCH
}
