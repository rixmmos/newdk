	//----------------------------------------------------------------------
// MSlayerGear.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 
//----------------------------------------------------------------------


#ifndef	__MSLAYERGEAR_H__
#define	__MSLAYERGEAR_H__

#include "MPlayerGear.h"
class MMagazine;


class MSlayerGear : public MPlayerGear	{
	public :
		//----------------------------------------------------
		
		//----------------------------------------------------
		
		//----------------------------------------------------
		enum GEAR_SLAYER 
		{
			GEAR_SLAYER_HELM = 0,			
			GEAR_SLAYER_NECKLACE,			
			GEAR_SLAYER_COAT,				
			GEAR_SLAYER_LEFTHAND,			
			GEAR_SLAYER_RIGHTHAND,			
			GEAR_SLAYER_GLOVE,				
			GEAR_SLAYER_BELT,				
			GEAR_SLAYER_TROUSER,			
			GEAR_SLAYER_BRACELET1,			
			GEAR_SLAYER_BRACELET2,			
			GEAR_SLAYER_RING1,				
			GEAR_SLAYER_RING2,				
			GEAR_SLAYER_RING3,				
			GEAR_SLAYER_RING4,				
			GEAR_SLAYER_SHOES,				
		
			GEAR_SLAYER_ZAP1,				
			GEAR_SLAYER_ZAP2,				
			GEAR_SLAYER_ZAP3,				
			GEAR_SLAYER_ZAP4,				

			GEAR_SLAYER_PDA,			
	        GEAR_SLAYER_SHOULDER,			

			GEAR_SLAYER_BLOOD_BIBLE1,				
			GEAR_SLAYER_BLOOD_BIBLE2,				
			GEAR_SLAYER_BLOOD_BIBLE3,				
			GEAR_SLAYER_BLOOD_BIBLE4,				
			GEAR_SLAYER_BLOOD_BIBLE5,				
			GEAR_SLAYER_BLOOD_BIBLE6,				

			MAX_GEAR_SLAYER
		};

		//----------------------------------------------------
		
		//----------------------------------------------------
		inline static bool		IsGearSlotHelm(MItem* pItem);
		inline static bool		IsGearSlotNecklace(MItem* pItem);
		inline static bool		IsGearSlotCoat(MItem* pItem);
		inline static bool		IsGearSlotLeftHand(MItem* pItem);
		inline static bool		IsGearSlotRightHand(MItem* pItem);
		inline static bool		IsGearSlotGlove(MItem* pItem);
		inline static bool		IsGearSlotBelt(MItem* pItem);
		inline static bool		IsGearSlotTrouser(MItem* pItem);
		inline static bool		IsGearSlotBracelet(MItem* pItem);
		inline static bool		IsGearSlotRing(MItem* pItem);
		inline static bool		IsGearSlotShoes(MItem* pItem);
		inline static bool		IsGearSlotCoreZap(MItem* pItem);
		inline static bool		IsGearSlotBloodBible(MItem* pItem);
		inline static bool		IsGearSlotPDA(MItem* pItem);
		inline static bool		IsGearSlotShoulder(MItem* pItem);
		


	public :
		MSlayerGear();
		~MSlayerGear();

		//------------------------------------------------
		
		//------------------------------------------------
		void			Init();

		//------------------------------------------------
		
		//------------------------------------------------
		bool			AddItem(MItem* pItem);
		bool			AddItem(MItem* pItem, GEAR_SLAYER n);

		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			GetItem(GEAR_SLAYER n) const { return MSlotItemManager::GetItem( (BYTE)n ); }
		
		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			RemoveItem(GEAR_SLAYER n);

		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			RemoveItem(TYPE_OBJECTID id);

		//------------------------------------------------
		
		//------------------------------------------------
		bool			ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);	

		//------------------------------------------------
		
		
		//------------------------------------------------
		bool			CanReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);

		//------------------------------------------------------		
		// Get FitSlot
		//------------------------------------------------------		
		bool			GetFitSlot(MItem* pItem, GEAR_SLAYER& n);	

		//------------------------------------------------------
		
		//------------------------------------------------------
		void			CheckAffectStatus(MItem* pItem);	

		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			GetGearItem_PickUp(int &n) const ;
		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			GetGearCoreZapItem(int slot) const	;
		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			GetGearCoreZapedItem(int slot) const	;
		//------------------------------------------------
		
		//------------------------------------------------
		const bool			IsHasAllCoreZap(int CoreZapType) const	;

		//------------------------------------------------
		
		//------------------------------------------------
		void			SetBloodBibleOpenSlot(int Num) { m_bBloodBibleOpenCount = Num;	}
		BYTE			GetBloodBibleOpenSlot() { return m_bBloodBibleOpenCount;	}
		const bool		IsCloseBloodBibleSlot(int slot) const	;
	protected :
		//------------------------------------------------
		
		//------------------------------------------------
		typedef bool (*FUNCTION_GEARSLOT_CHECK)(MItem*);

		//------------------------------------------------
		
		//------------------------------------------------
		static FUNCTION_GEARSLOT_CHECK		s_GearSlotCheckTable[MAX_GEAR_SLAYER];

		BYTE		m_Gilles_CoreZap;
		BYTE		m_bBloodBibleOpenCount;
};


extern MSlayerGear*		g_pSlayerGear;

extern MMagazine*		g_pCurrentMagazine;

#endif

