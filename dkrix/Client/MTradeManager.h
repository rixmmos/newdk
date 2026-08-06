//-----------------------------------------------------------------------------
// MTradeManager.h
//-----------------------------------------------------------------------------

//

// 

//

//

//

//

//-----------------------------------------------------------------------------

#ifndef __MTRADEMANAGER_H__
#define __MTRADEMANAGER_H__

#include "MInventory.h"
#include "MMoneyManager.h"

class MTradeManager {
	public :
		MTradeManager();
		~MTradeManager();

		//-------------------------------------------------------
		// Init / Release
		//-------------------------------------------------------
		void				Init();
		void				Release();

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		MInventory*			GetMyInventory() const			{ return m_pMyInventory; }
		MInventory*			GetOtherInventory() const		{ return m_pOtherInventory; }

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		MMoneyManager*		GetMyMoneyManager() const		{ return m_pMyMoney; }
		MMoneyManager*		GetOtherMoneyManager() const	{ return m_pOtherMoney; }

		//-------------------------------------------------------
		// Trade OK ?
		//-------------------------------------------------------
		bool				IsAcceptTime() const;
		void				SetNextAcceptTime();		

		bool				IsAcceptMyTrade() const				{ return m_bAcceptMyTrade; }
		bool				IsAcceptOtherTrade() const			{ return m_bAcceptOtherTrade; }

		void				AcceptMyTrade()						{ m_bAcceptMyTrade = true; }
		void				AcceptOtherTrade()					{ m_bAcceptOtherTrade = true; }

		void				RefuseMyTrade();
		void				RefuseOtherTrade();

		//-------------------------------------------------------
		// Trade
		//-------------------------------------------------------
		bool				CanTrade() const;		
		bool				Trade();				
		bool				CancelTrade();			

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void				SetOtherID(TYPE_OBJECTID otherID)	{ m_OtherID = otherID; }
		TYPE_OBJECTID		GetOtherID() const					{ return m_OtherID; }
		void				SetOtherName(const char* pName)		{ m_OtherName = pName; }
		const char*			GetOtherName() const				{ return m_OtherName.GetString(); }

	protected :
		TYPE_OBJECTID		m_OtherID;				
		MString				m_OtherName;			

		MInventory*			m_pMyInventory;			
		MInventory*			m_pOtherInventory;		

		MMoneyManager*		m_pMyMoney;				
		MMoneyManager*		m_pOtherMoney;			

		bool				m_bAcceptMyTrade;			
		bool				m_bAcceptOtherTrade;		

		DWORD				m_NextAcceptTime;		
};

extern MTradeManager*		g_pTradeManager;

#endif

