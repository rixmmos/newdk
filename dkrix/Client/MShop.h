//----------------------------------------------------------------------
// MShop.h
//----------------------------------------------------------------------

//

//


//

//----------------------------------------------------------------------
 
//----------------------------------------------------------------------

#ifndef __MSHOP_H__
#define	__MSHOP_H__

class MShopShelf;

class MShop {
	public :
		enum SHOP_TYPE
		{
			SHOP_NORMAL,
			SHOP_EVENT_STAR
		};

	public :
		MShop();
		~MShop();

		//-------------------------------------------------------
		// Init / Release
		//-------------------------------------------------------
		void				Init(unsigned int shopType);
		void				Release();
	
		//-------------------------------------------------------		
		// Shop Type
		//-------------------------------------------------------		
		SHOP_TYPE			GetShopType() const				{ return m_ShopType; }
		void				SetShopType(SHOP_TYPE type)		{ m_ShopType = type; }

		//-------------------------------------------------------		
		// Size
		//-------------------------------------------------------		
		unsigned int		GetSize() const		{ return m_Size; }

		//-------------------------------------------------------
		// Shelf
		//-------------------------------------------------------		
		bool				SetShelf(unsigned int shopType, MShopShelf* pShelf);
		MShopShelf*			GetShelf(unsigned int shopType) const;

		//-------------------------------------------------------		
		
		//-------------------------------------------------------		
		void				SetCurrent(unsigned int shopType);
		unsigned int		GetCurrent() const			{ return m_CurrentShelf; }
		MShopShelf*			GetCurrentShelf() const		{ return m_pShelf[m_CurrentShelf]; }


	protected :
		unsigned int		m_Size;			
		MShopShelf**		m_pShelf;

		
		unsigned int		m_CurrentShelf;

		
		SHOP_TYPE			m_ShopType;
};

#endif


