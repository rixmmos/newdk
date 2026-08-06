//----------------------------------------------------------------------
// MOustersGear.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MOUSTERSGEAR_H__
#define	__MOUSTERSGEAR_H__

#include "MPlayerGear.h"

class MOustersGear : public MPlayerGear	{
	public :
		//----------------------------------------------------
		
		//----------------------------------------------------
		
		//----------------------------------------------------
		enum GEAR_OUSTERS
		{
			GEAR_OUSTERS_CIRCLET,			
			GEAR_OUSTERS_COAT,				
			GEAR_OUSTERS_LEFTHAND,			
			GEAR_OUSTERS_RIGHTHAND,			
			GEAR_OUSTERS_BOOTS,				
			GEAR_OUSTERS_ARMSBAND1,			
			GEAR_OUSTERS_ARMSBAND2,			
			GEAR_OUSTERS_RING1,				
			GEAR_OUSTERS_RING2,				
			GEAR_OUSTERS_PENDENT1,			
			GEAR_OUSTERS_PENDENT2,			
			GEAR_OUSTERS_PENDENT3,			
			GEAR_OUSTERS_STONE1,			
			GEAR_OUSTERS_STONE2,			
			GEAR_OUSTERS_STONE3,			
			GEAR_OUSTERS_STONE4,			
			
			GEAR_OUSTERS_ZAP1,				
			GEAR_OUSTERS_ZAP2,				
			GEAR_OUSTERS_ZAP3,				
			GEAR_OUSTERS_ZAP4,				
			//by csm 2004 12.31 add item slot
			GEAR_OUSTERS_FASCIA,			
	        GEAR_OUSTERS_MITTEN,			

			GEAR_OUSTERS_BLOOD_BIBLE1,				
			GEAR_OUSTERS_BLOOD_BIBLE2,				
			GEAR_OUSTERS_BLOOD_BIBLE3,				
			GEAR_OUSTERS_BLOOD_BIBLE4,				
			GEAR_OUSTERS_BLOOD_BIBLE5,				
			GEAR_OUSTERS_BLOOD_BIBLE6,				
			MAX_GEAR_OUSTERS
		};	

		//----------------------------------------------------
		
		//----------------------------------------------------
		inline static bool		IsGearSlotCirclet(MItem* pItem);
		inline static bool		IsGearSlotCoat(MItem* pItem);
		inline static bool		IsGearSlotLeftHand(MItem* pItem);
		inline static bool		IsGearSlotRightHand(MItem* pItem);
		inline static bool		IsGearSlotBoots(MItem* pItem);
		inline static bool		IsGearSlotArmsBand(MItem* pItem);
		inline static bool		IsGearSlotRing(MItem* pItem);
		inline static bool		IsGearSlotPendent(MItem* pItem);
		inline static bool		IsGearSlotStone(MItem* pItem);
		inline static bool		IsGearSlotCoreZap(MItem* pItem);
		inline static bool		IsGearSlotBloodBible(MItem* pItem);
		inline static bool		IsGearSlotFascia(MItem* pItem);
		inline static bool		IsGearSlotMitten(MItem* pItem);


	public :
		MOustersGear();
		~MOustersGear();

		//------------------------------------------------
		
		//------------------------------------------------
		void			Init();

		//------------------------------------------------
		
		//------------------------------------------------
		bool			AddItem(MItem* pItem);
		bool			AddItem(MItem* pItem, GEAR_OUSTERS n);

		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			GetItem(GEAR_OUSTERS n) const { return MSlotItemManager::GetItem( (BYTE)n ); }
		
		//------------------------------------------------
		
		//------------------------------------------------
		MItem*			RemoveItem(GEAR_OUSTERS n);

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
		static FUNCTION_GEARSLOT_CHECK		s_GearSlotCheckTable[MAX_GEAR_OUSTERS];

		BYTE		m_Gilles_CoreZap;

		BYTE		m_bBloodBibleOpenCount;

};


extern MOustersGear	*	g_pOustersGear;

#endif

