//----------------------------------------------------------------------
// MVampireGear.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MVAMPIREGEAR_H__
#define	__MVAMPIREGEAR_H__

#include "MPlayerGear.h"

class MVampireGear : public MPlayerGear	{
	public :
		//----------------------------------------------------
		
		//----------------------------------------------------
		
		//----------------------------------------------------
		enum GEAR_VAMPIRE 
		{
			GEAR_VAMPIRE_NECKLACE,			
			GEAR_VAMPIRE_COAT,				
			GEAR_VAMPIRE_BRACELET1,			
			GEAR_VAMPIRE_BRACELET2,			
			GEAR_VAMPIRE_RING1,				
			GEAR_VAMPIRE_RING2,				
			GEAR_VAMPIRE_RING3,				
			GEAR_VAMPIRE_RING4,				
			GEAR_VAMPIRE_EARRING1,			
			GEAR_VAMPIRE_EARRING2,			
			GEAR_VAMPIRE_LEFTHAND,			
			GEAR_VAMPIRE_RIGHTHAND,			
			GEAR_VAMPIRE_AMULET1,			
			GEAR_VAMPIRE_AMULET2,			
			GEAR_VAMPIRE_AMULET3,			
			GEAR_VAMPIRE_AMULET4,			
			
			GEAR_VAMPIRE_ZAP1,				
			GEAR_VAMPIRE_ZAP2,				
			GEAR_VAMPIRE_ZAP3,				
			GEAR_VAMPIRE_ZAP4,				

			GEAR_VAMPIRE_PERSONA,		
			GEAR_VAMPIRE_DERMIS,		
			
 			
			GEAR_VAMPIRE_BLOOD_BIBLE1,				
			GEAR_VAMPIRE_BLOOD_BIBLE2,				
			GEAR_VAMPIRE_BLOOD_BIBLE3,				
			GEAR_VAMPIRE_BLOOD_BIBLE4,				
			GEAR_VAMPIRE_BLOOD_BIBLE5,				
			GEAR_VAMPIRE_BLOOD_BIBLE6,				
			
			MAX_GEAR_VAMPIRE
		};

		//----------------------------------------------------
		
		//----------------------------------------------------
		inline static bool		IsGearSlotNecklace(MItem* pItem);
		inline static bool		IsGearSlotCoat(MItem* pItem);
		inline static bool		IsGearSlotBracelet(MItem* pItem);
		inline static bool		IsGearSlotRing(MItem* pItem);
		inline static bool		IsGearSlotEarRing(MItem* pItem);
		inline static bool		IsGearSlotLeftHand(MItem* pItem);
		inline static bool		IsGearSlotRightHand(MItem* pItem);
		inline static bool		IsGearSlotAmulet(MItem* pItem);
		inline static bool		IsGearSlotCoreZap(MItem* pItem);
		inline static bool		IsGearSlotBloodBible(MItem* pItem);
		inline static bool		IsGearSlotDermis(MItem* pItem);
		inline static bool		IsGearSlotPersona(MItem* pItem);
		


	public :
		MVampireGear();
		~MVampireGear();

		//------------------------------------------------
		
		//------------------------------------------------
		void			Init();

		//------------------------------------------------
		
		//------------------------------------------------
		bool			AddItem(MItem* pItem);
		bool			AddItem(MItem* pItem, GEAR_VAMPIRE n);

		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			GetItem(GEAR_VAMPIRE n ) const	{ return MSlotItemManager::GetItem( (BYTE)n ); }
		
		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			RemoveItem(GEAR_VAMPIRE n);

		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			RemoveItem(TYPE_OBJECTID id);

		//------------------------------------------------
		
		//------------------------------------------------
		bool			ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);

		//------------------------------------------------
		
		
		//------------------------------------------------
		bool			CanReplaceItem(MItem* pItem, GEAR_VAMPIRE n, MItem*& pOldItem);

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
		static FUNCTION_GEARSLOT_CHECK		s_GearSlotCheckTable[MAX_GEAR_VAMPIRE];

		BYTE			m_Gilles_CoreZap;
		BYTE		m_bBloodBibleOpenCount;
};


extern MVampireGear	*	g_pVampireGear;

#endif

