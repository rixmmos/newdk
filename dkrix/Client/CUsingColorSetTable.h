//----------------------------------------------------------------------
// CUsingColorSetTable.h
//----------------------------------------------------------------------




//----------------------------------------------------------------------

#ifndef	__CUSINGCOLORSETTABLE_H__
#define	__CUSINGCOLORSETTABLE_H__

#include "CIndexSprite.h"
#include "CTypeTable.h"

#include <fstream>
using namespace std;


class USINGCOLORSET_NODE {
	public :
		USINGCOLORSET_NODE();
		~USINGCOLORSET_NODE();

		//-------------------------------------------------
		// Enable / Disable
		//-------------------------------------------------
		void			Enable(int set);
		void			Disable(int set);

		//-------------------------------------------------
		
		//-------------------------------------------------
		void			Enable(int start, int end);
		void			Disable(int start, int end);

		//-------------------------------------------------
		// File I/O
		//-------------------------------------------------
		void			SaveToFile(std::ofstream& file);
		bool			LoadFromFile(std::ifstream& file);

	protected :
		bool			m_bEnable[MAX_COLORSET];
};


typedef CTypeTable<USINGCOLORSET_NODE>		USINGCOLORSET_TABLE;
extern 	USINGCOLORSET_TABLE*	g_pUsingColorSetTable;


//----------------------------------------------------------------------

//----------------------------------------------------------------------
enum USINGCOLORSET
{
	USINGCOLORSET_NONE,
	USINGCOLORSET_ALL,
	USINGCOLORSET_SKIN,
	USINGCOLORSET_ITEM,
	MAX_USINGCOLORSET
};


#endif

