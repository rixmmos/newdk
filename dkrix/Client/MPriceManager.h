//-----------------------------------------------------------------------------
// MPriceManager.h
//-----------------------------------------------------------------------------

#ifndef __MPRICEMANAGER_H__
#define __MPRICEMANAGER_H__

class MItem;


struct STAR_ITEM_PRICE {
	int type;
	int number;
};

class MPriceManager {
	public :
		enum TRADE_TYPE
		{
			NPC_TO_PC,		
			PC_TO_NPC,		
			REPAIR,			
			SILVERING,		
		};

	public :
		MPriceManager(); 	
		~MPriceManager();

		//-------------------------------------------------------		
		// Get Item Price
		//-------------------------------------------------------		
		int			GetItemPrice(MItem* pItem, TRADE_TYPE type, bool bMysterious=false);
		void		GetItemPrice(MItem* pItem, STAR_ITEM_PRICE& price);
		int			GetMysteriousPrice(MItem* pItem) const;

		//-------------------------------------------------------		
		
		//-------------------------------------------------------		
		
		void		SetMarketCondBuy(int buy)			{ m_MarketCondBuy = buy; }
		void		SetMarketCondSell(int sell)			{ m_MarketCondSell = sell; }

		int			GetMarketCondBuy() const			{ return m_MarketCondBuy; }
		int			GetMarketCondSell() const			{ return m_MarketCondSell; }

		void		SetEventItemPrice(int Price)		{ m_EventFixPrice = Price; }

	protected :
		
		
		int					m_MarketCondBuy;		
		int					m_MarketCondSell;		
		int					m_EventFixPrice;		
};

extern MPriceManager*		g_pPriceManager;

#endif

