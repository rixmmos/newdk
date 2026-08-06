//----------------------------------------------------------------------
// TempInformation.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__TEMPINFORMATION_H__
#define	__TEMPINFORMATION_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif

class TempInformation {
	public :
		enum TEMP_MODE
		{
			MODE_NULL,
			MODE_SKILL_LEARN,		// (domainType, maxLevel, skillType)
			MODE_SHOP_BUY,			// (shelfType, index, x, y, MShop*)
			MODE_SHOP_SELL,			// (MItem*)
			MODE_SHOP_SELL_ALL_SKULL,		
			MODE_SHOP_REPAIR,		// (MItem*)
			MODE_SHOP_SILVERING,	// (MItem*)
			MODE_STORAGE_BUY,		// (price)
			MODE_STORAGE_BUY_WAIT,	// (nothing.. - -)

			MODE_TRADE_REQUEST,		// (ID)
			MODE_TRADE_VERIFY_DROP_TO_INVENTORY,		// (ID, x, y, MItem*)
			MODE_TRADE_VERIFY_INSERT_TO_INVENTORY,
			MODE_TRADE_VERIFY_PICKUP_FROM_INVENTORY,	// (ID, x, y, MItem*)
			MODE_TRADE_VERIFY_PICKUP_TO_QUICKSLOT,		// (slot,     MItem*)

			MODE_TRADE_VERIFY_CANCEL,					
			MODE_TRADE_VERIFY_CLOSE,					
			MODE_TRADE_VERIFY_ADD_MONEY,				// (money)
			MODE_TRADE_VERIFY_REMOVE_MONEY,				// (money)
			MODE_TRADE_VERIFY_ADD_ITEM,					// (ID, x, y, MItem*)
			MODE_TRADE_VERIFY_REMOVE_ITEM,				// (ID, x, y, MItem*)

			MODE_PARTY_REQUEST,		// (ID)

			MODE_SKILL_MAKE_ITEM,	// (itemClass, itemType)

			MODE_BONUSPOINT_USE,	// (part(0,1,2=str,dex,int))
	
			MODE_WAIT_RESURRECT,	

			MODE_ITEM_TO_ITEM,		
			MODE_WAIT_BRING_FEE,	
			MODE_INPUT_NAME,		
			MODE_SELECT_QUEST,		
			MODE_ITEM_MIXING,		
			MODE_ITEM_CODE_SHEET,	

			MODE_PETITEM_MOVETO_INVENTORY, 
			MODE_PETITEM_MOVETO_PETSTORAGE, 

			MODE_SMS_ADD_ADDRESS,	
			MODE_SMS_DELETE_ADDRESS,
			
			MODE_NICKNAME_CHANGE_CUSTOM, 
			MODE_NICKNAME_SELECT_NICK, 

			MAX_MODE
		};

	public :
		TempInformation();
		~TempInformation();

		//--------------------------------------------------------
		// File I/O
		//--------------------------------------------------------
		//void	SaveToFile(const char* filename);
		//bool	LoadFromFile(const char* filename);
		TEMP_MODE		Mode;

	public :
		void			SetMode(TEMP_MODE mode);
		const TEMP_MODE GetMode() const;

		int				Value1;
		int				Value2;
		int				Value3;
		int				Value4;

		int				PartyInviter;	

		void*			pValue;
};


extern	TempInformation*		g_pTempInformation;

#endif

