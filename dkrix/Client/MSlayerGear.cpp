//----------------------------------------------------------------------
// MSlayerGear.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MItem.h"
#include "MSlayerGear.h"
#include "MQuickSlot.h"

#ifdef __GAME_CLIENT__
	#include "UIFunction.h"
	#include "ClientFunction.h"
	#include "MPlayer.h"
	#include "DebugInfo.h"
#endif
#include "MTimeItemManager.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MSlayerGear*		g_pSlayerGear = NULL;

MMagazine*		g_pCurrentMagazine = NULL;

//----------------------------------------------------------------------
// static
//----------------------------------------------------------------------
MSlayerGear::FUNCTION_GEARSLOT_CHECK
MSlayerGear::s_GearSlotCheckTable[MAX_GEAR_SLAYER] =
{
	IsGearSlotHelm,				
	IsGearSlotNecklace,			
	IsGearSlotCoat,				
	IsGearSlotLeftHand,			
	IsGearSlotRightHand,		
	IsGearSlotGlove,			
	IsGearSlotBelt,				
	IsGearSlotTrouser,			
	IsGearSlotBracelet,			
	IsGearSlotBracelet,			
	IsGearSlotRing,				
	IsGearSlotRing,				
	IsGearSlotRing,				
	IsGearSlotRing,				
	IsGearSlotShoes,			
	IsGearSlotCoreZap,			
	IsGearSlotCoreZap,			
	IsGearSlotCoreZap,			
	IsGearSlotCoreZap,			
	IsGearSlotPDA,				// PDA
	IsGearSlotShoulder,			
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		
	IsGearSlotBloodBible,		

};

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		MSlayerGear::IsGearSlotHelm(MItem* pItem)		{ return pItem->IsGearSlotHelm(); }		
bool		MSlayerGear::IsGearSlotNecklace(MItem* pItem) 	{ return pItem->IsGearSlotNecklace(); }	
bool		MSlayerGear::IsGearSlotCoat(MItem* pItem) 		{ return pItem->IsGearSlotCoat(); }		
bool		MSlayerGear::IsGearSlotLeftHand(MItem* pItem)	{ return pItem->IsGearSlotLeftHand(); }	
bool		MSlayerGear::IsGearSlotRightHand(MItem* pItem)	{ return pItem->IsGearSlotRightHand(); }	
bool		MSlayerGear::IsGearSlotGlove(MItem* pItem) 		{ return pItem->IsGearSlotGlove(); }	
bool		MSlayerGear::IsGearSlotBelt(MItem* pItem) 		{ return pItem->IsGearSlotBelt(); }		
bool		MSlayerGear::IsGearSlotTrouser(MItem* pItem) 	{ return pItem->IsGearSlotTrouser(); }	
bool		MSlayerGear::IsGearSlotBracelet(MItem* pItem) 	{ return pItem->IsGearSlotBracelet(); }	
bool		MSlayerGear::IsGearSlotRing(MItem* pItem) 		{ return pItem->IsGearSlotRing(); }		
bool		MSlayerGear::IsGearSlotShoes(MItem* pItem)		{ return pItem->IsGearSlotShoes(); }	
bool		MSlayerGear::IsGearSlotCoreZap(MItem* pItem)	{ return pItem->IsGearSlotCoreZap(); }	
bool		MSlayerGear::IsGearSlotPDA(MItem* pItem)		{ return pItem->IsGearSlotPDA(); }// PDA
bool		MSlayerGear::IsGearSlotShoulder(MItem* pItem)	{ return pItem->IsGearSlotShoulder(); }// Shoulder

bool		MSlayerGear::IsGearSlotBloodBible(MItem* pItem)	{ return pItem->IsGearSlotBloodBible(); }


//----------------------------------------------------------------------
//
// constructor
// 
//----------------------------------------------------------------------
MSlayerGear::MSlayerGear()
{
	g_pQuickSlot = NULL;
	m_bBloodBibleOpenCount = 0;
}

MSlayerGear::~MSlayerGear()
{
	g_pQuickSlot = NULL;
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
MSlayerGear::Init()
{
	
	MPlayerGear::Init(MAX_GEAR_SLAYER);

	g_pCurrentMagazine = NULL;
	m_Gilles_CoreZap = 5;
	m_bBloodBibleOpenCount = 0;
}


//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void			
MSlayerGear::CheckAffectStatus(MItem* pItem)
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
MSlayerGear::AddItem(MItem* pItem)
{
	for (int i=0; i<MAX_GEAR_SLAYER; i++)
	{
		
		if (AddItem( pItem, (enum GEAR_SLAYER)i ))
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------
// Can Replace Item
//----------------------------------------------------------------------




//----------------------------------------------------------------------
bool			
MSlayerGear::CanReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
{
	
	if(pItem->IsAffectStatus() == false && !pItem->IsQuestItem() )
		return false;

	pOldItem = NULL;

	//---------------------------------------------------------
	
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsSlayerItem())
	{
		return false;
	}
	
	if(n>=GEAR_SLAYER_BLOOD_BIBLE1 && n<=GEAR_SLAYER_BLOOD_BIBLE6) 
		return false;
	
	
	if(n == GEAR_SLAYER_PDA)
	{
		if(pItem->IsGearSlotPDA() == true)
		{
			if (m_ItemSlot[GEAR_SLAYER_PDA]==NULL)
				return true;
			else
			{
				pOldItem = m_ItemSlot[GEAR_SLAYER_PDA];
				return true;
			}
		}

	}

	if(n == GEAR_SLAYER_SHOULDER)
	{
		if(pItem->IsGearSlotShoulder()== true)
		{
			
			if (m_ItemSlot[GEAR_SLAYER_SHOULDER]==NULL)
				return true;
			else
			{
				pOldItem = m_ItemSlot[GEAR_SLAYER_SHOULDER];
				return true;
			}
		}
		
	}
	//-------------------------------------------------
	//
	
	//
	//-------------------------------------------------
	if (n==GEAR_SLAYER_LEFTHAND || n==GEAR_SLAYER_RIGHTHAND)	
	{
		//-------------------------------------------------
		
		//-------------------------------------------------
		if (pItem->IsGearSlotTwoHand())
		{			
			//-------------------------------------------------
			
			//-------------------------------------------------			
			if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_SLAYER_LEFTHAND]==NULL)
			{
				return true;
			}
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_SLAYER_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]==m_ItemSlot[GEAR_SLAYER_LEFTHAND])
				{
					pOldItem = m_ItemSlot[GEAR_SLAYER_RIGHTHAND];
					return true;
				}
				
				return false;				
			}

			//-------------------------------------------------
			
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]!=NULL)
			{
				pOldItem = m_ItemSlot[GEAR_SLAYER_RIGHTHAND];
				return true;
			}

			//-------------------------------------------------
			
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_SLAYER_LEFTHAND]!=NULL)
			{			
				pOldItem = m_ItemSlot[GEAR_SLAYER_LEFTHAND];
				return true;
			}

			
			return false;
		}

		//-------------------------------------------------
		
		//-------------------------------------------------				
		else if (pItem->IsGearSlotRightHand())
		{
			
			pOldItem = m_ItemSlot[GEAR_SLAYER_RIGHTHAND];
			return true;
		}
		
		//-------------------------------------------------				
		
		//-------------------------------------------------
		else if (pItem->IsGearSlotLeftHand())
		{
			pOldItem = m_ItemSlot[GEAR_SLAYER_LEFTHAND];
			return true;
		}

		
		return false;
	}
	 
	if (n >= GEAR_SLAYER_RING1 && n <= GEAR_SLAYER_RING4)	
	{
		if (pItem->IsGearSlotRing())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]==NULL) 
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotCoreZap())
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
	else if(n >= GEAR_SLAYER_ZAP1 && n <= GEAR_SLAYER_ZAP4) 
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
MSlayerGear::AddItem(MItem* pItem, GEAR_SLAYER n)
{
	//---------------------------------------------------------
	
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsSlayerItem())
	{
		return false;
	}

	//-------------------------------------------------
	
	//-------------------------------------------------
	if ((n==GEAR_SLAYER_LEFTHAND || n==GEAR_SLAYER_RIGHTHAND) &&
		pItem->IsGearSlotTwoHand())
	{
		
		if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]==NULL
				&& m_ItemSlot[GEAR_SLAYER_LEFTHAND]==NULL)
		{
			
			if (MPlayerGear::AddItem( pItem, GEAR_SLAYER_RIGHTHAND ))
			{
				
				m_ItemSlot[GEAR_SLAYER_LEFTHAND] = pItem;

				//-------------------------------------------------
				
				//-------------------------------------------------
				if (pItem->IsGunItem())
				{
					MGunItem* pGunItem = (MGunItem*)pItem;
					g_pCurrentMagazine = pGunItem->GetMagazine();
				}

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
	 
	if (n >= GEAR_SLAYER_RING1 && n <= GEAR_SLAYER_RING4)	
	{
		if (pItem->IsGearSlotRing())
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
		if (pItem->IsGearSlotCoreZap())
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

		//--------------------------------------------
		
		//--------------------------------------------
		if (bAdd)
		{
			//--------------------------------------------
			
			//--------------------------------------------
			
			//--------------------------------------------
			if (n==GEAR_SLAYER_BELT)
			{
				if (pItem->GetItemClass()==ITEM_CLASS_BELT)
				{
					g_pQuickSlot = (MBelt*)pItem;
					
					#ifdef __GAME_CLIENT__
						UI_ResetQuickItemSlot();
					#endif
				}
				else
				{
					
				}
			}
			else
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				if (pItem->IsGunItem())
				{
					MGunItem* pGunItem = (MGunItem*)pItem;
					g_pCurrentMagazine = pGunItem->GetMagazine();
				}
			}

			//-------------------------------------------------
			
			//-------------------------------------------------
			#ifdef __GAME_CLIENT__
				PlaySound( pItem->GetGearSoundID() );
				g_pPlayer->CalculateStatus();	
			#endif

			return true;
		}
		
		//--------------------------------------------
		
		//--------------------------------------------
		return false;		
	}

	return false;
}

//----------------------------------------------------------------------
// Remove Item ( n)
//----------------------------------------------------------------------
MItem*			
MSlayerGear::RemoveItem(GEAR_SLAYER n)
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
		n = GEAR_SLAYER_RIGHTHAND;
	}
	 
	if(n>=GEAR_SLAYER_RING1 && n<= GEAR_SLAYER_RING4) 
	{
		if(NULL != m_ItemSlot[n+m_Gilles_CoreZap]) 
			n = (GEAR_SLAYER)(n + m_Gilles_CoreZap); 
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
	
	//-----------------------------------------------------
	if (n==GEAR_SLAYER_BELT)
	{
		g_pQuickSlot = NULL;

		#ifdef __GAME_CLIENT__
			g_pPlayer->CalculateStatus();
		#endif

		return pItem;
	}

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		m_ItemSlot[GEAR_SLAYER_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = NULL;
	}
	
	//-------------------------------------------------
	
	//-------------------------------------------------
	if (pItem->IsGunItem())
	{
		g_pCurrentMagazine = NULL;
	}

	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	return pItem;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*			
MSlayerGear::RemoveItem(TYPE_OBJECTID id)
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
	
	//-----------------------------------------------------
	if (pItem->GetItemClass()==ITEM_CLASS_BELT)
	{
		g_pQuickSlot = NULL;

		#ifdef __GAME_CLIENT__
			g_pPlayer->CalculateStatus();
		#endif

		return pItem;
	}

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		m_ItemSlot[GEAR_SLAYER_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = NULL;
	}

	//-------------------------------------------------
	
	//-------------------------------------------------
	if (pItem->IsGunItem())
	{
		g_pCurrentMagazine = NULL;
	}

	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	return pItem;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
MSlayerGear::ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
{
	//---------------------------------------------------------
	
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsSlayerItem())
	{
		pOldItem = NULL;
		return false;
	}

	
	if(n == GEAR_SLAYER_PDA)
	{
		if(pItem->IsGearSlotPDA()== true)
		{
		if (m_ItemSlot[GEAR_SLAYER_PDA]==NULL)
		{
			MPlayerGear::ReplaceItem( pItem, GEAR_SLAYER_PDA, pOldItem );
			return true;
		}
		else
			return false;
		}
	}

	if(n == GEAR_SLAYER_SHOULDER)
	{
		if(pItem->IsGearSlotShoulder()== true)
		{
			if (m_ItemSlot[GEAR_SLAYER_SHOULDER]==NULL)
			{
				MPlayerGear::ReplaceItem( pItem, GEAR_SLAYER_SHOULDER, pOldItem );
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
	if (n==GEAR_SLAYER_LEFTHAND || n==GEAR_SLAYER_RIGHTHAND)	
	{
		bool bAdd = false;
			
		//-------------------------------------------------
		
		//-------------------------------------------------
		if (pItem->IsGearSlotTwoHand())
		{			
			//-------------------------------------------------
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_SLAYER_LEFTHAND]==NULL)
			{
				
				pOldItem = NULL;
				if (MPlayerGear::AddItem( pItem, GEAR_SLAYER_RIGHTHAND ))
				{
					//-------------------------------------------------
					
					//-------------------------------------------------
					CheckItemStatus( pItem, GEAR_SLAYER_RIGHTHAND );

					pItem->SetItemSlot( GEAR_SLAYER_RIGHTHAND );
					m_ItemSlot[GEAR_SLAYER_LEFTHAND] = pItem;
					m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = pItem;

					//-------------------------------------------------
					
					//-------------------------------------------------
					if (pItem->IsGunItem())
					{
						MGunItem* pGunItem = (MGunItem*)pItem;
						g_pCurrentMagazine = pGunItem->GetMagazine();

						//------------------------------------------
						
						//------------------------------------------
						if (g_pCurrentMagazine==NULL)
						{
							MMagazine* pMagazine = (MMagazine*)MItem::NewItem( (ITEM_CLASS)ITEM_CLASS_MAGAZINE );

							
							pMagazine->SetID( 0 );

							
							for (int j=0; j<(*g_pItemTable)[ITEM_CLASS_MAGAZINE].GetSize(); j++)			
							{
								pMagazine->SetItemType(	j );

								if (pMagazine->IsInsertToItem( pItem ))
								{
									break;
								}
							}

							
							pMagazine->ClearItemOption();
						
							
							pMagazine->SetNumber( 0 );

							
							pGunItem->SetMagazine( pMagazine );
						}
					}				

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
			if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_SLAYER_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]==m_ItemSlot[GEAR_SLAYER_LEFTHAND])
				{
					
					if (MPlayerGear::ReplaceItem( pItem, 
													GEAR_SLAYER_RIGHTHAND, 
													pOldItem ))
					{
						//-------------------------------------------------
						
						//-------------------------------------------------
						CheckItemStatus( pItem, GEAR_SLAYER_RIGHTHAND );

						//pItem->SetItemSlot( GEAR_SLAYER_RIGHTHAND );
						m_ItemSlot[GEAR_SLAYER_LEFTHAND] = pItem;
						//m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = pItem;

						//-------------------------------------------------
						
						//-------------------------------------------------
						if (pOldItem!=NULL && pOldItem->IsGunItem())
						{
							g_pCurrentMagazine = NULL;
						}

						//-------------------------------------------------
						
						//-------------------------------------------------
						if (pItem->IsGunItem())
						{
							MGunItem* pGunItem = (MGunItem*)pItem;
							g_pCurrentMagazine = pGunItem->GetMagazine();
						}
					
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
			else if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_SLAYER_RIGHTHAND, 
													pOldItem );
			}		

			//-------------------------------------------------
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_SLAYER_LEFTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_SLAYER_LEFTHAND, 
													pOldItem );					
			}

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			if (bAdd)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_SLAYER_RIGHTHAND );
				
				pItem->SetItemSlot( GEAR_SLAYER_RIGHTHAND );
				m_ItemSlot[GEAR_SLAYER_LEFTHAND] = pItem;
				m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = pItem;

				//-------------------------------------------------
				
				//-------------------------------------------------
				if (pOldItem!=NULL && pOldItem->IsGunItem())
				{
					g_pCurrentMagazine = NULL;
				}

				//-------------------------------------------------
				
				//-------------------------------------------------
				if (pItem->IsGunItem())
				{
					MGunItem* pGunItem = (MGunItem*)pItem;
					g_pCurrentMagazine = pGunItem->GetMagazine();
				}				
		
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
													GEAR_SLAYER_RIGHTHAND, 
													pOldItem );

			//-------------------------------------------------
			
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					
					m_ItemSlot[GEAR_SLAYER_LEFTHAND] = NULL;
				}

				//-------------------------------------------------
				
				//-------------------------------------------------
				if (pOldItem->IsGunItem())
				{
					g_pCurrentMagazine = NULL;
				}
			}

			if (bAdd)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_SLAYER_RIGHTHAND );

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
													GEAR_SLAYER_LEFTHAND, 
													pOldItem );
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					
					m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = NULL;
				}

				//-------------------------------------------------
				
				//-------------------------------------------------
				if (pOldItem->IsGunItem())
				{
					g_pCurrentMagazine = NULL;
				}
			}

			if (bAdd)
			{
				//-------------------------------------------------
				
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_SLAYER_LEFTHAND );
				
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
	 
	if (n >= GEAR_SLAYER_RING1 && n <= GEAR_SLAYER_RING4)	
	{
		if (pItem->IsGearSlotRing())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]!=NULL) 
				return false;
		}
		else
		if (pItem->IsGearSlotCoreZap())
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
	if (pItem->IsGearItem()	&&				
		(*s_GearSlotCheckTable[n])(pItem))	
	{
		bool bAdd;
		//----------------------------------------------------
		
		//----------------------------------------------------	
		if (m_ItemSlot[n]==NULL)				
		{
			pOldItem = NULL;
			
			bAdd = MPlayerGear::AddItem( pItem, n );			
		}
		//----------------------------------------------------	
		
		//----------------------------------------------------	
		else
		{
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			bAdd = MPlayerGear::ReplaceItem( pItem, n, pOldItem );	
			
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (bAdd)
			{
				CheckItemStatus( pItem, n );
			}			
		}

		//--------------------------------------------
		
		//--------------------------------------------
		if (bAdd)
		{
			//--------------------------------------------
			
			//--------------------------------------------
			
			//--------------------------------------------
			if (n==GEAR_SLAYER_BELT)
			{
				if (pItem->GetItemClass()==ITEM_CLASS_BELT)
				{
					g_pQuickSlot = (MBelt*)pItem;				
				}
				else
				{
					
				}
			}
			else
			{
				//-------------------------------------------------
				
				
				
				//-------------------------------------------------
				if (pOldItem!=NULL && pOldItem->IsGunItem())
				{
					g_pCurrentMagazine = NULL;
				}

				//-------------------------------------------------
				
				//-------------------------------------------------
				if (pItem->IsGunItem())
				{
					MGunItem* pGunItem = (MGunItem*)pItem;
					g_pCurrentMagazine = pGunItem->GetMagazine();
				}	
			}

			//-------------------------------------------------
			
			//-------------------------------------------------
			#ifdef __GAME_CLIENT__
				PlaySound( pItem->GetGearSoundID() );
				g_pPlayer->CalculateStatus();
			#endif

			return true;
		}
	}

	return false;
}

MItem*	MSlayerGear::GetGearItem_PickUp(int &n) const	
{
	int Slot = n;

//		return NULL;
	if(Slot>=GEAR_SLAYER_RING1 && Slot<=GEAR_SLAYER_RING4)
	{
		if(m_ItemSlot[Slot+m_Gilles_CoreZap]!=NULL)
		{
			n = n+m_Gilles_CoreZap;
			return MSlotItemManager::GetItem( (BYTE)(Slot+m_Gilles_CoreZap) ); 
		}
	}
	
	return MSlotItemManager::GetItem( (BYTE)Slot ); 
}


MItem*	MSlayerGear::GetGearCoreZapItem(int slot) const	
{
	if(slot>=GEAR_SLAYER_RING1 && slot<=GEAR_SLAYER_RING4)
	{
		if(m_ItemSlot[slot+m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot+m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_CORE_ZAP)
				return MSlotItemManager::GetItem( (BYTE)(slot+m_Gilles_CoreZap) ); 
		}
	}
	
	return NULL;
}



MItem*	MSlayerGear::GetGearCoreZapedItem(int slot) const	
{
	if(slot>=GEAR_SLAYER_ZAP1 && slot<=GEAR_SLAYER_ZAP4)
	{
		if(slot-m_Gilles_CoreZap<0)
			return NULL;
		if(m_ItemSlot[slot-m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot-m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_RING)
				return MSlotItemManager::GetItem( (BYTE)(slot-m_Gilles_CoreZap) ); 
		}
	}
	
	return NULL;
}

const bool	MSlayerGear::IsHasAllCoreZap(int CoreZapType) const	
{
	BYTE bType[4] = {0,};
	if(CoreZapType == 0) 
	{
		MItem* pItem;
		for(int i = 0; i<4; i++)
		{
			pItem = m_ItemSlot[GEAR_SLAYER_ZAP1+i];
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

const bool	MSlayerGear::IsCloseBloodBibleSlot(int slot) const	
{
	if(slot>=GEAR_SLAYER_BLOOD_BIBLE1 && slot<=GEAR_SLAYER_BLOOD_BIBLE6)
	{
			int TempSlot = slot - GEAR_SLAYER_BLOOD_BIBLE1 + 1;
		if(TempSlot > m_bBloodBibleOpenCount)
			return true;
	}
	return false;
}
