//-----------------------------------------------------------------------------
// MMoneyManager.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#ifndef __MMONEYMANAGER_H__
#define __MMONEYMANAGER_H__

class MMoneyManager {

	public :
		MMoneyManager();
		MMoneyManager(const MMoneyManager& mm); 	
		~MMoneyManager();

		//-------------------------------------------------------
		// Money
		//-------------------------------------------------------		
		bool		SetMoney(int money);
		int			GetMoney() const		{ return m_Money; }

		bool		AddMoney(int money);
		bool		UseMoney(int money);

		
		bool		CanAddMoney(int money);
		bool		CanUseMoney(int money);

		//-------------------------------------------------------		
		// Money Limit
		//-------------------------------------------------------		
		void		SetMoneyLimit(int limit)	{ m_MoneyLimit = limit; }
		int			GetMoneyLimit() const		{ return m_MoneyLimit;}
		int			GetMaxAddMoney() const		{ return m_MoneyLimit - m_Money; }

	protected :
		int			m_MoneyLimit;	
		int			m_Money;		
};

extern MMoneyManager*		g_pMoneyManager;

#endif

