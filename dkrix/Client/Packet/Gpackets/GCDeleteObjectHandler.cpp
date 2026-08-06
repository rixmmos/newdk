//--------------------------------------------------------------------------------
//
// Filename    : GCDeleteObjectHandler.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCDeleteObject.h"
#include "ClientDef.h"
#include "UIFunction.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCDeleteObjectHandler::execute ( GCDeleteObject * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	//cout << "Object[" << pPacket->getObjectID() << "] deleted from zone." << endl;

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
		TYPE_OBJECTID objectID = pPacket->getObjectID();

		if (g_pPlayer->IsItemCheckBufferDropToRelicTable())
		{
			//---------------------------------------------
			
			//---------------------------------------------
			MItem* pItem = g_pPlayer->GetItemCheckBuffer();

			if(pItem->GetID() == objectID)UI_DropItem();
			g_pPlayer->ClearItemCheckBuffer();
		}

		//------------------------------------------------------
		
		//------------------------------------------------------
		if ( g_pZone->RemoveCreature( objectID ) )
		{
			DEBUG_ADD_FORMAT("Remove Creature OK. ID=%d", objectID);
		}
		//------------------------------------------------------
		
		//------------------------------------------------------
		else if ( g_pZone->RemoveItem( objectID ) )
		{			
			DEBUG_ADD_FORMAT("Remove Item OK. ID=%d", objectID);
		}
		//------------------------------------------------------
		
		//------------------------------------------------------
		else
		{
			DEBUG_ADD_FORMAT("[Error] Can't Remove Object. ID=%d", objectID);
		}
	}

#endif

	__END_CATCH
}
