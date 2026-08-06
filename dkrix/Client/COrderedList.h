//----------------------------------------------------------------------
// COrderedList.h
//----------------------------------------------------------------------
// 

//

//

//
//----------------------------------------------------------------------

#ifndef	__CORDEREDLIST_H__
#define	__CORDEREDLIST_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif
#include <list>




//----------------------------------------------------------------------
//
// COrderedList class
//
//----------------------------------------------------------------------
template <class Type>
class COrderedList {
	public :
		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		typedef	std::list<Type>	DATA_LIST;

	public :
		COrderedList();
		~COrderedList(); 

		//--------------------------------------------------------------
		// Release
		//--------------------------------------------------------------
		void		Release();

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		bool		Add(Type data);

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		bool		Remove(Type data);

		//--------------------------------------------------------------
		// Get 
		//--------------------------------------------------------------
		int								GetSize() const		{ return m_List.size(); }
		typename DATA_LIST::const_iterator	GetIterator() const	{ return m_List.begin(); }

		//--------------------------------------------------------------
		// operator
		//--------------------------------------------------------------
		void		operator	-= (const COrderedList<Type>& listSub);

	protected :
		DATA_LIST			m_List;
};


//----------------------------------------------------------------------
// COrderedList.cpp
//----------------------------------------------------------------------

//#include "COrderedList.h"

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
template <class Type>
COrderedList<Type>::COrderedList()
{
}

template <class Type>
COrderedList<Type>::~COrderedList()
{
	Release();
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
template <class Type>
void
COrderedList<Type>::Release()
{
	m_List.clear();
}

//----------------------------------------------------------------------
// Add(x,y)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
template <class Type>
bool
COrderedList<Type>::Add(Type data)
{
	typename DATA_LIST::iterator iData = m_List.begin();

	
	
	while (iData != m_List.end())
	{
		
		
		if (*iData > data)
		{
			m_List.insert(iData, data);
			return true;
		}

		
		
		if (*iData==data)
		{
			return false;
		}

		iData++;
	}	

	
	
	m_List.push_back( data );

	return true;
}

//----------------------------------------------------------------------
// Remove
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <class Type>
bool		
COrderedList<Type>::Remove(Type data)
{
	typename DATA_LIST::iterator iData = m_List.begin();

	
	while (iData != m_List.end())
	{		
		
		if (*iData==data)
		{
			m_List.erase(iData);
			return true;
		}
		
		
		
		if (*iData > data)
		{			
			return false;
		}

		iData++;
	}	

	
	return false;
}

//----------------------------------------------------------------------
// operator -=
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <class Type>
void		
COrderedList<Type>::operator -= (const COrderedList<Type>& listSub)
{
	typename DATA_LIST::iterator iDataThis = m_List.begin();
	typename DATA_LIST::const_iterator iDataSub = listSub.m_List.begin();
	typename DATA_LIST::iterator iDataTemp;

	
	while (iDataThis != m_List.end() && iDataSub != listSub.m_List.end())
	{		
		
		if (*iDataThis == *iDataSub)
		{
			iDataTemp = iDataThis;		

			
			iDataThis++;
			iDataSub++;

			
			m_List.erase( iDataTemp );
		}
		
		else if (*iDataThis < *iDataSub)
		{
			
			iDataThis++;
		}
		
		else
		{
			iDataSub++;
		}
	}	
}

#endif