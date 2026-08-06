//----------------------------------------------------------------------
// MItem.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
/*

 [ class hierarchy ]


  ITEM	- BELT
			- MOTORCYCLE
			- CORPSE

		- PILE_ITEM
			- POTION			
			- MAGAZINE

		- GEAR_ITEM
			- RING
			- BRACELET
			- NECKLACE
			- SHOES
			- SWORD
			- BLADE
			- SHIELD
			- CROSS
			- GLOVE
			- HELM
			- GUN_SG
			- GUN_SMG
			- GUN_AR
			- GUN_TR

		- WATER
		- HOLYWATER
		- ETC					
		- MOTORCYCLE(-I)
		- KEY(+V)
		- BELT	
		- BOMB_MATERIAL
		- LEARNINGITEM
		- MONEY(+V)
		- BOMB
		- MINE

		- VAMPIRE_ITEM(+V)
			- VAMPIRE_GEAR_ITEM
				- VAMPIRE_EARRING
				- VAMPIRE_RING
				- VAMPIRE_BRACELET
				- VAMPIRE_NECKLACE
				- VAMPIRE_COAT
				- VAMPIRE_SHOES

*/

#ifndef	__MITEM_H__
#define	__MITEM_H__

#include <string>
#include "MObject.h"
#include "MITemTable.h"
#include "MGridItemManager.h"
#include "MSlotItemManager.h"
#include "ItemClassDef.h"
//#include "SkillDef.h"
//#include "AddonDef.h"
#include "CAnimationFrame.h"
#include "RaceType.h"
class MCreature;


#define	MAX_DROP_COUNT					6


class MItem : public MObject, public CAnimationFrame {
	public :
		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		enum ITEM_GEAR_SLOT
		{	
			SLOT_HELM = 0,			
			SLOT_NECKLACE,			
			SLOT_COAT,				
			SLOT_LEFTHAND,			
			SLOT_RIGHTHAND,			
			SLOT_TWOHAND,			
			SLOT_BELT,				
			SLOT_BRACELET,			
			SLOT_TROUSER,			
			SLOT_RING,				
			SLOT_SHOES,				
			SLOT_GLOVE,				
			SLOT_PDA,				// PDA
			SLOT_SHOULDER,			

			// vampire 
			SLOT_VAMPIRE_RING,
			SLOT_VAMPIRE_BRACELET,
			SLOT_VAMPIRE_NECKLACE,
			SLOT_VAMPIRE_COAT,
			SLOT_VAMPIRE_EARRING,
			SLOT_VAMPIRE_LEFTHAND,
			SLOT_VAMPIRE_RIGHTHAND,
			SLOT_VAMPIRE_TWOHAND,			
			SLOT_VAMPIRE_AMULET,
			SLOT_VAMPIRE_DERMIS,			
			SLOT_VAMPIRE_PERSONA,			

			// ousters
			SLOT_OUSTERS_CIRCLET,			
			SLOT_OUSTERS_COAT,				
			SLOT_OUSTERS_LEFTHAND,			
			SLOT_OUSTERS_RIGHTHAND,			
			SLOT_OUSTERS_TWOHAND,			
			SLOT_OUSTERS_BOOTS,				
			SLOT_OUSTERS_ARMSBAND,			
			SLOT_OUSTERS_RING,				
			SLOT_OUSTERS_PENDENT,			
			SLOT_OUSTERS_STONE,			
			SLOT_OUSTERS_FASCIA,			
			SLOT_OUSTERS_MITTEN,			


			
			SLOT_BLOOD_BIBLE,			 

			MAX_SLOT,				

			SLOT_NULL				
		};


	public :
		MItem();
		virtual ~MItem();

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		virtual ITEM_CLASS	GetItemClass() const				{ return ITEM_CLASS_NULL; }

		//-------------------------------------------------------
		// ItemType
		//-------------------------------------------------------
		virtual void		SetItemType(TYPE_ITEMTYPE type)		{ m_ItemType = type; }
		TYPE_ITEMTYPE		GetItemType() const					{ return m_ItemType; }

		//-------------------------------------------------------
		// ItemStyle
		//-------------------------------------------------------
		int					GetItemStyle() const				{ return (*g_pItemTable)[GetItemClass()][m_ItemType].ItemStyle; }
		int					IsNormalItem() const				{ return ( (*g_pItemTable)[GetItemClass()][m_ItemType].ItemStyle == 0 ); }
		int					IsUniqueItem() const				{ return ( (*g_pItemTable)[GetItemClass()][m_ItemType].ItemStyle == 1 ); }
		int					IsQuestItem() const;
		bool				IsSpecialColorItem() const;

		int					GetSpecialColorItemColorset();
		static int			GetSpecialColorItemColorset(unsigned short srcColor);
		static int			GetUniqueItemColorset();
		static int			GetQuestItemColorset();
		int					GetRareItemColorset();

		//-------------------------------------------------------
		// ItemOption
		//-------------------------------------------------------
		int									GetItemDefaultOptionCount() const	{ return (*g_pItemTable)[GetItemClass()][m_ItemType].DefaultOptionList.size(); }
		bool								IsEmptyItemDefaultOption() const	{ return (*g_pItemTable)[GetItemClass()][m_ItemType].DefaultOptionList.empty(); }
		const std::list<TYPE_ITEM_OPTION>&	GetItemDefaultOptionList() const	{ return (*g_pItemTable)[GetItemClass()][m_ItemType].DefaultOptionList; }		
		void								SetItemOptionList(const std::list<TYPE_ITEM_OPTION> &option_list)	{ m_ItemOptionList = option_list; }
		const std::list<TYPE_ITEM_OPTION>&	GetItemOptionList() const					{ return m_ItemOptionList; }		
		int									GetItemOptionListCount() const				{ return m_ItemOptionList.size(); }
		bool								IsEmptyItemOptionList() const				{ return m_ItemOptionList.empty(); }
		void								RemoveItemOption(TYPE_ITEM_OPTION option);
		void								AddItemOption(TYPE_ITEM_OPTION option);
		void								ChangeItemOption(TYPE_ITEM_OPTION ori_option, TYPE_ITEM_OPTION new_option);
		void								ClearItemOption()	{ m_ItemOptionList.clear(); }

		//-------------------------------------------------------
		// Slayer / Vampire / Ousters ?
		//-------------------------------------------------------
		int					IsSlayerItem() const			{ return ((*g_pItemTable)[GetItemClass()][m_ItemType].Race & FLAG_RACE_SLAYER); }
		int					IsVampireItem() const			{ return ((*g_pItemTable)[GetItemClass()][m_ItemType].Race & FLAG_RACE_VAMPIRE); }
		int					IsOustersItem() const			{ return ((*g_pItemTable)[GetItemClass()][m_ItemType].Race & FLAG_RACE_OUSTERS); }

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void				SetItemColorSet(WORD cs)		{ m_ItemColorSet = cs; }		
		WORD				GetItemColorSet() const			{ return m_ItemColorSet; }		

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		
		virtual bool		IsContainerItem() const		{ return false; }		

		
		virtual bool		IsPileItem() const				{ return false; }

		
		virtual bool		IsChargeItem() const			{ return false; }

		
		virtual bool		IsGearItem() const			{ return false; }

		
		virtual bool		IsAddonItem() const			{ return false; }

		
		virtual bool		IsBasicWeapon() const		{ return false; }

		
		virtual bool		IsGunItem() const			{ return false; }

		
		virtual bool		IsInventoryItem() const		{ return true; }
		
		
		
		virtual bool		IsQuickItem() const			{ return false; }

		
	#ifdef __TEST_SUB_INVENTORY__   
		virtual void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0)	{}
	#else
		virtual void		UseInventory()				{}
	#endif
		virtual void		UseQuickItem()				{}
		virtual void		UseGear()					{}


		
		virtual bool		IsInsertToItem(const MItem* pItem) const;

		
		virtual ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_NULL; }

		
		virtual bool		IsGearSlotHelm()	const			{ return false; }	
		virtual bool		IsGearSlotNecklace() const		{ return false; }	
		virtual bool		IsGearSlotCoat() const			{ return false; }	
		virtual bool		IsGearSlotLeftHand() const		{ return false; }	
		virtual bool		IsGearSlotRightHand() const		{ return false; }	
		virtual bool		IsGearSlotTwoHand() const		{ return false; }	
		virtual bool		IsGearSlotBelt() const			{ return false; }	
		virtual bool		IsGearSlotBracelet() const		{ return false; }	
		virtual bool		IsGearSlotTrouser() const		{ return false; }	
		virtual bool		IsGearSlotRing() const			{ return false; }	
		virtual bool		IsGearSlotShoes() const			{ return false; }	
		virtual bool		IsGearSlotGlove() const			{ return false; }	
		virtual bool		IsGearSlotCoreZap() const		{ return false; }	
		virtual bool		IsGearSlotBloodBible() const		{ return false; }	
		virtual bool		IsGearSlotPDA() const			{ return false; }	
		virtual bool		IsGearSlotShoulder() const		{ return false; }	

		// vampire
		virtual bool		IsGearSlotVampireEarRing() const	{ return false; }
		virtual bool		IsGearSlotVampireRing() const	{ return false; }
		virtual bool		IsGearSlotVampireBracelet() const	{ return false; }
		virtual bool		IsGearSlotVampireNecklace() const	{ return false; }
		virtual bool		IsGearSlotVampireCoat() const	{ return false; }
		virtual bool		IsGearSlotVampireLeftHand() const	{ return false; }
		virtual bool		IsGearSlotVampireRightHand() const	{ return false; }
		virtual bool		IsGearSlotVampireTwoHand() const	{ return false; }	
		virtual bool		IsGearSlotVampireAmulet() const	{ return false; }
		virtual bool		IsGearSlotVampireCoreZap() const		{ return false; }	
		virtual bool		IsGearSlotVampireBloodBible() const		{ return false; }	
		virtual bool		IsGearSlotVampireDermis() const		{ return false; }	
		virtual bool		IsGearSlotVampirePersona() const		{ return false; }	

		// ousters
		virtual bool		IsGearSlotOustersCirclet() const		{ return false; }
		virtual bool		IsGearSlotOustersCoat() const			{ return false; }
		virtual bool		IsGearSlotOustersLeftHand() const		{ return false; }
		virtual bool		IsGearSlotOustersRightHand() const		{ return false; }
		virtual bool		IsGearSlotOustersTwoHand() const		{ return false; }	
		virtual bool		IsGearSlotOustersBoots() const			{ return false; }
		virtual bool		IsGearSlotOustersArmsBand() const		{ return false; }
		virtual bool		IsGearSlotOustersRing() const			{ return false; }
		virtual bool		IsGearSlotOustersPendent() const		{ return false; }
		virtual bool		IsGearSlotOustersStone() const			{ return false; }
		virtual bool		IsGearSlotOustersCoreZap() const		{ return false; }	
		virtual bool		IsGearSlotOustersBloodBible() const		{ return false; }	
		virtual bool		IsGearSlotOustersFascia() const		{ return false; }	
		virtual bool		IsGearSlotOustersMitten() const		{ return false; }	

		// 2005, 1, 3, sobeit add start
		virtual bool		IsDurationAlwaysOkay() const		{ return false; }	
		// 2005, 1, 3, sobeit add end
		
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		static bool			IsUseKorean()	{ return s_bUseKorean; }
		static bool			IsUseEnglish()	{ return !s_bUseKorean; }
		static void			UseKorean()		{ s_bUseKorean = true; }
		static void			UseEnglish()	{ s_bUseKorean = false; }

		//---------------------------------------------------
		
		//---------------------------------------------------
		const char*				GetName();
		const char*				GetEName() const;
		const char*				GetDescription() const;
		TYPE_ITEM_WEIGHT		GetWeight() const;
		TYPE_ITEM_PRICE			GetPrice() const;
		BYTE					GetGridWidth() const;
		BYTE					GetGridHeight() const;

		// FrameID
		TYPE_FRAMEID			GetTileFrameID() const;			
		TYPE_FRAMEID			GetInventoryFrameID() const;	
		TYPE_FRAMEID			GetGearFrameID() const;			
		TYPE_FRAMEID			GetAddonMaleFrameID() const;
		TYPE_FRAMEID			GetAddonFemaleFrameID() const;
		TYPE_FRAMEID			GetDropFrameID() const;			

		// Sound ID
		TYPE_SOUNDID			GetUseSoundID() const;			
		TYPE_SOUNDID			GetTileSoundID() const;			
		TYPE_SOUNDID			GetInventorySoundID() const;	
		TYPE_SOUNDID			GetGearSoundID() const;			

		// addon slot
		virtual ADDON			GetAddonSlot() const	{ return ADDON_NULL; }

		//---------------------------------------------------
		
		//---------------------------------------------------
		bool					IsGenderForMale() const;
		bool					IsGenderForFemale() const;
		bool					IsGenderForAll() const;

		//---------------------------------------------------
		
		//---------------------------------------------------
		BYTE					GetRequireSTR()	const;
		BYTE					GetRequireDEX()	const;
		BYTE					GetRequireINT()	const;
		BYTE					GetRequireLevel() const;		
		WORD					GetRequireSUM() const;		

		//---------------------------------------------------
		
		//---------------------------------------------------
		TYPE_ACTIONINFO			GetUseActionInfo() const;

		//---------------------------------------------------
		
		//---------------------------------------------------
		BYTE				GetItemOptionPart(int OptionNum = 0) const;
		const char*			GetItemOptionName(int OptionNum = 0) const;
		const char*			GetItemOptionEName(int OptionNum = 0) const;
		BYTE				GetItemOptionPlusPoint(int OptionNum = 0) const;
//		BYTE				GetItemOptionPlusRequireAbility() const;
		DWORD				GetItemOptionPriceMultiplier() const;
		WORD				GetItemOptionColorSet(int OptionNum = 0);
		int					GetItemOptionRequireSTR()	const;
		int					GetItemOptionRequireDEX()	const;
		int					GetItemOptionRequireINT()	const;
		int					GetItemOptionRequireLevel() const;		
		int					GetItemOptionRequireSUM() const;

		//---------------------------------------------------
		
		//---------------------------------------------------
		
		void					SetGridXY(BYTE x, BYTE y)	{ m_GridX=x; m_GridY=y; }
		BYTE					GetGridX() const			{ return m_GridX; }
		BYTE					GetGridY() const			{ return m_GridY; }		

		
		void					SetItemSlot(BYTE n)		{ m_GridX = n; }
		BYTE					GetItemSlot() const		{ return m_GridX; }
		
		
		TYPE_ITEM_DURATION		GetCurrentDurability() const				{ return m_CurrentDurability; }
		virtual void			SetCurrentDurability(TYPE_ITEM_DURATION d)	{ m_CurrentDurability = d; }

		
		virtual TYPE_ITEM_NUMBER	GetNumber() const					{ return m_Number; }
		virtual void				SetNumber(TYPE_ITEM_NUMBER n)		{ m_Number = n; }

		
		virtual TYPE_ITEM_NUMBER	GetMaxNumber() const				{ return 1; }

		
		//---------------------------------------------------
		
		//---------------------------------------------------		
		virtual int		GetMaxDurability() const	{ return -1; }	
		virtual int		GetCarryingCapacity() const	{ return -1; }	
		virtual int		GetProtectionValue() const	{ return -1; }	
		virtual int		GetDefenseValue() const		{ return -1; }	
		virtual int		GetPocketNumber() const		{ return -1; }	
		virtual int		GetMinDamage() const		{ return -1; }	
		virtual int		GetMaxDamage() const		{ return -1; }	
		virtual int		GetHealPoint() const		{ return -1; }	
		virtual int		GetManaPoint() const		{ return -1; }	
		virtual ITEM_CLASS	GetGunClass() const		{ return ITEM_CLASS_NULL; }	
		virtual int		GetMagazineSize() const		{ return -1; }	
		virtual int		GetReach() const			{ return  1; }	
		virtual int		GetToHit() const			{ return -1; }	// ToHit
		virtual int		GetCriticalHit() const		{ return -1; }	
		virtual int		GetLucky() const			{ return -9999; }	

		//---------------------------------------------------
		
		//---------------------------------------------------
		void		SetDropping();
		BOOL		IsDropping() const		{ return m_bDropping; }
		int			GetDropHeight() const	{ return s_DropHeight[m_DropCount]; }
		void		NextDropFrame();		

		//---------------------------------------------------
		// Identified
		//---------------------------------------------------
		BOOL		IsIdentified() const	{ return m_bIdentified; }
		void		SetIdentified()			{ m_bIdentified = TRUE; }
		void		UnSetIdentified()		{ m_bIdentified = FALSE; }

		//-----------------------------------------------
		
		//-----------------------------------------------
		bool	IsAffectStatus() const		{ return m_bAffectStatus; }
		void	SetAffectStatus() 			{ m_bAffectStatus = true; }
		void	UnSetAffectStatus()			{ m_bAffectStatus = false; }

		//---------------------------------------------------
		
		//---------------------------------------------------
		BOOL		IsTrade() const			{ return m_bTrade; }
		void		SetTrade()				{ m_bTrade = TRUE; }
		void		UnSetTrade()			{ m_bTrade = FALSE; }

		//---------------------------------------------------
		// Silver
		//---------------------------------------------------
		int			GetSilverMax() const	{ return (*g_pItemTable)[GetItemClass()][m_ItemType].SilverMax; }	
		int			GetSilver() const		{ return m_Silver; }
		virtual void	SetSilver(int s)		{ m_Silver = s; }

		//---------------------------------------------------
		// Speed
		//---------------------------------------------------
		int			GetOriginalSpeed() const;
		int			GetSpeed() const;
		void		SetSpeed(WORD s)		{ m_Speed = s; }

		//---------------------------------------------------
		// Grade
		//---------------------------------------------------
		int		GetGrade() const		{ return m_Grade; }
		virtual void	SetGrade(int s)		{ m_Grade = s; }

		//---------------------------------------------------
		// EnchantLevel
		//---------------------------------------------------
		WORD		GetEnchantLevel() const		{ return m_EnchantLevel; }
		virtual void	SetEnchantLevel(WORD s)		{ m_EnchantLevel = s; }

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void		SetName(const char* pName);

		//-------------------------------------------------------
		// Quest
		//-------------------------------------------------------
		void		SetQuestFlag(bool isQuest = true) { m_Quest = isQuest; }

		void					SetPersnalPrice(TYPE_ITEM_PRICE price) { m_persnal_price = price;}
		TYPE_ITEM_PRICE			GetPersnalPrice() { return m_persnal_price; }

		void			SetPersnal(bool  persnal) { m_persnal = persnal;}
		bool		GetPersnal() { return m_persnal; }

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		//void	SaveToFile(std::ofstream& file);
		//void	LoadFromFile(std::ifstream& file);

	public :		
		
		typedef MItem* (*FUNCTION_NEWITEMCLASS)();

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		static MItem*		NewItem(ITEM_CLASS itemClass);

		
		static FUNCTION_NEWITEMCLASS		s_NewItemClassTable[MAX_ITEM_CLASS];

	
	//-------------------------------------------------------
	//
	
	//
	//-------------------------------------------------------
	protected :				
		
		TYPE_ITEMTYPE			m_ItemType;	

		
		std::list<TYPE_ITEM_OPTION>		m_ItemOptionList;

		
		WORD					m_ItemColorSet;
		
		
		BYTE					m_GridX;
		BYTE					m_GridY;

		
		TYPE_ITEM_DURATION		m_CurrentDurability;		

		
		TYPE_ITEM_NUMBER		m_Number;

		//---------------------------------------------------
		// drop
		//---------------------------------------------------
		BOOL					m_bDropping;					
		int						m_DropCount;					
		static int				s_DropHeight[MAX_DROP_COUNT];	

		//---------------------------------------------------
		// identified
		//---------------------------------------------------
		BOOL					m_bIdentified;

		//---------------------------------------------------
		
		//---------------------------------------------------
		bool					m_bAffectStatus;	

		//---------------------------------------------------
		
		//---------------------------------------------------
		BOOL					m_bTrade;

		//---------------------------------------------------
		// Silver
		//---------------------------------------------------
		int						m_Silver;	
		
		//---------------------------------------------------
		// Speed
		//---------------------------------------------------
		int						m_Speed;

		//---------------------------------------------------
		// Grade
		//---------------------------------------------------
		int						m_Grade;		
		
		//---------------------------------------------------
		// Enchant Level
		//---------------------------------------------------
		int						m_EnchantLevel;		
		
		//---------------------------------------------------
		
		//---------------------------------------------------
		char*					m_pName;

		
		static bool				s_bUseKorean;
		
		//---------------------------------------------------
		
		//---------------------------------------------------
		bool					m_Quest;
		bool					m_persnal;
		//by thai00
		TYPE_ITEM_PRICE			m_persnal_price;

};

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class MGearItem : public MItem {
	public :
		MGearItem() {}
		virtual ~MGearItem() {}

	public :
		bool			IsGearItem() const			{ return true; }

		
		virtual int	GetMaxDurability() const;
		virtual int	GetProtectionValue() const	{ return (*g_pItemTable)[GetItemClass()][m_ItemType].Value2; }	
		virtual int	GetDefenseValue() const		{ return (*g_pItemTable)[GetItemClass()][m_ItemType].Value6; }	// def
};

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class MPileItem : public MItem {
	public :
		MPileItem() { m_Number = 1; }
		virtual ~MPileItem() {}

		TYPE_ITEM_NUMBER	GetNumber() const					{ return m_Number; }
		void				SetNumber(TYPE_ITEM_NUMBER n)		{ m_Number = n; }

		bool			IsPileItem() const			{ return true; }		
};

//-------------------------------------------------------

//-------------------------------------------------------
class MUsePotionItem : public MPileItem
{
public:
	#ifdef __TEST_SUB_INVENTORY__   
		virtual void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		virtual void	UseInventory();
	#endif
	
};

//----------------------------------------------------------------------
// MArmorItem
//----------------------------------------------------------------------
class MArmorItem : public MGearItem
{
	public :
		MArmorItem() {}
		~MArmorItem() {}

		int		GetProtectionValue() const;
		int		GetDefenseValue() const;
};
//----------------------------------------------------------------------
// MArmorItem2
//----------------------------------------------------------------------
class MArmorItem2 : public MGearItem
{
	public :
		MArmorItem2() {}
		~MArmorItem2() {}

		int		GetProtectionValue() const;
		int		GetDefenseValue() const;
		int		GetMaxDurability() const;
};
//----------------------------------------------------------------------
// Accessory Item
//----------------------------------------------------------------------
class MAccessoryItem : public MGearItem
{
	public :
		MAccessoryItem() {}
		~MAccessoryItem() {}

		int		GetLucky() const;
};
//----------------------------------------------------------------------
// BELT
//----------------------------------------------------------------------
class MBelt : public MArmorItem2, public MSlotItemManager {
	public :
		MBelt() {}
		~MBelt() {}

	public :
		//------------------------------------------------
		//
		//					MGearItem
		//
		//------------------------------------------------
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_BELT; }

		//------------------------------------------------
		
		//------------------------------------------------
		void				SetItemType(TYPE_ITEMTYPE type);

		ITEM_GEAR_SLOT	GetGearSlot() const		{ return SLOT_BELT; }
		bool			IsGearSlotBelt() const		{ return true; }

		bool			IsContainerItem() const		{ return true; }

		//------------------------------------------------
		//
		//				SlotItemManager
		//
		//------------------------------------------------
		
		//------------------------------------------------
		bool			AddItem(MItem* pItem);
		bool			AddItem(MItem* pItem, BYTE n);
		
		//------------------------------------------------
		
		//------------------------------------------------
		bool			ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);	

		//------------------------------------------------
		
		
		//------------------------------------------------
		bool			CanReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);

		//------------------------------------------------
		// Find Slot To Add Item
		//------------------------------------------------
		bool			FindSlotToAddItem(MItem* pItem, int &slot) const;

		
		virtual int	GetPocketNumber() const		{ return (*g_pItemTable)[ITEM_CLASS_BELT][m_ItemType].Value3; }	

		static MItem*	NewItem()				{ return new MBelt; }
};

//----------------------------------------------------------------------
// CORPSE
//----------------------------------------------------------------------
class MCorpse : public MItem, public MGridItemManager  {
	public :
		MCorpse();
		~MCorpse();

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_CORPSE; }

		bool			IsInventoryItem() const		{ return false; }

		bool			IsContainerItem() const		{ return true; }

		static MItem*	NewItem()				{ return new MCorpse; }

		
		//----------------------------------------------------------
		
		//----------------------------------------------------------
		MCreature*		GetCreature() const			{ return m_pCreature; }	
		MCreature*		SetCreature(MCreature* pCreature)	{ MCreature* pTemp=m_pCreature; m_pCreature=pCreature; return pTemp; }

	public :
		
		MCreature*		m_pCreature;
};

//----------------------------------------------------------------------
// POTION
//----------------------------------------------------------------------
class MPotion : public MUsePotionItem {
	public :
		MPotion() {}
		~MPotion() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_POTION; }

		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		// quick item
		bool		IsQuickItem() const			{ return true; }

		
		int		GetHealPoint() const		{ return (*g_pItemTable)[ITEM_CLASS_POTION][m_ItemType].Value1; }	
		
		
		int		GetManaPoint() const		{ return (*g_pItemTable)[ITEM_CLASS_POTION][m_ItemType].Value2; }	

		// use item
		void	UseQuickItem();
		
		static MItem*	NewItem()	{ return new MPotion; }
};

//----------------------------------------------------------------------
// WATER
//----------------------------------------------------------------------
class MWater : public MPileItem {
	public :
		MWater() {}
		~MWater() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_WATER; }

		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

		static MItem*	NewItem()	{ return new MWater; }
};

//----------------------------------------------------------------------
// HOLYWATER
//----------------------------------------------------------------------
class MHolyWater : public MPileItem {
	public :
		MHolyWater() {}
		~MHolyWater() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_HOLYWATER; }

		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		// damage
		virtual int	GetMinDamage() const	{ return (*g_pItemTable)[ITEM_CLASS_HOLYWATER][m_ItemType].Value1; }	
		virtual int	GetMaxDamage() const	{ return (*g_pItemTable)[ITEM_CLASS_HOLYWATER][m_ItemType].Value2; }	

		static MItem*	NewItem()	{ return new MHolyWater; }
};

//----------------------------------------------------------------------
// MAGAZINE
//----------------------------------------------------------------------
class MMagazine : public MPileItem {
	public :
		MMagazine() {}
		~MMagazine() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_MAGAZINE; }

		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		// quick item
		bool		IsQuickItem() const			{ return true; }

		// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
		void		UseQuickItem();

		
		bool		IsInsertToItem(const MItem* pItem) const;
		
		
		ITEM_CLASS	GetGunClass() const		{ return (ITEM_CLASS)(*g_pItemTable)[ITEM_CLASS_MAGAZINE][m_ItemType].Value1; }	
		virtual int	GetMagazineSize() const	{ return (*g_pItemTable)[ITEM_CLASS_MAGAZINE][m_ItemType].Value2; }	

		static MItem*	NewItem()	{ return new MMagazine; }
};

//----------------------------------------------------------------------
// Ring
//----------------------------------------------------------------------
class MRing : public MAccessoryItem {
	public :
		MRing() {}
		~MRing() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_RING; }

		ITEM_GEAR_SLOT	GetGearSlot() const		{ return SLOT_RING; }
		bool		IsGearSlotRing() const		{ return true; }

		static MItem*	NewItem()	{ return new MRing; }
};

//----------------------------------------------------------------------
// Bracelet
//----------------------------------------------------------------------
class MBracelet : public MAccessoryItem {
	public :
		MBracelet() {}
		~MBracelet() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_BRACELET; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_BRACELET; }
		bool		IsGearSlotBracelet() const			{ return true; }

		static MItem*	NewItem()	{ return new MBracelet; }
};
					

//----------------------------------------------------------------------
// NECKLACE
//----------------------------------------------------------------------
class MNecklace : public MAccessoryItem {
	public :
		MNecklace() {}
		~MNecklace() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_NECKLACE; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_NECKLACE; }
		bool		IsGearSlotNecklace() const			{ return true; }

		static MItem*	NewItem()	{ return new MNecklace; }
};

//----------------------------------------------------------------------
// COAT
//----------------------------------------------------------------------
class MCoat : public MArmorItem {
	public :
		MCoat() {}
		~MCoat() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_COAT; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_COAT; }
		bool		IsGearSlotCoat() const			{ return true; }

		bool		IsAddonItem() const				{ return true; }
		ADDON		GetAddonSlot() const			{ return ADDON_COAT; }

		static MItem*	NewItem()	{ return new MCoat; }
};

//----------------------------------------------------------------------
// Trouser
//----------------------------------------------------------------------
class MTrouser : public MArmorItem {
	public :
		MTrouser() {}
		~MTrouser() {}

	public :
		ITEM_CLASS	GetItemClass() const			{ return ITEM_CLASS_TROUSER; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_TROUSER; }
		bool		IsGearSlotTrouser() const			{ return true; }

		bool		IsAddonItem() const				{ return true; }
		ADDON		GetAddonSlot() const			{ return ADDON_TROUSER; }

		static MItem*	NewItem()	{ return new MTrouser; }
};
	
//----------------------------------------------------------------------
// SHOES
//----------------------------------------------------------------------
class MShoes : public MArmorItem2 {
	public :
		MShoes() {}
		~MShoes() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SHOES; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_SHOES; }
		bool		IsGearSlotShoes() const			{ return true; }

		static MItem*	NewItem()	{ return new MShoes; }
};

//----------------------------------------------------------------------
// BasicWeapon
//----------------------------------------------------------------------
class MWeaponItem : public MGearItem
{
	public :
		MWeaponItem() {}
		~MWeaponItem() {}

		bool		IsBasicWeapon() const		{ return true; }
		
		int			GetMinDamage() const;
		int			GetMaxDamage() const;
		int			GetToHit() const;
		int			GetCriticalHit() const;//		{ return (*g_pItemTable)[GetItemClass()][m_ItemType].CriticalHit; }

};

//----------------------------------------------------------------------
// SWORD
//----------------------------------------------------------------------
class MSword : public MWeaponItem {
	public :
		MSword() {}
		~MSword() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SWORD; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_RIGHTHAND; }
		bool		IsGearSlotRightHand() const		{ return true; }

		bool		IsGearSlotSword() const			{ return true; }

		bool		IsAddonItem() const				{ return true; }
		ADDON		GetAddonSlot() const			{ return ADDON_RIGHTHAND; }

		static MItem*	NewItem()	{ return new MSword; }
};

//----------------------------------------------------------------------
// BLADE
//----------------------------------------------------------------------
class MBlade : public MWeaponItem {
	public :
		MBlade() {}
		~MBlade() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_BLADE; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_TWOHAND; }
		bool		IsGearSlotTwoHand() const		{ return true; }

		bool		IsGearSlotBlade() const			{ return true; }

		bool		IsAddonItem() const				{ return true; }
		ADDON		GetAddonSlot() const			{ return ADDON_RIGHTHAND; }

		static MItem*	NewItem()	{ return new MBlade; }
};

//----------------------------------------------------------------------
// Shield
//----------------------------------------------------------------------
class MShield : public MArmorItem2 {
	public :
		MShield() {}
		~MShield() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SHIELD; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_LEFTHAND; }
		bool		IsGearSlotLeftHand() const		{ return true; }

		bool		IsAddonItem() const				{ return true; }
		ADDON		GetAddonSlot() const			{ return ADDON_LEFTHAND; }
		
		static MItem*	NewItem()	{ return new MShield; }
};

//----------------------------------------------------------------------
// 
//----CROSS------------------------------------------------------------------
class MCross : public MWeaponItem {
	public :
		MCross() {}
		~MCross() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_CROSS; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_TWOHAND; }
		bool		IsGearSlotTwoHand() const		{ return true; }

		bool		IsGearSlotCross() const			{ return true; }

		bool		IsAddonItem() const				{ return true; }
		ADDON		GetAddonSlot() const			{ return ADDON_RIGHTHAND; }

		
		virtual int	GetManaPoint() const		{ return (*g_pItemTable)[ITEM_CLASS_CROSS][m_ItemType].Value5; }	
		
		static MItem*	NewItem()	{ return new MCross; }
};

//----------------------------------------------------------------------
// Mace
//----------------------------------------------------------------------
class MMace : public MWeaponItem {
	public :
		MMace() {}
		~MMace() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_MACE; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_TWOHAND; }
		bool		IsGearSlotTwoHand() const		{ return true; }

		bool		IsGearSlotMace() const			{ return true; }

		bool		IsAddonItem() const				{ return true; }
		ADDON		GetAddonSlot() const			{ return ADDON_RIGHTHAND; }

		
		virtual int	GetManaPoint() const		{ return (*g_pItemTable)[ITEM_CLASS_MACE][m_ItemType].Value5; }	
		
		static MItem*	NewItem()	{ return new MMace; }
};

//----------------------------------------------------------------------
// GLOVE
//----------------------------------------------------------------------
class MGlove : public MArmorItem2 {
	public :
		MGlove() {}
		~MGlove() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_GLOVE; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_GLOVE; }
		bool		IsGearSlotGlove() const			{ return true; }

		static MItem*	NewItem()	{ return new MGlove; }
};

//----------------------------------------------------------------------
// HELM
//----------------------------------------------------------------------
class MHelm : public MArmorItem2 {
	public :
		MHelm() {}
		~MHelm() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_HELM; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_HELM; }
		bool		IsGearSlotHelm() const			{ return true; }

		bool		IsAddonItem() const				{ return true; }
		ADDON		GetAddonSlot() const			{ return ADDON_HELM; }

		static MItem*	NewItem()	{ return new MHelm; }
};

//----------------------------------------------------------------------
//
// Gun Item
//
//----------------------------------------------------------------------
class MGunItem : public MWeaponItem {
	public :
		MGunItem() { m_pMagazine=NULL; }
		~MGunItem() { if (m_pMagazine!=NULL) delete m_pMagazine; }

	public :
		bool			IsGunItem() const				{ return true; }	

		bool			IsAddonItem() const				{ return true; }
		ADDON			GetAddonSlot() const			{ return ADDON_RIGHTHAND; }

		
		virtual int	GetReach() const			{ return (*g_pItemTable)[GetItemClass()][m_ItemType].Value5; }	
		virtual int	GetMagazineSize() const		{ return (m_pMagazine==NULL)?0:m_pMagazine->GetNumber(); }	
		
		//----------------------------------------------------------
		
		//----------------------------------------------------------
		MMagazine*		GetMagazine() const					{ return m_pMagazine; }
		MMagazine*		RemoveMagazine()					{ MMagazine* pTemp=m_pMagazine; m_pMagazine=NULL; return pTemp; }
		MMagazine*		SetMagazine(MMagazine* pMagazine)	{ MMagazine* pTemp=m_pMagazine; m_pMagazine=pMagazine; return pTemp; }
		void			SetMagazineNULL()					{ m_pMagazine = NULL; }

	protected :
		MMagazine*		m_pMagazine;
};

//----------------------------------------------------------------------
// SG
//----------------------------------------------------------------------
class MGunSG : public MGunItem {
	public :
		MGunSG() {}
		~MGunSG() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SG; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_TWOHAND; }
		bool		IsGearSlotTwoHand() const		{ return true; }

		static MItem*	NewItem()	{ return new MGunSG; }
};

//----------------------------------------------------------------------
// SMG
//----------------------------------------------------------------------
class MGunSMG : public MGunItem {
	public :
		MGunSMG() {}
		~MGunSMG() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SMG; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_TWOHAND; }
		bool		IsGearSlotTwoHand() const		{ return true; }

		static MItem*	NewItem()	{ return new MGunSMG; }
};

//----------------------------------------------------------------------
// AR
//----------------------------------------------------------------------
class MGunAR : public MGunItem {
	public :
		MGunAR() {}
		~MGunAR() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_AR; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_TWOHAND; }
		bool		IsGearSlotTwoHand() const		{ return true; }

		static MItem*	NewItem()	{ return new MGunAR; }
};
		
//----------------------------------------------------------------------
// TR
//----------------------------------------------------------------------
class MGunTR : public MGunItem {
	public :
		MGunTR() {}
		~MGunTR() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SR; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_TWOHAND; }
		bool		IsGearSlotTwoHand() const			{ return true; }

		static MItem*	NewItem()	{ return new MGunTR; }
};			
				
//----------------------------------------------------------------------
// ETC
//----------------------------------------------------------------------
class MItemETC : public MPileItem {
	public :
		MItemETC() {}
		~MItemETC() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_ETC; }

		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

		static MItem*	NewItem()	{ return new MItemETC; }
};

//----------------------------------------------------------------------
// MOTORCYCLE
//----------------------------------------------------------------------
class MMotorcycle : public MItem, public MGridItemManager  {
	public :
		MMotorcycle() {}
		~MMotorcycle() {}

	public :

		ITEM_CLASS	GetItemClass() const			{ return ITEM_CLASS_MOTORCYCLE; }

		bool			IsInventoryItem() const		{ return false; }

		bool			IsContainerItem() const		{ return true; }

		bool			IsAddonItem() const				{ return true; }
		ADDON			GetAddonSlot() const			{ return ADDON_MOTOR; }
		
		virtual int	GetMaxDurability() const;
		virtual int	GetCarryingCapacity() const	{ return (*g_pItemTable)[ITEM_CLASS_MOTORCYCLE][m_ItemType].Value2; }	
		
		static MItem*	NewItem()	{ return new MMotorcycle; }
};

//----------------------------------------------------------------------
// KEY
//----------------------------------------------------------------------
class MKey : public MItem {
	public :
		MKey() {}
		~MKey() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_KEY; }

		// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
		void		UseQuickItem();
		bool		IsQuickItem() const { return true; }

		static MItem*	NewItem()	{ return new MKey; }
};

//----------------------------------------------------------------------
// Relic
//----------------------------------------------------------------------
class MRelic : public MItem {
	public :
		MRelic() {}
		~MRelic() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_RELIC; }

		static MItem*	NewItem()	{ return new MRelic; }
};

//----------------------------------------------------------------------
// BOMB_MATERIAL
//----------------------------------------------------------------------
class MBombMaterial : public MPileItem {
	public :
		MBombMaterial() {}
		~MBombMaterial() {}

	public :
		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_BOMB_MATERIAL; }

	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

		static MItem*	NewItem()	{ return new MBombMaterial; }
};

//----------------------------------------------------------------------
// LEARNINGITEM
//----------------------------------------------------------------------
class MLearningItem : public MItem {
	public :
		MLearningItem() {}
		~MLearningItem() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_LEARNINGITEM; }

		static MItem*	NewItem()	{ return new MLearningItem; }
};

//----------------------------------------------------------------------
// MONEY
//----------------------------------------------------------------------
class MMoney : public MPileItem {
	public :
		MMoney() { m_Number = 1; }
		~MMoney() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_MONEY; }
		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		static MItem*	NewItem()	{ return new MMoney; }

		//yckou
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

};

//----------------------------------------------------------------------
// BOMB
//----------------------------------------------------------------------
class MBomb : public MPileItem {
	public :
		MBomb() {}
		~MBomb() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_BOMB; }

		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		
		int		GetMinDamage() const		{ return (*g_pItemTable)[ITEM_CLASS_BOMB][m_ItemType].Value1; }	
		int		GetMaxDamage() const		{ return (*g_pItemTable)[ITEM_CLASS_BOMB][m_ItemType].Value2; }	

		static MItem*	NewItem()	{ return new MBomb; }
};

//----------------------------------------------------------------------
// MINE
//----------------------------------------------------------------------
class MMine : public MPileItem {
	public :
		MMine() { m_bInstalled = false; }
		~MMine() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_MINE; }

		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		
		int		GetMinDamage() const		{ return (*g_pItemTable)[ITEM_CLASS_MINE][m_ItemType].Value1; }	
		int		GetMaxDamage() const		{ return (*g_pItemTable)[ITEM_CLASS_MINE][m_ItemType].Value2; }	

		
		bool	IsInstalled() const			{ return m_bInstalled; }
		void	SetInstalled() 				{ m_bInstalled = true; }
		void	UnSetInstalled() 			{ m_bInstalled = false; }

		// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

		static MItem*	NewItem()			{ return new MMine; }

	protected :
		bool				m_bInstalled;
};

//----------------------------------------------------------------------
// Skull
//-----------------------------------------------------------------------
/*
class MSkull : public MItem {
	public :
		MSkull() {}
		~MSkull() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SKULL; }
		
		bool		IsVampireItem() const		{ return true; }

		static MItem*	NewItem()	{ return new MSkull; }
};
*/

//----------------------------------------------------------------------
// VAMPIRE_RING
//----------------------------------------------------------------------
class MVampireRing : public MAccessoryItem {
	public :
		MVampireRing() {}
		~MVampireRing() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_VAMPIRE_RING; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_VAMPIRE_RING; }
		bool		IsGearSlotVampireRing() const			{ return true; }

		static MItem*	NewItem()	{ return new MVampireRing; }
};

//----------------------------------------------------------------------
// VAMPIRE_EARRING
//----------------------------------------------------------------------
class MVampireEarRing : public MAccessoryItem {
	public :
		MVampireEarRing() {}
		~MVampireEarRing() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_VAMPIRE_EARRING; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_VAMPIRE_EARRING; }
		bool		IsGearSlotVampireEarRing() const			{ return true; }

		static MItem*	NewItem()	{ return new MVampireEarRing; }
};

//----------------------------------------------------------------------
// VAMPIRE_BRACELET
//----------------------------------------------------------------------
class MVampireBracelet : public MAccessoryItem {
	public :
		MVampireBracelet() {}
		~MVampireBracelet() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_VAMPIRE_BRACELET; }

		ITEM_GEAR_SLOT	GetGearSlot() const					{ return SLOT_VAMPIRE_BRACELET; }
		bool		IsGearSlotVampireBracelet() const			{ return true; }

		static MItem*	NewItem()	{ return new MVampireBracelet; }
};

//----------------------------------------------------------------------
// VAMPIRE_NECKLACE
//----------------------------------------------------------------------
class MVampireNecklace : public MAccessoryItem {
	public :
		MVampireNecklace() {}
		~MVampireNecklace() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_VAMPIRE_NECKLACE; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_VAMPIRE_NECKLACE; }
		bool		IsGearSlotVampireNecklace() const	{ return true; }

		static MItem*	NewItem()	{ return new MVampireNecklace; }
};

//----------------------------------------------------------------------
// VAMPIRE_COAT
//----------------------------------------------------------------------
class MVampireCoat : public MArmorItem {
	public :
		MVampireCoat() {}
		~MVampireCoat() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_VAMPIRE_COAT; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_VAMPIRE_COAT; }
		bool		IsGearSlotVampireCoat() const	{ return true; }
		
		bool		IsAddonItem() const				{ return true; }
		ADDON		GetAddonSlot() const			{ return ADDON_COAT; }
		

		static MItem*	NewItem()	{ return new MVampireCoat; }
};

//----------------------------------------------------------------------
// MSkull
//----------------------------------------------------------------------
class MSkull : public MPileItem {
	public :
		MSkull() {}
		~MSkull() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SKULL; }

		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

		static MItem*	NewItem()	{ return new MSkull; }
};

//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MSerum : public MUsePotionItem {
	public :
		MSerum() {}
		~MSerum() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SERUM; }

		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;
		
		
		int		GetHealPoint() const		{ return (*g_pItemTable)[ITEM_CLASS_SERUM][m_ItemType].Value1; }	

		static MItem*	NewItem()	{ return new MSerum; }
};

//----------------------------------------------------------------------
// ETC
//----------------------------------------------------------------------
class MVampireETC : public MPileItem {
	public :
		MVampireETC() {}
		~MVampireETC() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_VAMPIRE_ETC; }

		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

		static MItem*	NewItem()	{ return new MVampireETC; }
};


//----------------------------------------------------------------------
// Slayer Portal
//----------------------------------------------------------------------
class MSlayerPortalItem : public MItem {
	public :
		MSlayerPortalItem() { m_Number = 0; }
		virtual ~MSlayerPortalItem() {}

		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SLAYER_PORTAL_ITEM; }

		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		TYPE_ITEM_NUMBER	GetNumber() const					{ return m_Number; }
		void				SetNumber(TYPE_ITEM_NUMBER n)		{ m_Number = n; }

		bool				IsChargeItem() const				{ return true; }

		void				SetEnchantLevel(WORD s);

		// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

		static MItem*	NewItem()	{ return new MSlayerPortalItem; }
};

//----------------------------------------------------------------------
// Vampire Portal
//----------------------------------------------------------------------
class MVampirePortalItem : public MItem {
	public :
		MVampirePortalItem();
		virtual ~MVampirePortalItem() {}

		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_VAMPIRE_PORTAL_ITEM; }
		
		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		TYPE_ITEM_NUMBER	GetNumber() const					{ return m_Number; }
		void				SetNumber(TYPE_ITEM_NUMBER n)		{ m_Number = n; }

		bool				IsChargeItem() const				{ return true; }

		
		bool				IsMarked() const;
		void				SetZone(int zoneID, TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y);
		int					GetZoneID() const	{ return m_ZoneID; }
		TYPE_SECTORPOSITION	GetZoneX() const	{ return m_ZoneX; }
		TYPE_SECTORPOSITION	GetZoneY() const	{ return m_ZoneY; }

		
		void				SetSilver(int s);
		void				SetCurrentDurability(TYPE_ITEM_DURATION d);
		void				SetEnchantLevel(WORD s);

		// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

		static MItem*	NewItem()	{ return new MVampirePortalItem; }

	protected :
		int						m_ZoneID;
		TYPE_SECTORPOSITION		m_ZoneX;
		TYPE_SECTORPOSITION		m_ZoneY;
};

//----------------------------------------------------------------------
// Event GiftBox
//----------------------------------------------------------------------
class MEventGiftBoxItem : public MItem {
	public :
		MEventGiftBoxItem()	{}
		virtual ~MEventGiftBoxItem() {}

		ITEM_CLASS	GetItemClass() const			{ return ITEM_CLASS_EVENT_GIFT_BOX; }

	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
		
		static MItem*	NewItem()	{ return new MEventGiftBoxItem; }
};

//----------------------------------------------------------------------
// Event Star
//----------------------------------------------------------------------
class MEventStarItem : public MPileItem {
	public :
		MEventStarItem()	{}
		virtual ~MEventStarItem() {}

		ITEM_CLASS	GetItemClass() const			{ return ITEM_CLASS_EVENT_STAR; }

		TYPE_ITEM_NUMBER	GetMaxNumber() const;

	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
		
		static MItem*	NewItem()	{ return new MEventStarItem; }
};

//----------------------------------------------------------------------
// Vampire Weapon
//----------------------------------------------------------------------
class MVampireWeapon : public MWeaponItem {
	public :
		MVampireWeapon() {}
		~MVampireWeapon() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_VAMPIRE_WEAPON; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_VAMPIRE_TWOHAND; }
		bool		IsGearSlotTwoHand() const		{ return true; }

		bool		IsGearSlotVampireWeapon() const			{ return true; }

		static MItem*	NewItem()	{ return new MVampireWeapon; }
};

//----------------------------------------------------------------------
// VAMPIRE_Amulet
//----------------------------------------------------------------------
class MVampireAmulet : public MAccessoryItem {
	public :
		MVampireAmulet() {}
		~MVampireAmulet() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_VAMPIRE_AMULET; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_VAMPIRE_AMULET; }
		bool		IsGearSlotVampireAmulet() const			{ return true; }
		bool		IsDurationAlwaysOkay() const		{ return true; }	
		static MItem*	NewItem()	{ return new MVampireAmulet; }
};

//----------------------------------------------------------------------
// QuestItem
//----------------------------------------------------------------------
class MQuestItem : public MItem {
	public :
		MQuestItem()	{}
		virtual ~MQuestItem() {}

		ITEM_CLASS	GetItemClass() const			{ return ITEM_CLASS_QUEST_ITEM; }

		static MItem*	NewItem()	{ return new MQuestItem; }
};

//----------------------------------------------------------------------
// Event Tree
//----------------------------------------------------------------------
class MEventTreeItem : public MItem {
	public :
		MEventTreeItem()	{}
		virtual ~MEventTreeItem() {}

		ITEM_CLASS	GetItemClass() const			{ return ITEM_CLASS_EVENT_TREE; }

	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
		
		static MItem*	NewItem()	{ return new MEventTreeItem; }
};

//----------------------------------------------------------------------
// Event ETC
//----------------------------------------------------------------------
class MEventEtcItem : public MPileItem {
	public :
		MEventEtcItem()	{}
		virtual ~MEventEtcItem() {}

		ITEM_CLASS	GetItemClass() const			{ return ITEM_CLASS_EVENT_ETC; }
		
		TYPE_ITEM_NUMBER	GetMaxNumber() const;

		// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
		void		UseQuickItem();
		bool		IsQuickItem() const { return (GetItemType()>=14&& GetItemType()<=17)?true:false; }

		static MItem*	NewItem()	{ return new MEventEtcItem; }
};

//----------------------------------------------------------------------
// BloodBible
//----------------------------------------------------------------------
class MBloodBible : public MItem {
	public :
		MBloodBible() {}
		~MBloodBible() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_BLOOD_BIBLE; }

		static MItem*	NewItem()	{ return new MBloodBible; }
};


//----------------------------------------------------------------------
// CastleSymbol
//----------------------------------------------------------------------
class MCastleSymbol : public MItem {
	public :
		MCastleSymbol() {}
		~MCastleSymbol() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_CASTLE_SYMBOL; }

		static MItem*	NewItem()	{ return new MCastleSymbol; }
};

//----------------------------------------------------------------------
// CoupleRing
//----------------------------------------------------------------------
class MCoupleRing : public MRing {
	public :
		MCoupleRing() {}
		~MCoupleRing() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_COUPLE_RING; }

		// use item
		void	UseGear();
		bool	IsDurationAlwaysOkay() const		{ return true; }	

		static MItem*	NewItem()	{ return new MCoupleRing; }
};

//----------------------------------------------------------------------
// VampireCoupleRing
//----------------------------------------------------------------------
class MVampireCoupleRing : public MVampireRing {
	public :
		MVampireCoupleRing() {}
		~MVampireCoupleRing() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_VAMPIRE_COUPLE_RING; }
		
		// use item
		void	UseGear();
		bool	IsDurationAlwaysOkay() const		{ return true; }	
		static MItem*	NewItem()	{ return new MVampireCoupleRing; }
};

class MEventItem : public MItem {
public :
	MEventItem() {}
	~MEventItem() {}

	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_EVENT_ITEM; }

	static MItem* NewItem()		{ return new MEventItem; }

};

class MDyePotionItem : public MItem {
public :
	MDyePotionItem() {}
	~MDyePotionItem() {}

	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_DYE_POTION; }

	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
//	void		UseQuickItem();
//	bool		IsQuickItem() const { return true; }


	static MItem* NewItem()	{ return new MDyePotionItem; }
};

class MResurrectItem : public MPileItem {
public :
	MResurrectItem() {}
	~MResurrectItem() {}

	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_RESURRECT_ITEM; }
	TYPE_ITEM_NUMBER	GetMaxNumber() const;

	static MItem* NewItem() { return new MResurrectItem; }
};

class MMixingItem : public MPileItem {
public :
	MMixingItem() {}
	~MMixingItem() {}
	
	ITEM_CLASS	GetItemClass() const { return ITEM_CLASS_MIXING_ITEM; }
	TYPE_ITEM_NUMBER	GetMaxNumber() const;

	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

	static MItem* NewItem() { return new MMixingItem; }
};

//----------------------------------------------------------------------
// Ousters ArmsBand
//----------------------------------------------------------------------
class MOustersArmsBand : public MArmorItem2, public MSlotItemManager {
public :
	MOustersArmsBand() {}
	~MOustersArmsBand() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_OUSTERS_ARMSBAND; }
	
	ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_OUSTERS_ARMSBAND; }
	
	bool		IsGearSlotOustersArmsBand() const			{ return true; }
	bool		IsContainerItem() const { return true;}

	void		SetItemType(TYPE_ITEMTYPE type);	
	
	//------------------------------------------------
	//
	//				SlotItemManager
	//
	//------------------------------------------------
	
	//------------------------------------------------
	bool			AddItem(MItem* pItem);
	bool			AddItem(MItem* pItem, BYTE n);
	
	//------------------------------------------------
	
	//------------------------------------------------
	bool			ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);		
	//------------------------------------------------
	
	
	//------------------------------------------------
	bool			CanReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);
	
	//------------------------------------------------
	// Find Slot To Add Item
	//------------------------------------------------
	bool			FindSlotToAddItem(MItem* pItem, int &slot) const;	
	
	virtual int	GetPocketNumber() const		{ return (*g_pItemTable)[ITEM_CLASS_OUSTERS_ARMSBAND][m_ItemType].Value3; }	
	
	static MItem*	NewItem()	{ return new MOustersArmsBand; }
};

//----------------------------------------------------------------------
// Ousters Boots
//----------------------------------------------------------------------
class MOustersBoots : public MArmorItem {
public :
	MOustersBoots() {}
	~MOustersBoots() {}
	
public :
	ITEM_CLASS	GetItemClass() const			{ return ITEM_CLASS_OUSTERS_BOOTS; }
	
	ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_OUSTERS_BOOTS; }
	bool		IsGearSlotOustersBoots() const			{ return true; }
	
	bool		IsAddonItem() const				{ return true; }
	ADDON		GetAddonSlot() const			{ return ADDON_TROUSER; }
	
	static MItem*	NewItem()	{ return new MOustersBoots; }
};

//----------------------------------------------------------------------
// Ousters Chakram
//----------------------------------------------------------------------
class MOustersChakram : public MWeaponItem {
public :
	MOustersChakram() {}
	~MOustersChakram() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_OUSTERS_CHAKRAM; }
	
	ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_OUSTERS_TWOHAND; }
	bool		IsGearSlotTwoHand() const		{ return true; }
	
	bool		IsGearSlotOustersWeapon() const			{ return true; }
	
	bool		IsAddonItem() const				{ return true; }
	ADDON		GetAddonSlot() const			{ return ADDON_RIGHTHAND; }
	
	static MItem*	NewItem()	{ return new MOustersChakram; }
};

//----------------------------------------------------------------------
// Ousters Circlet
//----------------------------------------------------------------------
class MOustersCirclet : public MArmorItem2 {
public :
	MOustersCirclet() {}
	~MOustersCirclet() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_OUSTERS_CIRCLET; }
	
	ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_OUSTERS_CIRCLET; }
	bool		IsGearSlotOustersCirclet() const			{ return true; }
	
	static MItem*	NewItem()	{ return new MOustersCirclet; }
};

//----------------------------------------------------------------------
// Ousters Coat
//----------------------------------------------------------------------
class MOustersCoat : public MArmorItem {
public :
	MOustersCoat() {}
	~MOustersCoat() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_OUSTERS_COAT; }
	
	ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_OUSTERS_COAT; }
	bool		IsGearSlotOustersCoat() const	{ return true; }
	
	bool		IsAddonItem() const				{ return true; }
	ADDON		GetAddonSlot() const			{ return ADDON_COAT; }
	
	
	static MItem*	NewItem()	{ return new MOustersCoat; }
};

//----------------------------------------------------------------------
// Ousters Pendent
//----------------------------------------------------------------------
class MOustersPendent : public MAccessoryItem {
public :
	MOustersPendent() {}
	~MOustersPendent() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_OUSTERS_PENDENT; }
	
	ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_OUSTERS_PENDENT; }
	bool		IsGearSlotOustersPendent() const			{ return true; }
	
	static MItem*	NewItem()	{ return new MOustersPendent; }
};

//----------------------------------------------------------------------
// Ousters Ring
//----------------------------------------------------------------------
class MOustersRing : public MAccessoryItem {
public :
	MOustersRing() {}
	~MOustersRing() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_OUSTERS_RING; }
	
	ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_OUSTERS_RING; }
	bool		IsGearSlotOustersRing() const			{ return true; }
	
	static MItem*	NewItem()	{ return new MOustersRing; }
};

//----------------------------------------------------------------------
// Ousters Stone
//----------------------------------------------------------------------
class MOustersStone : public MAccessoryItem {
public :
	MOustersStone() {}
	~MOustersStone() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_OUSTERS_STONE; }
	
	ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_OUSTERS_STONE; }
	bool		IsGearSlotOustersStone() const			{ return true; }
	
	static MItem*	NewItem()	{ return new MOustersStone; }
};

//----------------------------------------------------------------------
// Ousters Wristlet
//----------------------------------------------------------------------
class MOustersWristlet : public MWeaponItem {
public :
	MOustersWristlet() {}
	~MOustersWristlet() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_OUSTERS_WRISTLET; }
	
	ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_OUSTERS_TWOHAND; }
	bool		IsGearSlotTwoHand() const		{ return true; }
	
	bool		IsGearSlotOustersWristlet() const			{ return true; }
	
	bool		IsAddonItem() const				{ return true; }
	ADDON		GetAddonSlot() const			{ return ADDON_RIGHTHAND; }
	
	static MItem*	NewItem()	{ return new MOustersWristlet; }
};

//----------------------------------------------------------------------
// Ousters Larva
//----------------------------------------------------------------------
class MOustersLarva : public MPileItem {
public :
	MOustersLarva() {}
	~MOustersLarva() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_LARVA; }
	TYPE_ITEM_NUMBER	GetMaxNumber() const;	
	static MItem*	NewItem()	{ return new MOustersLarva; }
};

//----------------------------------------------------------------------
// Ousters Pupa
//----------------------------------------------------------------------
class MOustersPupa : public MUsePotionItem {
public :
	MOustersPupa() {}
	~MOustersPupa() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_PUPA; }
	TYPE_ITEM_NUMBER	GetMaxNumber() const;
	
	
	int		GetHealPoint() const		{ return (*g_pItemTable)[ITEM_CLASS_PUPA][m_ItemType].Value1; }	
	
	int		GetManaPoint() const		{ return (*g_pItemTable)[ITEM_CLASS_PUPA][m_ItemType].Value2; }	

	bool	IsQuickItem() const			{ return true; }

	// use item
	void	UseQuickItem();
		
	
	static MItem*	NewItem()	{ return new MOustersPupa; }
};

//----------------------------------------------------------------------
// Ousters ComposMei
//----------------------------------------------------------------------
class MOustersComposMei : public MUsePotionItem {
public :
	MOustersComposMei() {}
	~MOustersComposMei() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_COMPOS_MEI; }
	TYPE_ITEM_NUMBER	GetMaxNumber() const;

	
	int		GetHealPoint() const		{ return (*g_pItemTable)[ITEM_CLASS_COMPOS_MEI][m_ItemType].Value1; }	
	
	
	int		GetManaPoint() const		{ return (*g_pItemTable)[ITEM_CLASS_COMPOS_MEI][m_ItemType].Value2; }	

	bool	IsQuickItem() const			{ return true; }

	// use item
	void	UseQuickItem();
	
	
	static MItem*	NewItem()	{ return new MOustersComposMei; }
};

//----------------------------------------------------------------------
// Ousters SummonGem
//----------------------------------------------------------------------
class MOustersSummonGem : public MItem {
public :
	MOustersSummonGem() {}
	~MOustersSummonGem() {}
	
public :
	ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_OUSTERS_SUMMON_ITEM; }
	
	static MItem*	NewItem()	{ return new MOustersSummonGem; }

	
	TYPE_ITEM_NUMBER	GetMaxNumber() const;
	
	TYPE_ITEM_NUMBER	GetNumber() const					{ return m_Number; }
	void				SetNumber(TYPE_ITEM_NUMBER n)		{ m_Number = n; }
	
	void				SetEnchantLevel(WORD s);

	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
	
	bool				IsChargeItem() const				{ return true; }
};

class MEffectItem : public MPileItem {
public :	
	MEffectItem() {}
	~MEffectItem() {}

public :
	ITEM_CLASS	GetItemClass() const			{ return ITEM_CLASS_EFFECT_ITEM; }

	static MItem* NewItem()		{ return new MEffectItem; }

	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

	TYPE_ITEM_NUMBER			GetMaxNumber() const;
};

class MCodeSheetItem : public MItem {
public :
	MCodeSheetItem() {}
	~MCodeSheetItem() {}

	ITEM_CLASS	GetItemClass() const			{ return ITEM_CLASS_CODE_SHEET; }

	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif

	static MItem* NewItem()		{ return new MCodeSheetItem; }
};

class MMoonCardItem : public MPileItem {
public : 
	MMoonCardItem() { }
	~MMoonCardItem() { }

	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
	
	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_MOON_CARD; }	
	static MItem*			NewItem()					{ return new MMoonCardItem; }	
	TYPE_ITEM_NUMBER		GetMaxNumber() const;
};

class MSweeperItem : public MItem {
public :
	MSweeperItem() { }
	~MSweeperItem() { }

	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_SWEEPER; }
	static MItem*			NewItem()					{ return new MSweeperItem; }
};

class MPetItem : public MItem {
private:
	DWORD	m_UpdateTime;
	DWORD	m_PetExpRemain;
	//2004, 5, 11 sobeit add start
	DWORD	m_PetKeepedDay;
	//2004, 5, 11 sobeit add end
	WORD	m_PetFoodType;
	bool	m_bCanGamble;
	bool	m_bCutHead;
	bool	m_bCanAttack;
public :
	MPetItem();
	~MPetItem() { }

	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_PET_ITEM; }

	// pet info
	void					SetPetExpRemain(DWORD exp)			{ m_PetExpRemain = exp; }
	const DWORD				GetPetExpRemain() const				{ return m_PetExpRemain; }
	void					SetPetFoodType(WORD type)		{ m_PetFoodType = type; }
	const DWORD				GetPetFoodType() const			{ return m_PetFoodType; }
	void					SetPetGamble(bool bGamble)		{ m_bCanGamble = bGamble; }
	const bool				IsCanGamble() const				{ return m_bCanGamble; }
	void					SetPetCutHead(bool bCutHead)	{ m_bCutHead = bCutHead; }
	const bool				IsCanCutHead() const			{ return m_bCutHead; }
	void					SetPetAttack(bool bAttack)	{ m_bCanAttack = bAttack; }
	const bool				IsCanAttack() const			{ return m_bCanAttack; }
	const DWORD				GetUpdateTime() const			{ return m_UpdateTime; }
	void					SetUpdateTime(DWORD updateTime)	{ m_UpdateTime = updateTime; }
	void					SetCurrentDurability(TYPE_ITEM_DURATION d)	{ MItem::SetCurrentDurability(d); SetUpdateTime(timeGetTime()); }

	//2004, 5, 11 sobeit add start
	void					SetPetKeepedDay(DWORD day)			{ m_PetKeepedDay = day; }
	const DWORD				GetPetKeepedDay() const				{ return m_PetKeepedDay; }
	//2004, 5, 11 sobeit add end
	std::string				GetPetName();
	std::string				GetPetOptionName();
	std::string				GetPetOptionEName();
	
	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
	
	static MItem*			NewItem()					{ return new MPetItem; }
};

class MPetFood : public MPileItem {
public :
	MPetFood() { }
	~MPetFood() { }

	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_PET_FOOD; }

	TYPE_ITEM_NUMBER		GetMaxNumber() const;
	
	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
	
	static MItem*			NewItem()					{ return new MPetFood; }
};

class MPetEnchantItem : public MPileItem {
public :
	MPetEnchantItem() { }
	~MPetEnchantItem() { }

	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
	
	TYPE_ITEM_NUMBER		GetMaxNumber() const;

	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_PET_ENCHANT_ITEM; }

	static MItem*			NewItem()					{ return new MPetEnchantItem; }
};

class MLuckyBag : public MPileItem {
public :
	MLuckyBag() { }
	~MLuckyBag() { }

	TYPE_ITEM_NUMBER		GetMaxNumber() const;

	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_LUCKY_BAG; }

	static MItem*			NewItem()					{ return new MLuckyBag; }
};

class MSms_item : public MItem {
public :
	MSms_item() { }
	~MSms_item() { }

	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_SMS_ITEM; }


	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
	
	static MItem*			NewItem()					{ return new MSms_item; }
};


class MCoreZap : public MAccessoryItem {
public :
	MCoreZap() { }
	~MCoreZap() { }

	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_CORE_ZAP; }

	
	static MItem*			NewItem()					{ return new MCoreZap; }
	bool	IsGearSlotCoreZap() const					{ return true; }
	bool	IsGearSlotVampireCoreZap() const			{ return true; }
	bool	IsGearSlotOustersCoreZap() const			{ return true; }
	bool	IsDurationAlwaysOkay() const		{ return true; }	
};

class MGQuestItem : public MItem {
public :
	MGQuestItem() { }
	~MGQuestItem() { }

	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_GQUEST_ITEM; }
	static MItem*			NewItem()					{ return new MGQuestItem; }

};


class MTrapItem : public MItem {
public :
	MTrapItem() { }
	~MTrapItem() { }

	// use item
	#ifdef __TEST_SUB_INVENTORY__   
		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);
	#else
		void		UseInventory();
	#endif
	
	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_TRAP_ITEM; }
	static MItem*			NewItem()					{ return new MTrapItem; }

};
 

class MBloodBibleSign : public MGearItem {
public :
	MBloodBibleSign() { }
	~MBloodBibleSign() { }

	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_BLOOD_BIBLE_SIGN; }
	
	static MItem*			NewItem()					{ return new MBloodBibleSign; }

	ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_BLOOD_BIBLE; }

	bool	IsGearSlotBloodBible() const				{ return true; }
	bool	IsGearSlotVampireBloodBible() const			{ return true; }
	bool	IsGearSlotOustersBloodBible() const			{ return true; }

	bool	IsDurationAlwaysOkay() const		{ return true; }	
};

class MWarItem : public MItem {
public :
	MWarItem() { }
	~MWarItem() { }

	ITEM_CLASS				GetItemClass() const		{ return ITEM_CLASS_WAR_ITEM; }
	static MItem*			NewItem()					{ return new MWarItem; }

};



class MCarryingReceiver : public MAccessoryItem {
	public :
		MCarryingReceiver() {}
		~MCarryingReceiver() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_CARRYING_RECEIVER; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_PDA; }
//
		static MItem*	NewItem()	{ return new MCarryingReceiver; }

		bool		IsGearSlotPDA() const			{ return true; }	
		bool	IsDurationAlwaysOkay() const		{ return true; }	
};

class MShoulderArmor : public MAccessoryItem {
	public :
		MShoulderArmor() {}
		~MShoulderArmor() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SHOULDER_ARMOR; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_SHOULDER; }

		static MItem*	NewItem()	{ return new MShoulderArmor; }

		bool		IsGearSlotShoulder() const		{ return true; }	
		bool	IsDurationAlwaysOkay() const		{ return true; }	
		bool		IsAddonItem() const				{ return true; }
};
class MDermis : public MAccessoryItem {
	public :
		MDermis() {}
		~MDermis() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_DERMIS; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_VAMPIRE_DERMIS; }
		//bool		IsGearSlotVampireRing() const			{ return true; }

		static MItem*	NewItem()	{ return new MDermis; }
		bool		IsGearSlotVampireDermis() const		{ return true; }	
		bool	IsDurationAlwaysOkay() const		{ return true; }	
};

class MPersona : public MAccessoryItem {
	public :
		MPersona() {}
		~MPersona() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_PERSONA; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_VAMPIRE_PERSONA; }
//		bool		IsGearSlotVampireRing() const			{ return true; }

		static MItem*	NewItem()	{ return new MPersona; }
		bool		IsGearSlotVampirePersona() const		{ return true; }	
		bool	IsDurationAlwaysOkay() const		{ return true; }	
};


class MFascia : public MAccessoryItem {
	public :
		MFascia() {}
		~MFascia() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_FASCIA; }

		ITEM_GEAR_SLOT	GetGearSlot() const				{ return SLOT_OUSTERS_FASCIA; }
//		bool		IsGearSlotVampireRing() const			{ return true; }

		static MItem*	NewItem()	{ return new MFascia; }
		bool		IsGearSlotOustersFascia() const		{ return true; }	
		bool	IsDurationAlwaysOkay() const		{ return true; }	
};

class MMitten : public MArmorItem {
	public :
		MMitten() {}
		~MMitten() {}

	public :
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_MITTEN; }

		ITEM_GEAR_SLOT	GetGearSlot() const			{ return SLOT_OUSTERS_MITTEN; }
//		bool		IsGearSlotCoat() const			{ return true; }

//		bool		IsAddonItem() const				{ return true; }
//		ADDON		GetAddonSlot() const			{ return ADDON_COAT; }

		static MItem*	NewItem()	{ return new MMitten; }
		bool		IsGearSlotOustersMitten() const		{ return true; }	
		bool	IsDurationAlwaysOkay() const		{ return true; }	
};

#ifdef __TEST_SUB_INVENTORY__   

class MSubInventory : public MItem , public MGridItemManager {
	public :
		MSubInventory() {}
		~MSubInventory() {}

	public :
		
		ITEM_CLASS	GetItemClass() const		{ return ITEM_CLASS_SUB_INVENTORY; }

		void		UseInventory(TYPE_OBJECTID SubInventoryItemID = 0);

		static MItem*	NewItem()	{ return new MSubInventory; }

		void			SetItemType(TYPE_ITEMTYPE type);

		bool			IsContainerItem() const		{ return true; }

		//------------------------------------------------
		//
		//				SlotItemManager
		//
		//------------------------------------------------
		
		//------------------------------------------------
		bool			AddItem(MItem* pItem);
		bool			AddItem(MItem* pItem, BYTE X, BYTE Y);
		
		//------------------------------------------------
		
		//------------------------------------------------
		bool			ReplaceItem(MItem* pItem, BYTE X, BYTE Y, MItem*& pOldItem);	

		//------------------------------------------------
		
		
		//------------------------------------------------
		bool			CanReplaceItem(MItem* pItem,BYTE X, BYTE Y, MItem*& pOldItem);
};
#endif
#endif