//--------------------------------------------------------------------------------
//
// Filename    : GCRemoveCorpseHeadHandler.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCRemoveCorpseHead.h"
#include "ClientDef.h"

#ifdef __GAME_CLIENT__
#endif

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCRemoveCorpseHeadHandler::execute ( GCRemoveCorpseHead * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY 


	int objectID = pPacket->getObjectID();

	
	//------------------------------------------------------
	
	//------------------------------------------------------
	if (g_pZone==NULL)
	{
		// message
		DEBUG_ADD("[Error] Zone is Not Init.. yet.");			
	}
	//------------------------------------------------------
	
	//------------------------------------------------------
	else
	{	
		//----------------------------------------	
		
		//----------------------------------------	
		MCreature* pCreature = g_pZone->GetCreature( objectID );
		
		//---------------------------------------------------------
		//
		
		//
		//---------------------------------------------------------
		if (pCreature==NULL)
		{
			
			MItem* pItem = g_pZone->GetItem( objectID );

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			if (pItem==NULL)
			{
				
				DEBUG_ADD("[Error] There is no such Item ID");
			}
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			else if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)
			{
				MCreature* pCreature = ((MCorpse*)pItem)->GetCreature();

				pCreature->RemoveHead();
			}
			else
			{
				
				DEBUG_ADD_FORMAT("[Error] It's not Corpse. id=%d", objectID);
			}
		}
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		else
		{
			pCreature->RemoveHead();			
		}
	}


	__END_CATCH
}
