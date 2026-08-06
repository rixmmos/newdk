//-----------------------------------------------------------------------------
// MTimeItemManager.H									- by sonee
//-----------------------------------------------------------------------------
// MTimeItemManager

//  - 2003.04.04
//-----------------------------------------------------------------------------

#ifndef __TIME_ITEM_MANAGER_HEADER__
#define __TIME_ITEM_MANAGER_HEADER__

#pragma warning(disable:4786)

#include "MTypeDef.h"
#include <map>



class MTimeItemManager : public std::map<TYPE_OBJECTID, DWORD>
{
public :
	typedef std::map<TYPE_OBJECTID, DWORD>			TIMEITEM_MAP;

	MTimeItemManager();		
	~MTimeItemManager();
	
	bool	IsExist( TYPE_OBJECTID objectID );	
	bool	IsExpired( TYPE_OBJECTID objectID );

	bool	AddTimeItem( TYPE_OBJECTID objectID, DWORD time );			
	bool	RemoveTimeItem( TYPE_OBJECTID objectID );
	
	
	int		GetDay( TYPE_OBJECTID objectID );
	int		GetHour( TYPE_OBJECTID objectID );
	int		GetMinute( TYPE_OBJECTID objectID );
	int		GetSecond( TYPE_OBJECTID objectID );
};

extern MTimeItemManager		*g_pTimeItemManager;

#endif