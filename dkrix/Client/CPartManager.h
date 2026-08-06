//----------------------------------------------------------------------
// CPartManager.h
//----------------------------------------------------------------------


//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//	    0 : .					 0 : DataType[4] : 4
//	    1 : .					 1 : DataType[2] : 2
//	    2 : 1					 2 : DataType[3] : 3
//	    3 : 2					 
//	    4 : 0					
//	    5 : .					
//----------------------------------------------------------------------
//

//


//


//






//
//

//

//








//		...
//





//


//

//

//      LRU list :  MRU ----> LRU 
//

//



//
//----------------------------------------------------------------------

#ifndef	__CPARTMANAGER_H__
#define	__CPARTMANAGER_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include <list>
#include "DebugInfo.h"

template <class IndexType, class PartIndexType, class DataType>
class CPartManager {
	public :
		typedef	std::list<PartIndexType>	PARTINDEX_LIST;				
		typedef	typename PARTINDEX_LIST::iterator	PARTINDEX_LIST_ITERATOR;
		
	public :
		CPartManager();
		~CPartManager();

		//-------------------------------------------------------
		// Init/Release
		//-------------------------------------------------------
		void			Init(IndexType maxIndex, PartIndexType maxPart);
		void			Release();

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		bool			IsDataNULL(IndexType index)		{ return m_pPartIndex[index]==m_PartIndexNULL; }
		bool			IsDataNotNULL(IndexType index)	{ return m_pPartIndex[index]!=m_PartIndexNULL; }

		//-------------------------------------------------------
		// Get / Set		
		//-------------------------------------------------------
		void			SetData(IndexType index, const DataType& data);
		
		IndexType		SetData(IndexType index, const DataType& data, DataType& oldData);
		bool			GetData(IndexType index, DataType& data);

		
		PartIndexType	GetUsed() const					{ return m_nPartUsed; }
		IndexType		GetMaxIndex() const				{ return m_nIndex; }
		PartIndexType	GetMaxPartIndex() const			{ return m_nPart; }

	protected :
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		PartIndexType	GetLRU() const;
		DataType&		GetPartData(PartIndexType partIndex)	{ return m_pData[partIndex]; }


	protected :
		//-------------------------------------------------------
		// [Index] ( PartIndex )
		//-------------------------------------------------------
		IndexType			m_nIndex;			
		PartIndexType*		m_pPartIndex;		

		//-------------------------------------------------------
		// [PartIndex] ( Data : ParentIndex : LastTime )
		//-------------------------------------------------------
		PartIndexType		m_nPart;			
		PartIndexType		m_nPartUsed;		
		DataType*			m_pData;			
		IndexType*			m_pParentIndex;		
		DWORD*				m_pLastTime;		

		//-------------------------------------------------------
		// LRU list
		//-------------------------------------------------------
		PARTINDEX_LIST				m_listLRU;			// LRU list
		PARTINDEX_LIST_ITERATOR*	m_pPartIterator;	// iterator for LRU list
		
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		IndexType			m_IndexNULL;		
		PartIndexType		m_PartIndexNULL;	

		//-------------------------------------------------------
		// count
		//-------------------------------------------------------
		DWORD				m_Counter;
};


//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
template <class IndexType, class PartIndexType, class DataType>
CPartManager<IndexType, PartIndexType, DataType>::CPartManager()
{
	m_nIndex = 0;
	m_pPartIndex = NULL;

	m_nPart = 0;
	m_nPartUsed = 0;
	m_pData = NULL;
	m_pParentIndex = NULL;
	m_pLastTime = NULL;

	// LRU
	m_pPartIterator = NULL;

	
	m_IndexNULL	= (IndexType)0xFFFFFFFF;
	m_PartIndexNULL = (PartIndexType)0xFFFFFFFF;

	// timer
	m_Counter = 0;
}

template <class IndexType, class PartIndexType, class DataType>
CPartManager<IndexType, PartIndexType, DataType>::~CPartManager()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init(maxIndex, maxPart)
//----------------------------------------------------------------------
template <class IndexType, class PartIndexType, class DataType>
void		
CPartManager<IndexType, PartIndexType, DataType>::Init(IndexType maxIndex, PartIndexType maxPart)
{
	Release();

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (maxPart > maxIndex)
	{
		maxPart = maxIndex;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	m_nIndex = maxIndex;
	m_pPartIndex = new PartIndexType [m_nIndex];

	for (int i=0; i<m_nIndex; i++)
	{
		m_pPartIndex[i] = m_PartIndexNULL;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	m_nPart			= maxPart;
	m_nPartUsed		= 0;
	m_pData			= new DataType [m_nPart];
	m_pParentIndex	= new IndexType [m_nPart];	
	m_pLastTime		= new DWORD [m_nPart];		
	
	//------------------------------------------------------
	// LRU list
	//------------------------------------------------------
	m_listLRU.clear();
	m_pPartIterator		= new PARTINDEX_LIST_ITERATOR [m_nPart];	// iterator

	for (int i=0; i<m_nPart; i++)
	{
		m_pData[i] = NULL;
		m_pParentIndex[i] = m_IndexNULL;
		m_pLastTime[i] = 0;

		m_pPartIterator[i] = m_listLRU.end();
	}

	m_Counter = 0;
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
template <class IndexType, class PartIndexType, class DataType>
void		
CPartManager<IndexType, PartIndexType, DataType>::Release()
{
	if (m_pPartIndex!=NULL)
	{
		delete [] m_pPartIndex;
		m_pPartIndex = NULL;
		m_nIndex = 0;
	}

	if (m_pData!=NULL)
	{
		delete [] m_pData;
		m_pData = NULL;
		m_nPart = 0;
		m_nPartUsed = 0;
	}

	if (m_pParentIndex!=NULL)
	{
		delete [] m_pParentIndex;
		m_pParentIndex = NULL;
	}

	if (m_pLastTime!=NULL)
	{
		delete [] m_pLastTime;
		m_pLastTime = NULL;
	}
	
	if (m_pPartIterator!=NULL)
	{
		delete [] m_pPartIterator;
		m_pPartIterator = NULL;
	}

	m_listLRU.clear();
}

//----------------------------------------------------------------------
// Set Data
//----------------------------------------------------------------------


//----------------------------------------------------------------------
template <class IndexType, class PartIndexType, class DataType>
void
CPartManager<IndexType, PartIndexType, DataType>::SetData(IndexType index, const DataType& data)
{
	
	PartIndexType	newPartIndex = GetLRU();

	//------------------------------------------------------------
	
	//------------------------------------------------------------		
	IndexType oldIndex = m_pParentIndex[newPartIndex];
	if (oldIndex != m_IndexNULL)
	{
		
		m_pPartIndex[oldIndex] = m_PartIndexNULL;
		m_pParentIndex[newPartIndex] = m_IndexNULL;	
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	
	m_pPartIndex[index] = newPartIndex;
	m_pParentIndex[newPartIndex] = index;

	
	m_pData[newPartIndex] = data;

	
	m_pLastTime[newPartIndex] = m_Counter;

	//------------------------------------------------------------
	// LRU list
	//------------------------------------------------------------
	
	
	m_listLRU.push_front( newPartIndex );
	m_pPartIterator[newPartIndex] = m_listLRU.begin();
	
	
	if (m_nPartUsed < m_nPart) 
		m_nPartUsed++;
}

//----------------------------------------------------------------------
// Set Data
//----------------------------------------------------------------------


//


//----------------------------------------------------------------------
template <class IndexType, class PartIndexType, class DataType>
IndexType
CPartManager<IndexType, PartIndexType, DataType>::SetData(IndexType index, const DataType& data, DataType& oldData)
{
	
	PartIndexType	newPartIndex = GetLRU();

	//bool bReplace = false;
	//------------------------------------------------------------
	
	//------------------------------------------------------------	
	IndexType oldIndex = m_pParentIndex[newPartIndex];
	if (oldIndex != m_IndexNULL)
	{
		
		m_pPartIndex[oldIndex] = m_PartIndexNULL;
		m_pParentIndex[newPartIndex] = m_IndexNULL;

		
		oldData = m_pData[newPartIndex];

		//bReplace = true;
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	
	m_pPartIndex[index] = newPartIndex;
	m_pParentIndex[newPartIndex] = index;

	
	m_pData[newPartIndex] = data;

	
	m_pLastTime[newPartIndex] = m_Counter++;

	//------------------------------------------------------------
	// LRU list
	//------------------------------------------------------------
	
	
	if (m_pPartIterator[newPartIndex]!=m_listLRU.end())
	{
		m_listLRU.erase( m_pPartIterator[newPartIndex] );		
	}
	m_listLRU.push_front( newPartIndex );
	m_pPartIterator[newPartIndex] = m_listLRU.begin();
	
	//---------------------------------------------------
	// [ TEST CODE ]
	/*
	PARTINDEX_LIST_ITERATOR	iIndex = m_listLRU.begin();

	char str[1024] = "[SetData] ";
	char str2[256];
	while (iIndex != m_listLRU.end())
	{
		sprintf(str2, "%d ", *iIndex);
		strcat(str, str2);

		iIndex++;
	}
	DEBUG_ADD(str);
	*/
	//---------------------------------------------------
	
	
	if (m_nPartUsed < m_nPart) 
		m_nPartUsed++;

	//return bReplace;

	return oldIndex;
}


//----------------------------------------------------------------------
// Get Data
//----------------------------------------------------------------------


//----------------------------------------------------------------------
template <class IndexType, class PartIndexType, class DataType>
bool
CPartManager<IndexType, PartIndexType, DataType>::GetData(IndexType index, DataType& data)
{
	PartIndexType partIndex = m_pPartIndex[index];

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (partIndex==m_PartIndexNULL)
	{
		return false;
	}

	
	m_pLastTime[partIndex] = m_Counter++;

	//------------------------------------------------------
	
	
	//------------------------------------------------------
	
	if (m_pPartIterator[partIndex]!=m_listLRU.end())
	{
		m_listLRU.erase( m_pPartIterator[partIndex] );		
	}
	m_listLRU.push_front( partIndex );				
	m_pPartIterator[partIndex] = m_listLRU.begin();		

	//---------------------------------------------------
	// [ TEST CODE ]
	/*
	PARTINDEX_LIST_ITERATOR	iIndex = m_listLRU.begin();

	char str[1024] = "[GetData] ";
	char str2[256];
	while (iIndex != m_listLRU.end())
	{
		sprintf(str2, "%d ", *iIndex);
		strcat(str, str2);

		iIndex++;
	}
	DEBUG_ADD(str);
	*/
	//---------------------------------------------------


	//------------------------------------------------------
	
	//------------------------------------------------------
	if (m_Counter==0xFFFFFFFF)
	{
		
		
		int leastTimeIndex = 0;
		int maxTimeIndex = 0;
		for (int i=1; i<m_nPart; i++)
		{
			
			if (m_pLastTime[i] < m_pLastTime[leastTimeIndex])
			{
				leastTimeIndex = i;
			}

			
			if (m_pLastTime[i] > m_pLastTime[maxTimeIndex])
			{
				maxTimeIndex = i;
			}
		}

		
		int leastTime = m_pLastTime[leastTimeIndex];
		for (int i=0; i<m_nPart; i++)
		{
			m_pLastTime[i] -= m_pLastTime[leastTimeIndex];
		}

		
		m_Counter = m_pLastTime[maxTimeIndex];
	}

	
	data = m_pData[partIndex];

	return true;
}

//----------------------------------------------------------------------
// Get LRU
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template <class IndexType, class PartIndexType, class DataType>
PartIndexType
CPartManager<IndexType, PartIndexType, DataType>::GetLRU() const
{
	//---------------------------------------------------
	
	
	//---------------------------------------------------
	if (m_nPartUsed < m_nPart)
	{	
		//DEBUG_ADD_FORMAT("[LRU] %d", m_nPartUsed);
		return m_nPartUsed;		
	}

	//---------------------------------------------------
	
	//---------------------------------------------------
	PartIndexType leastTimeIndex = *m_listLRU.rbegin();


	//---------------------------------------------------
	// [ TEST CODE ]
	/*
	PARTINDEX_LIST::const_iterator	iIndex = m_listLRU.begin();

	char str[1024] = "[GetLRU] ";
	char str2[256];
	while (iIndex != m_listLRU.end())
	{
		sprintf(str2, "%d ", *iIndex);
		strcat(str, str2);

		iIndex++;
	}
	DEBUG_ADD(str);
	*/
	//---------------------------------------------------

	//---------------------------------------------------
	
	//---------------------------------------------------
	 

	//---------------------------------------------------
	
	//---------------------------------------------------
	//DEBUG_ADD_FORMAT("[LRU] %d", leastTimeIndex);
	return leastTimeIndex;
}


#endif
