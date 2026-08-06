//////////////////////////////////////////////////////////////////////
//
// Filename    : RCRequestedFileHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "RCRequestedFile.h"
#include "../RequestClientPlayer.h"
#include "RequestFileManager.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void RCRequestedFileHandler::execute ( RCRequestedFile * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	RequestClientPlayer* pRequestClientPlayer = dynamic_cast<RequestClientPlayer*>( pPlayer );

	if (pRequestClientPlayer!=NULL
		&& g_pRequestFileManager!=NULL)
	{
		RequestReceiveInfo* pInfo = new RequestReceiveInfo( pRequestClientPlayer->getRequestServerName().c_str() );
				
		int listNum = pPacket->getListNum();

		for (int i=0; i<listNum; i++)
		{
			RCRequestedFileInfo* pFileInfo = pPacket->popInfo();

			if (pFileInfo!=NULL)
			{
				ReceiveFileInfo* pReceiveFileInfo = new ReceiveFileInfo( pFileInfo->getFilename().c_str(), pFileInfo->getRequestFileType() );

				pInfo->AddReceiveFileInfo( pReceiveFileInfo );

				pReceiveFileInfo->StartReceive( pFileInfo->getFileSize() );

				delete pFileInfo;
			}
		}
		
		if (g_pRequestFileManager->AddMyRequest(pInfo))
		{
			
			//pInfo->StartReceive( pFileInfo->getFileSize() );
		}
		else
		{
			//pRequestClientPlayer->disconnect( UNDISCONNECTED );
			throw DisconnectException("can't add myRequest");
		}
	}

#endif

	__END_CATCH
}
