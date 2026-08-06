//----------------------------------------------------------------------
// CPositionList.h
//----------------------------------------------------------------------
// 

//



//

//
//----------------------------------------------------------------------

#ifndef	__CPOSITIONLIST_H__
#define	__CPOSITIONLIST_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include <list>
//#include <fstream.h>



//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
template <class Type>
class POSITION_NODE {
	public :
		void	operator = (const POSITION_NODE& node)
		{
			X = node.X;
			Y = node.Y;
		}

		bool	operator == (const POSITION_NODE& node)
		{
			return X==node.X && Y==node.Y;
		}

		bool	operator > (const POSITION_NODE& node)
		{
			if (X > node.X) return true;
			if (X < node.X) return false;
			if (Y > node.Y) return true;
			return false;
		}

		bool	operator < (const POSITION_NODE& node)
		{
			if (X < node.X) return true;
			if (X > node.X) return false;
			if (Y < node.Y) return true;
			return false;
		}

	public :
		Type			X;
		Type			Y;
};



//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
template <class Type>
class CPositionList {
	public :
		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		typedef	POSITION_NODE<Type>				POSITION_NODE_TYPE;
		typedef	std::list<POSITION_NODE_TYPE>	POSITION_LIST;

	public :
		CPositionList();
		~CPositionList(); 

		//--------------------------------------------------------------
		// Release
		//--------------------------------------------------------------
		void		Release();

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		bool		Add(Type x, Type y);

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		bool		Remove(Type x, Type y);

		//--------------------------------------------------------------
		// Get 
		//--------------------------------------------------------------
		int								GetSize() const		{ return m_listPosition.size(); }
		typename POSITION_LIST::const_iterator	GetIterator() const	{ return m_listPosition.begin(); }


		//--------------------------------------------------------------
		// File I/O
		//--------------------------------------------------------------
		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(std::ifstream& file);

	

	protected :
		POSITION_LIST		m_listPosition;

		
		static BYTE			s_SizeOfPositionType;
};


//----------------------------------------------------------------------
// CPositionList.cpp
//----------------------------------------------------------------------

//#include "CPositionList.h"

//----------------------------------------------------------------------
// Init Static Member
//----------------------------------------------------------------------
template <class Type>
BYTE	CPositionList<Type>::s_SizeOfPositionType	= sizeof(Type);

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
template <class Type>
CPositionList<Type>::CPositionList()
{
}

template <class Type>
CPositionList<Type>::~CPositionList()
{
	Release();
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
template <class Type>
void
CPositionList<Type>::Release()
{
	m_listPosition.clear();
}

//----------------------------------------------------------------------
// Add(x,y)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
template <class Type>
bool
CPositionList<Type>::Add(Type x, Type y)
{
	
	POSITION_NODE<Type>	node;
	node.X = x;
	node.Y = y;

	typename POSITION_LIST::iterator iPosition = m_listPosition.begin();

	
	
	while (iPosition != m_listPosition.end())
	{
		
		
		if (*iPosition > node)
		{
			m_listPosition.insert(iPosition, node);
			return true;
		}

		
		
		if (*iPosition==node)
		{
			return false;
		}

		iPosition++;
	}	

	
	
	m_listPosition.push_back( node );

	return true;
}

//----------------------------------------------------------------------
// Remove
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <class Type>
bool		
CPositionList<Type>::Remove(Type x, Type y)
{
	
	POSITION_NODE<Type>	node;
	node.X = x;
	node.Y = y;

	typename POSITION_LIST::iterator iPosition = m_listPosition.begin();

	
	while (iPosition != m_listPosition.end())
	{		
		
		if (*iPosition==node)
		{
			m_listPosition.erase(iPosition);
			return true;
		}
		
		
		
		if (*iPosition > node)
		{			
			return false;
		}

		iPosition++;
	}	

	
	return false;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <class Type>
void		
CPositionList<Type>::SaveToFile(std::ofstream& file)
{
	//----------------------------------------
	
	//----------------------------------------
	WORD size = m_listPosition.size();

	file.write((const char*)&size, 2);

	
	if (size==0)
		return;


	typename POSITION_LIST::iterator iPosition = m_listPosition.begin();

	POSITION_NODE<Type>	node;

	//----------------------------------------
	
	//----------------------------------------
	while (iPosition != m_listPosition.end())
	{		
		node = *iPosition;

		file.write((const char*)&node.X, s_SizeOfPositionType);
		file.write((const char*)&node.Y, s_SizeOfPositionType);

		iPosition++;
	}	
	
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
template <class Type>
void		
CPositionList<Type>::LoadFromFile(std::ifstream& file)
{
	
	Release();

	WORD size;

	//----------------------------------------
	
	//----------------------------------------
	file.read((char*)&size, 2);

	
	if (size==0)
		return;

	POSITION_NODE<Type> node;

	//----------------------------------------
	
	
	//----------------------------------------
	for (int i=0; i<size; i++)
	{		
		file.read((char*)&node.X, s_SizeOfPositionType);
		file.read((char*)&node.Y, s_SizeOfPositionType);

		m_listPosition.push_back( node );
	}
}


//----------------------------------------------------------------------
// Test Code
//----------------------------------------------------------------------
/*
template <class Type>
void		
CPositionList<Type>::Print()
{

	typename POSITION_LIST::iterator iPosition = m_listPosition.begin();

	POSITION_NODE<Type> node;

	while (iPosition != m_listPosition.end())
	{
		node = *iPosition;

		cout << "( " << node.X << ", " << node.Y << " )" << endl;

		iPosition++;
	}
}


void main()
{
	CPositionList<int>	list;
	
	list.Add(10,10);	
	list.Add(16,17);
	list.Add(10,12);
	list.Add(12,17);
	list.Remove(12,16);
	list.Add(10,12);	
	list.Add(11,15);
	list.Add(10,11);
	list.Remove(10,10);
	list.Add(11,15);
	list.Add(10,11);
	list.Remove(12,17);
	list.Add(11,9);
	list.Remove(11,9);
	list.Add(11,9);	
	list.Add(10,7);

	list.Print();
}


//----------
// Result 
//----------
( 10, 7 )
( 10, 11 )
( 10, 12 )
( 11, 9 )
( 11, 15 )
( 16, 17 )


*/


#endif
