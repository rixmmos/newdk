//----------------------------------------------------------------------
// MOustersGear.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MItem.h"
#include "MOustersGear.h"
#include "MQuickSlot.h"

#ifdef __GAME_CLIENT__
	#include "UIFunction.h"
	#include "ClientFunction.h"
	#include "MPlayer.h"
#endif
#include "MTimeItemManager.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MOustersGear*		g_pOustersGear = NULL;

//----------------------------------------------------------------------
// static
//----------------------------------------------------------------------
MOustersGear::FUNCTION_GEARSLOT_CHECK
MOustersGear::s_GearSlotCheckTable[MAX_GEAR_OUSTERS] =
{
	IsGearSlotCirclet,			
	IsGearSlotCoat,				
	IsGearSlotLeftHand,			
	IsGearSlotRightHand,			
	IsGearSlotBoots,				
	IsGearSlotArmsBand,			
	IsGearSlotArmsBand,			
	IsGearSlotRing,				
	IsGearSlotRing,				
	IsGearSlotPendent,			
	IsGearSlotPendent,			
	IsGearSlotPendent,			
	IsGearSlotStone,			
	IsGearSlotStone,			
	IsGearSlotStone,			
	IsGearSlotStone,			
	IsGearSlotCoreZap,			
	IsGearSlotCoreZap,			
	IsGearSlotCoreZap,			
	IsGearSlotCoreZap,			
	IsGearSlotFascia,			
	IsGearSlotMitten,			
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		

};


//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		MOustersGear::IsGearSlotCirclet(MItem* pItem)		{ return pItem->IsGearSlotOustersCirclet(); }
bool		MOustersGear::IsGearSlotCoat(MItem* pItem)			{ return pItem->IsGearSlotOustersCoat(); }
bool		MOustersGear::IsGearSlotLeftHand(MItem* pItem)		{ return pItem->IsGearSlotOustersLeftHand(); }
bool		MOustersGear::IsGearSlotRightHand(MItem* pItem)		{ return pItem->IsGearSlotOustersRightHand(); }
bool		MOustersGear::IsGearSlotBoots(MItem* pItem)			{ return pItem->IsGearSlotOustersBoots(); }
bool		MOustersGear::IsGearSlotArmsBand(MItem* pItem)		{ return pItem->IsGearSlotOustersArmsBand(); }
bool		MOustersGear::IsGearSlotRing(MItem* pItem)			{ return pItem->IsGearSlotOustersRing(); }
bool		MOustersGear::IsGearSlotPendent(MItem* pItem)		{ return pItem->IsGearSlotOustersPendent(); }
bool		MOustersGear::IsGearSlotStone(MItem* pItem)			{ return pItem->IsGearSlotOustersStone(); }
bool		MOustersGear::IsGearSlotCoreZap(MItem* pItem)		{ return pItem->IsGearSlotOustersCoreZap(); }	
bool		MOustersGear::IsGearSlotFascia(MItem* pItem)		{ return pItem->IsGearSlotOustersFascia(); }	
bool		MOustersGear::IsGearSlotMitten(MItem* pItem)		{ return pItem->IsGearSlotOustersMitten(); }	
bool		MOustersGear::IsGearSlotBloodBible(MItem* pItem)	{ return pItem->IsGearSlotOustersBloodBible(); }	

//----------------------------------------------------------------------
//
// constructor
// 
//----------------------------------------------------------------------
MOustersGear::MOustersGear()
{
	g_pArmsBand1 = NULL;
	g_pArmsBand2 = NULL;
	m_bBloodBibleOpenCount = 0;
}

MOustersGear::~MOustersGear()
{
	g_pArmsBand1 = NULL;
	g_pArmsBand2 = NULL;
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
MOustersGear::Init()
{
	
	MPlayerGear::Init(MAX_GEAR_OUSTERS);
	m_Gilles_CoreZap = 4;
	m_bBloodBibleOpenCount = 0;
}

//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void			
MOustersGear::CheckAffectStatus(MItem* pItem)
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
MOustersGear::AddItem(MItem* pItem)
{
	for (int i=0; i<MAX_GEAR_OUSTERS; i++)
	{
		
		if (AddItem( pItem, (enum GEAR_OUSTERS)i ))
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
MOustersGear::CanReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
{	
	
	if(pItem->IsAffectStatus() == false&& !pItem->IsQuestItem())
		return false;

	pOldItem = NULL;

	//---------------------------------------------------------
	
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsOustersItem())
	{
		return false;
	}
	if(n>=GEAR_OUSTERS_BLOOD_BIBLE1 && n<=GEAR_OUSTERS_BLOOD_BIBLE6) 
		return false;

	
		
	// bycsm 2004.12.31

		if(n == GEAR_OUSTERS_FASCIA)
	{
		if(pItem->IsGearSlotOustersFascia() == true)
		{
			if (m_ItemSlot[GEAR_OUSTERS_FASCIA]==NULL)
				return true;
			else
			{
				pOldItem = m_ItemSlot[GEAR_OUSTERS_FASCIA];
				return true;
			}
		}

	}

	if(n == GEAR_OUSTERS_MITTEN)
	{
		if(pItem->IsGearSlotOustersMitten()== true)
		{
			
			if (m_ItemSlot[GEAR_OUSTERS_MITTEN]==NULL)
				return true;
			else
			{
				pOldItem = m_ItemSlot[GEAR_OUSTERS_MITTEN];
				return true;
			}
		}
		
	}


	
	//-------------------------------------------------
	//
	
	//
	//-------------------------------------------------
	if (n==GEAR_OUSTERS_LEFTHAND || n==GEAR_OUSTERS_RIGHTHAND)	
	{
		//-------------------------------------------------
		
		//-------------------------------------------------
		if (pItem->IsGearSlotTwoHand())
		{			
			//-------------------------------------------------
			
			//-------------------------------------------------			
			if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_OUSTERS_LEFTHAND]==NULL)
			{
				return true;
			}
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_OUSTERS_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]==m_ItemSlot[GEAR_OUSTERS_LEFTHAND])
				{
					pOldItem = m_ItemSlot[GEAR_OUSTERS_RIGHTHAND];
					return true;
				}
				
				return false;				
			}

			//-------------------------------------------------
			
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]!=NULL)
			{
				pOldItem = m_ItemSlot[GEAR_OUSTERS_RIGHTHAND];
				return true;
			}

			//-------------------------------------------------
			
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_OUSTERS_LEFTHAND]!=NULL)
			{			
				pOldItem = m_ItemSlot[GEAR_OUSTERS_LEFTHAND];
				return true;
			}

			
			return false;
		}

		//-------------------------------------------------
		
		//-------------------------------------------------				
		else if (pItem->IsGearSlotRightHand())
		{
			
			pOldItem = m_ItemSlot[GEAR_OUSTERS_RIGHTHAND];
			return true;
		}
		
		//-------------------------------------------------				
		
		//-------------------------------------------------
		else if (pItem->IsGearSlotLeftHand())
		{
			pOldItem = m_ItemSlot[GEAR_OUSTERS_LEFTHAND];
			return true;
		}

		
		return false;
	}

	 
	if (n >= GEAR_OUSTERS_STONE1 && n <= GEAR_OUSTERS_STONE4)	
	{
		if (pItem->IsGearSlotOustersStone())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]==NULL) 
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotOustersCoreZap())
		{			
			if (m_ItemSlot[n]!=NULL) 
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
		}
		return false;
	}
	else if(n >= GEAR_OUSTERS_ZAP1 && n <= GEAR_OUSTERS_ZAP4) 
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
MOustersGear::AddItem(MItem* pItem, GEAR_OUSTERS n)
{	
	//---------------------------------------------------------
	
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsOustersItem())
	{
		return false;
	}

	//-------------------------------------------------
	
	//-------------------------------------------------
	if ((n==GEAR_OUSTERS_LEFTHAND || n==GEAR_OUSTERS_RIGHTHAND) &&
		pItem->IsGearSlotTwoHand())
	{
		
		if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]==NULL
				&& m_ItemSlot[GEAR_OUSTERS_LEFTHAND]==NULL)
		{
			
			if (MPlayerGear::AddItem( pItem, GEAR_OUSTERS_RIGHTHAND ))
			{
				
				m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = pItem;

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
	 
	if (n >= GEAR_OUSTERS_STONE1 && n <= GEAR_OUSTERS_STONE4)	
	{
		if (pItem->IsGearSlotOustersStone())
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
		if (pItem->IsGearSlotOustersCoreZap())
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

			if( n == GEAR_OUSTERS_ARMSBAND1	)
			{
				g_pArmsBand1 = (MOustersArmsBand*)pItem;
				#ifdef __GAME_CLIENT__		
				UI_ResetQuickItemSlot();
				#endif
			}
			else if ( n == GEAR_OUSTERS_ARMSBAND2 )
			{
				g_pArmsBand2 = (MOustersArmsBand*)pItem;
				#ifdef __GAME_CLIENT__		
				UI_ResetQuickItemSlot();
				#endif
			}
			
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
MOustersGear::RemoveItem(GEAR_OUSTERS n)
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
		n = GEAR_OUSTERS_RIGHTHAND;
	}
	 
	if(n>=GEAR_OUSTERS_STONE1 && n<= GEAR_OUSTERS_STONE4) 
	{
		if(NULL != m_ItemSlot[n+m_Gilles_CoreZap]) 
			n = (GEAR_OUSTERS)(n + m_Gilles_CoreZap); 
	}

//		return false;

	pItem = MPlayerGear::RemoveItem( (BYTE)n );

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	if( n == GEAR_OUSTERS_ARMSBAND1 || n == GEAR_OUSTERS_ARMSBAND2 )
	{
		if( n == GEAR_OUSTERS_ARMSBAND1 )
			g_pArmsBand1 = NULL;
		else
			g_pArmsBand2 = NULL;
		
	}

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = NULL;
	}
	
	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	
	return pItem;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*			
MOustersGear::RemoveItem(TYPE_OBJECTID id)
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
		m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = NULL;
	}
	if( pItem->GetItemClass() == ITEM_CLASS_OUSTERS_ARMSBAND )
	{
		if( pItem->GetItemSlot()== GEAR_OUSTERS_ARMSBAND1 )
			g_pArmsBand1 = NULL;
		else
			g_pArmsBand2 = NULL;		
	}


	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	
	return pItem;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
MOustersGear::ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
{
	//---------------------------------------------------------
	
	//---------------------------------------------------------	

	if (n>=m_Size || !pItem->IsOustersItem())
	{
		return false;
	}


	if(n == GEAR_OUSTERS_FASCIA)
	{
		if(pItem->IsGearSlotOustersFascia() == true)
		{
			if (m_ItemSlot[GEAR_OUSTERS_FASCIA]==NULL)
			{
				MPlayerGear::ReplaceItem( pItem, GEAR_OUSTERS_FASCIA, pOldItem );
				return true;
			}
			else
				return false;
		}
	}
	
	if(n == GEAR_OUSTERS_MITTEN)
	{
		if(pItem->IsGearSlotOustersMitten() == true)
		{
			if (m_ItemSlot[GEAR_OUSTERS_MITTEN]==NULL)
			{
				MPlayerGear::ReplaceItem( pItem, GEAR_OUSTERS_MITTEN, pOldItem );
				return true;
			}
			else
				return false;
		}
	}
	//-------------------------------------------------
	//
	
	//
	//-------------------------------------------------
	if (n==GEAR_OUSTERS_LEFTHAND || n==GEAR_OUSTERS_RIGHTHAND)	
	{
		bool bAdd = false;
			
		//-------------------------------------------------
		
		//-------------------------------------------------
		if (pItem->IsGearSlotTwoHand())
		{			
			//-------------------------------------------------
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_OUSTERS_LEFTHAND]==NULL)
			{
				
				pOldItem = NULL;
				if (MPlayerGear::AddItem( pItem, GEAR_OUSTERS_RIGHTHAND ))
				{
					//-------------------------------------------------
					
					//-------------------------------------------------
					CheckItemStatus( pItem, GEAR_OUSTERS_RIGHTHAND );

					pItem->SetItemSlot( GEAR_OUSTERS_RIGHTHAND );
					m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = pItem;
					m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = pItem;

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
			if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_OUSTERS_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]==m_ItemSlot[GEAR_OUSTERS_LEFTHAND])
				{
					
					if (MPlayerGear::ReplaceItem( pItem, 
													GEAR_OUSTERS_RIGHTHAND, 
													pOldItem ))
					{
						//-------------------------------------------------
						
						//-------------------------------------------------
						CheckItemStatus( pItem, GEAR_OUSTERS_RIGHTHAND );

						//pItem->SetItemSlot( GEAR_OUSTERS_RIGHTHAND );
						m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = pItem;
						//m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = pItem;

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
			else if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_OUSTERS_RIGHTHAND, 
													pOldItem );
			}		

			//-------------------------------------------------
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_OUSTERS_LEFTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_OUSTERS_LEFTHAND, 
													pOldItem );					
			}

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			if (bAdd)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_OUSTERS_RIGHTHAND );
				
				pItem->SetItemSlot( GEAR_OUSTERS_RIGHTHAND );
				m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = pItem;
				m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = pItem;
		
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
													GEAR_OUSTERS_RIGHTHAND, 
													pOldItem );

			//-------------------------------------------------
			
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					
					m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = NULL;
				}

			}

			if (bAdd)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_OUSTERS_RIGHTHAND );

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
													GEAR_OUSTERS_LEFTHAND, 
													pOldItem );
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					
					m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = NULL;
				}

			}

			if (bAdd)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_OUSTERS_LEFTHAND );
				
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
	 
	if (n >= GEAR_OUSTERS_STONE1 && n <= GEAR_OUSTERS_STONE4)	
	{
		if (pItem->IsGearSlotOustersStone())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]!=NULL) 
				return false;
		}
		else
		if (pItem->IsGearSlotOustersCoreZap())
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
	//	return false;
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

			if( n == GEAR_OUSTERS_ARMSBAND1	)
			{
				g_pArmsBand1 = (MOustersArmsBand*)pItem;
				#ifdef __GAME_CLIENT__		
				UI_ResetQuickItemSlot();
				#endif
			}
			else if ( n == GEAR_OUSTERS_ARMSBAND2 )
			{
				g_pArmsBand2 = (MOustersArmsBand*)pItem;
				#ifdef __GAME_CLIENT__		
				UI_ResetQuickItemSlot();
				#endif
			}
			return true;
		}

		return false;
	}

	return false;
}

MItem*	MOustersGear::GetGearItem_PickUp(int &n) const	
{
	int Slot = n;

//		return NULL;
	if(Slot>=GEAR_OUSTERS_STONE1 && Slot<=GEAR_OUSTERS_STONE4)
	{
		if(m_ItemSlot[Slot+m_Gilles_CoreZap]!=NULL)
		{
			n = n+m_Gilles_CoreZap;
			return MSlotItemManager::GetItem( (BYTE)(Slot+m_Gilles_CoreZap) ); 
		}
	}
	
	return MSlotItemManager::GetItem( (BYTE)Slot ); 
}


MItem*	MOustersGear::GetGearCoreZapItem(int slot) const	
{
	if(slot>=GEAR_OUSTERS_STONE1 && slot<=GEAR_OUSTERS_STONE4)
	{
		if(m_ItemSlot[slot+m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot+m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_CORE_ZAP)
				return MSlotItemManager::GetItem( (BYTE)(slot+m_Gilles_CoreZap) ); 
		}
	}
	
	return NULL;
}


MItem*	MOustersGear::GetGearCoreZapedItem(int slot) const	
{
	if(slot>=GEAR_OUSTERS_ZAP1 && slot<=GEAR_OUSTERS_ZAP4)
	{
		if(slot-m_Gilles_CoreZap<0)
			return NULL;
		if(m_ItemSlot[slot-m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot-m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_OUSTERS_STONE)
				return MSlotItemManager::GetItem( (BYTE)(slot-m_Gilles_CoreZap) ); 
		}
	}
	
	return NULL;
}

const bool	MOustersGear::IsHasAllCoreZap(int CoreZapType) const	
{
	BYTE bType[4] = {0,};
	if(CoreZapType == 0) 
	{
		MItem* pItem;
		for(int i = 0; i<4; i++)
		{
			pItem = m_ItemSlot[GEAR_OUSTERS_ZAP1+i];
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

const bool	MOustersGear::IsCloseBloodBibleSlot(int slot) const	
{
	if(slot>=GEAR_OUSTERS_BLOOD_BIBLE1 && slot<=GEAR_OUSTERS_BLOOD_BIBLE6)
	{
		int TempSlot = slot - GEAR_OUSTERS_BLOOD_BIBLE1 + 1;
		if(TempSlot > m_bBloodBibleOpenCount)
			return true;
	}
	return false;
}
