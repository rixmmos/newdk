//----------------------------------------------------------------------
// MVampireGear.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MItem.h"
#include "MVampireGear.h"

#ifdef __GAME_CLIENT__
	#include "ClientFunction.h"
	#include "MPlayer.h"
#endif
#include "MTimeItemManager.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MVampireGear*		g_pVampireGear = NULL;

//----------------------------------------------------------------------
// static
//----------------------------------------------------------------------
MVampireGear::FUNCTION_GEARSLOT_CHECK
MVampireGear::s_GearSlotCheckTable[MAX_GEAR_VAMPIRE] =
{
	IsGearSlotNecklace,			
	IsGearSlotCoat,				
	IsGearSlotBracelet,			
	IsGearSlotBracelet,			
	IsGearSlotRing,				
	IsGearSlotRing,				
	IsGearSlotRing,				
	IsGearSlotRing,				
	IsGearSlotEarRing,			
	IsGearSlotEarRing,			
	IsGearSlotLeftHand,			
	IsGearSlotRightHand,		
	IsGearSlotAmulet,			
	IsGearSlotAmulet,			
	IsGearSlotAmulet,			
	IsGearSlotAmulet,			
	IsGearSlotCoreZap,			
	IsGearSlotCoreZap,			
	IsGearSlotCoreZap,			
	IsGearSlotCoreZap,			
	IsGearSlotDermis,			
	IsGearSlotPersona,			
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
};

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		MVampireGear::IsGearSlotNecklace(MItem* pItem) 	{ return pItem->IsGearSlotVampireNecklace(); }	
bool		MVampireGear::IsGearSlotCoat(MItem* pItem) 		{ return pItem->IsGearSlotVampireCoat(); }		
bool		MVampireGear::IsGearSlotBracelet(MItem* pItem) 	{ return pItem->IsGearSlotVampireBracelet(); }	
bool		MVampireGear::IsGearSlotRing(MItem* pItem) 		{ return pItem->IsGearSlotVampireRing(); }		
bool		MVampireGear::IsGearSlotEarRing(MItem* pItem) 	{ return pItem->IsGearSlotVampireEarRing(); }	
bool		MVampireGear::IsGearSlotLeftHand(MItem* pItem) 	{ return pItem->IsGearSlotVampireLeftHand(); }	
bool		MVampireGear::IsGearSlotRightHand(MItem* pItem) { return pItem->IsGearSlotVampireRightHand(); }	
bool		MVampireGear::IsGearSlotAmulet(MItem* pItem)	{ return pItem->IsGearSlotVampireAmulet(); }	
bool		MVampireGear::IsGearSlotCoreZap(MItem* pItem)	{ return pItem->IsGearSlotVampireCoreZap(); }	
bool		MVampireGear::IsGearSlotDermis(MItem* pItem)	{ return pItem->IsGearSlotVampireDermis(); }	
bool		MVampireGear::IsGearSlotPersona(MItem* pItem)	{ return pItem->IsGearSlotVampirePersona(); }	
bool		MVampireGear::IsGearSlotBloodBible(MItem* pItem){ return pItem->IsGearSlotVampireBloodBible(); }	

//----------------------------------------------------------------------
//
// constructor
// 
//----------------------------------------------------------------------
MVampireGear::MVampireGear()
{
	m_bBloodBibleOpenCount = 0;
}

MVampireGear::~MVampireGear()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init 
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MVampireGear::Init()
{
	
	MPlayerGear::Init(MAX_GEAR_VAMPIRE);
	m_Gilles_CoreZap = 12;
	m_bBloodBibleOpenCount = 0;
}

//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void			
MVampireGear::CheckAffectStatus(MItem* pItem)
{
	#ifdef __GAME_CLIENT__

		g_pPlayer->CheckAffectStatus( pItem );

	#endif
}

//----------------------------------------------------------------------
// Add Item ( pItem )
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
MVampireGear::AddItem(MItem* pItem)
{
	for (int i=0; i<MAX_GEAR_VAMPIRE; i++)
	{
		
		if (AddItem( pItem, (enum GEAR_VAMPIRE)i ))
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------
// Can Replace Item ( pItem, n )
//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
bool			
MVampireGear::CanReplaceItem(MItem* pItem, GEAR_VAMPIRE n, MItem*& pOldItem)
{	
	
	if(pItem->IsAffectStatus() == false&& !pItem->IsQuestItem())
		return false;

	pOldItem = NULL;

	//---------------------------------------------------------
	
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsVampireItem())
	{
		return false;
	}

	if(n>=GEAR_VAMPIRE_BLOOD_BIBLE1 && n<=GEAR_VAMPIRE_BLOOD_BIBLE6) 
		return false;
	//-------------------------------------------------
	//
	
	//
	//-------------------------------------------------
	if (n==GEAR_VAMPIRE_LEFTHAND || n==GEAR_VAMPIRE_RIGHTHAND)	
	{
		//-------------------------------------------------
		
		//-------------------------------------------------
		if (pItem->IsGearSlotTwoHand())
		{			
			//-------------------------------------------------
			
			//-------------------------------------------------			
			if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]==NULL)
			{
				return true;
			}
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]==m_ItemSlot[GEAR_VAMPIRE_LEFTHAND])
				{
					pOldItem = m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND];
					return true;
				}
				
				return false;				
			}

			//-------------------------------------------------
			
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]!=NULL)
			{
				pOldItem = m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND];
				return true;
			}

			//-------------------------------------------------
			
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]!=NULL)
			{			
				pOldItem = m_ItemSlot[GEAR_VAMPIRE_LEFTHAND];
				return true;
			}

			
			return false;
		}

		//-------------------------------------------------
		
		//-------------------------------------------------				
		else if (pItem->IsGearSlotRightHand())
		{
			
			pOldItem = m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND];
			return true;
		}
		
		//-------------------------------------------------				
		
		//-------------------------------------------------
		else if (pItem->IsGearSlotLeftHand())
		{
			pOldItem = m_ItemSlot[GEAR_VAMPIRE_LEFTHAND];
			return true;
		}

		
		return false;
	}
	 
	if (n >= GEAR_VAMPIRE_RING1 && n <= GEAR_VAMPIRE_RING4)	
	{
		if (pItem->IsGearSlotVampireRing())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]==NULL) 
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotVampireCoreZap())
		{			
			if (m_ItemSlot[n]!=NULL) 
			{
				if(m_ItemSlot[n+m_Gilles_CoreZap]!=NULL)
					pOldItem = m_ItemSlot[n+m_Gilles_CoreZap];
				else
					pOldItem = NULL;
				return true;
			}
		}
		return false;
	}
	else if(n >= GEAR_VAMPIRE_ZAP1 && n <= GEAR_VAMPIRE_ZAP4) 
		return false;

	//----------------------------------------------------
	
	//----------------------------------------------------	
	if (pItem->IsGearItem()	&&				
		(*s_GearSlotCheckTable[n])(pItem))	
	{
		
		pOldItem = m_ItemSlot[n];
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Add Item ( pItem, n )
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool			
MVampireGear::AddItem(MItem* pItem, GEAR_VAMPIRE n)
{	
	//---------------------------------------------------------
	
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsVampireItem())
	{
		return false;
	}

	//-------------------------------------------------
	
	//-------------------------------------------------
	if ((n==GEAR_VAMPIRE_LEFTHAND || n==GEAR_VAMPIRE_RIGHTHAND) &&
		pItem->IsGearSlotTwoHand())
	{
		
		if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]==NULL
				&& m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]==NULL)
		{
			
			if (MPlayerGear::AddItem( pItem, GEAR_VAMPIRE_RIGHTHAND ))
			{
				
				m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = pItem;

				//-------------------------------------------------
				
				//-------------------------------------------------
				#ifdef __GAME_CLIENT__
					PlaySound( pItem->GetGearSoundID() );
					g_pPlayer->CalculateStatus();	
				#endif

				return true;
			}

			return false;
		}

		return false;
	}
	 
	if (n >= GEAR_VAMPIRE_RING1 && n <= GEAR_VAMPIRE_RING4)	
	{
		if (pItem->IsGearSlotVampireRing())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]==NULL&&m_ItemSlot[n]==NULL) 
			{
				if (MPlayerGear::AddItem( pItem, n ))	
				{
					//-------------------------------------------------
					
					//-------------------------------------------------
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();	
					#endif

					return true;
				}
			}
		}
		else 
		if (pItem->IsGearSlotVampireCoreZap())
		{
			if (m_ItemSlot[n-m_Gilles_CoreZap]!=NULL && m_ItemSlot[n]==NULL) 
			{		
				if (MPlayerGear::AddItem( pItem, n + m_Gilles_CoreZap ))
				{
					//-------------------------------------------------
					
					//-------------------------------------------------
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();	
					#endif

					return true;
				}

			}
		}

		return false;
	}

//		return false;
	//----------------------------------------------------
	
	//----------------------------------------------------	
	if (m_ItemSlot[n]==NULL &&				
		pItem->IsGearItem()	&&				
		(*s_GearSlotCheckTable[n])(pItem))	
	{
		
		
		bool bAdd = MPlayerGear::AddItem( pItem, n );

		//-------------------------------------------------
		
		//-------------------------------------------------
		if (bAdd)
		{
			#ifdef __GAME_CLIENT__		
				PlaySound( pItem->GetGearSoundID() );
				g_pPlayer->CalculateStatus();
			#endif

			return true;
		}		

		return false;
	}

	return false;
}

//----------------------------------------------------------------------
// Remove Item ( n)
//----------------------------------------------------------------------
MItem*			
MVampireGear::RemoveItem(GEAR_VAMPIRE n)
{ 
	MItem* pItem = m_ItemSlot[n];

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		n = GEAR_VAMPIRE_RIGHTHAND;
	}
	
	 
	if(n>=GEAR_VAMPIRE_RING1 && n<= GEAR_VAMPIRE_RING4) 
	{
		if(NULL != m_ItemSlot[n+m_Gilles_CoreZap]) 
			n = (GEAR_VAMPIRE)(n + m_Gilles_CoreZap); 
	}

//		return false;

	pItem = MPlayerGear::RemoveItem( (BYTE)n );

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = NULL;
	}
	
	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	
	return pItem;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*			
MVampireGear::RemoveItem(TYPE_OBJECTID id)
{ 
	MItem* pItem = MPlayerGear::RemoveItem( id ); 

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = NULL;
	}


	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	
	return pItem;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
MVampireGear::ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
{
	//---------------------------------------------------------
	
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsVampireItem())
	{
		return false;
	}

	//-------------------------------------------------
	//
	
	//
	//-------------------------------------------------
	if (n==GEAR_VAMPIRE_LEFTHAND || n==GEAR_VAMPIRE_RIGHTHAND)	
	{
		bool bAdd = false;
			
		//-------------------------------------------------
		
		//-------------------------------------------------
		if (pItem->IsGearSlotTwoHand())
		{			
			//-------------------------------------------------
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]==NULL)
			{
				
				pOldItem = NULL;
				if (MPlayerGear::AddItem( pItem, GEAR_VAMPIRE_RIGHTHAND ))
				{
					//-------------------------------------------------
					
					//-------------------------------------------------
					CheckItemStatus( pItem, GEAR_VAMPIRE_RIGHTHAND );

					pItem->SetItemSlot( GEAR_VAMPIRE_RIGHTHAND );
					m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = pItem;
					m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = pItem;

					//-------------------------------------------------
					
					//-------------------------------------------------
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );						
						g_pPlayer->CalculateStatus();			
					#endif

					return true;
				}

				return false;
			}
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]==m_ItemSlot[GEAR_VAMPIRE_LEFTHAND])
				{
					
					if (MPlayerGear::ReplaceItem( pItem, 
													GEAR_VAMPIRE_RIGHTHAND, 
													pOldItem ))
					{
						//-------------------------------------------------
						
						//-------------------------------------------------
						CheckItemStatus( pItem, GEAR_VAMPIRE_RIGHTHAND );

						//pItem->SetItemSlot( GEAR_VAMPIRE_RIGHTHAND );
						m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = pItem;
						//m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = pItem;

						//-------------------------------------------------
						
						//-------------------------------------------------
						#ifdef __GAME_CLIENT__
							PlaySound( pItem->GetGearSoundID() );
							g_pPlayer->CalculateStatus();	
						#endif	

						return true;
					}

					return false;
				}
				
				//-------------------------------------------------
				
				//-------------------------------------------------
				pOldItem = NULL;
				return false;
				
			}

			//-------------------------------------------------
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_VAMPIRE_RIGHTHAND, 
													pOldItem );
			}		

			//-------------------------------------------------
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_VAMPIRE_LEFTHAND, 
													pOldItem );					
			}

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			if (bAdd)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_VAMPIRE_RIGHTHAND );
				
				pItem->SetItemSlot( GEAR_VAMPIRE_RIGHTHAND );
				m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = pItem;
				m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = pItem;
		
				//-------------------------------------------------
				
				//-------------------------------------------------
				#ifdef __GAME_CLIENT__
					PlaySound( pItem->GetGearSoundID() );
					g_pPlayer->CalculateStatus();
				#endif

				return true;
			}

			return false;
		}
		//-------------------------------------------------
		
		//-------------------------------------------------		
		else if (pItem->IsGearSlotRightHand())
		{
			bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_VAMPIRE_RIGHTHAND, 
													pOldItem );

			//-------------------------------------------------
			
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					
					m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = NULL;
				}

			}

			if (bAdd)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_VAMPIRE_RIGHTHAND );

				//-------------------------------------------------
				
				//-------------------------------------------------
				#ifdef __GAME_CLIENT__
					PlaySound( pItem->GetGearSoundID() );
					g_pPlayer->CalculateStatus();
				#endif

				return true;
			}

			return false;
		}
		//-------------------------------------------------
		
		//-------------------------------------------------
		else if (pItem->IsGearSlotLeftHand())
		{
			bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_VAMPIRE_LEFTHAND, 
													pOldItem );
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					
					m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = NULL;
				}

			}

			if (bAdd)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_VAMPIRE_LEFTHAND );
				
				//-------------------------------------------------
				
				//-------------------------------------------------
				#ifdef __GAME_CLIENT__
					PlaySound( pItem->GetGearSoundID() );
					g_pPlayer->CalculateStatus();
				#endif	

				return true;
			}

			return false;
		}
		
		//-------------------------------------------------
		
		
		//-------------------------------------------------
		pOldItem = NULL;
		return false;
		
	}
	 
	if (n >= GEAR_VAMPIRE_RING1 && n <= GEAR_VAMPIRE_RING4)	
	{
		if (pItem->IsGearSlotVampireRing())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]!=NULL) 
				return false;
		}
		else
		if (pItem->IsGearSlotVampireCoreZap())
		{			
			if (m_ItemSlot[n]!=NULL) 
			{
				bool bAdd = false;
				if(m_ItemSlot[n+m_Gilles_CoreZap] == NULL)
				{
					pOldItem = NULL;
					bAdd = MPlayerGear::AddItem( pItem, n+m_Gilles_CoreZap );	
				}
				else 
				{
					bAdd = MPlayerGear::ReplaceItem( pItem, n+m_Gilles_CoreZap, pOldItem );	
					
					if (bAdd)
					{
						CheckItemStatus( pItem, n+m_Gilles_CoreZap );
					}			
				}

				if (bAdd) 
				{
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();
					#endif
					return true;
				}
				else 
					return false;
			}
			return false;
		}
	}

//		return false;
	
	//----------------------------------------------------
	
	//----------------------------------------------------	
	if (
		pItem->IsGearItem()	&&				
		(*s_GearSlotCheckTable[n])(pItem))	
	{
		
		if (MPlayerGear::ReplaceItem( pItem, n, pOldItem ))
		{
			//-------------------------------------------------
			
			//-------------------------------------------------
			CheckItemStatus( pItem, n );			

			//-------------------------------------------------
			
			//-------------------------------------------------
			#ifdef __GAME_CLIENT__
				PlaySound( pItem->GetGearSoundID() );
			#endif

			#ifdef __GAME_CLIENT__
				g_pPlayer->CalculateStatus();
			#endif

			return true;
		}

		return false;
	}

	return false;
}


MItem*	MVampireGear::GetGearItem_PickUp(int &n) const	
{
	int Slot = n;

//		return NULL;
	if(Slot>=GEAR_VAMPIRE_RING1 && Slot<=GEAR_VAMPIRE_RING4)
	{
		if(m_ItemSlot[Slot+m_Gilles_CoreZap]!=NULL)
		{
			n = n+m_Gilles_CoreZap;
			return MSlotItemManager::GetItem( (BYTE)(Slot+m_Gilles_CoreZap) ); 
		}
	}
	
	return MSlotItemManager::GetItem( (BYTE)Slot ); 
}



MItem*	MVampireGear::GetGearCoreZapItem(int slot) const	
{
	if(slot>=GEAR_VAMPIRE_RING1 && slot<=GEAR_VAMPIRE_RING4)
	{
		if(m_ItemSlot[slot+m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot+m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_CORE_ZAP)
				return MSlotItemManager::GetItem( (BYTE)(slot+m_Gilles_CoreZap) ); 
		}
	}
	
	return NULL;
}


MItem*	MVampireGear::GetGearCoreZapedItem(int slot) const	
{
	if(slot>=GEAR_VAMPIRE_ZAP1 && slot<=GEAR_VAMPIRE_ZAP4)
	{
		if(slot-m_Gilles_CoreZap<0)
			return NULL;
		if(m_ItemSlot[slot-m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot-m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_VAMPIRE_RING)
				return MSlotItemManager::GetItem( (BYTE)(slot-m_Gilles_CoreZap) ); 
		}
	}
	
	return NULL;
}

const bool	MVampireGear::IsHasAllCoreZap(int CoreZapType) const	
{
	BYTE bType[4] = {0,};
	if(CoreZapType == 0) 
	{
		MItem* pItem;
		for(int i = 0; i<4; i++)
		{
			pItem = m_ItemSlot[GEAR_VAMPIRE_ZAP1+i];
			if(pItem != NULL)
			{
				if(pItem->GetItemType()>3)
					return false;
				if(bType[pItem->GetItemType()]!= 0)
					return false;
				bType[pItem->GetItemType()] = 1;
			}
			else
				return false;
		}
		if(bType[0]+bType[1]+bType[2]+bType[3] == 4)
			return true;
	}
	
	return false;
}

const bool	MVampireGear::IsCloseBloodBibleSlot(int slot) const	
{
	if(slot>=GEAR_VAMPIRE_BLOOD_BIBLE1 && slot<=GEAR_VAMPIRE_BLOOD_BIBLE6)
	{
		int TempSlot = slot - GEAR_VAMPIRE_BLOOD_BIBLE1 + 1;
		if(TempSlot > m_bBloodBibleOpenCount)
			return true;
	}
	return false;
}
