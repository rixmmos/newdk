//----------------------------------------------------------------------
// TArray.h
//----------------------------------------------------------------------
//
// Template Array class
//
//----------------------------------------------------------------------
//

//

//      bool		SaveToFile(std::ofstream& file);

//
//----------------------------------------------------------------------

#ifndef	__TARRAY_H__
#define	__TARRAY_H__


#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include <fstream>
using namespace std;

template <class DataType, class SizeType>
class TArray {
	public :
		TArray(SizeType size=0);
		~TArray();

		//--------------------------------------------------------
		// Init / Release
		//--------------------------------------------------------
		void		Init(SizeType size);
		void		Release();
	

		//--------------------------------------------------------
		// Get Size
		//--------------------------------------------------------
		SizeType	GetSize() const				{ return m_Size; }
	
		//--------------------------------------------------------
		// File I/O
		//--------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(std::ifstream& file);

		//--------------------------------------------------------
		// Operator overloading
		//--------------------------------------------------------
		DataType&	operator [] (SizeType n)		{ return m_pData[n]; }
		const DataType&	operator [] (SizeType n) const	{ return m_pData[n]; }
		void		operator = (const TArray<DataType, SizeType>& array);
		
		
		void		operator += (const TArray<DataType, SizeType>& array);


	protected :
		SizeType		m_Size;
		DataType*		m_pData;

		
		static BYTE		s_SIZEOF_SizeType;
};




//----------------------------------------------------------------------
//
// Initialize static data member
//
//----------------------------------------------------------------------
template <class DataType, class SizeType>
BYTE	TArray<DataType, SizeType>::s_SIZEOF_SizeType = sizeof(SizeType);


//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
template <class DataType, class SizeType> 
TArray<DataType, SizeType>::TArray(SizeType size)
{
	m_Size	= 0;
	m_pData = NULL;	

	Init(size);
}

template <class DataType, class SizeType> 
TArray<DataType, SizeType>::~TArray()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
template <class DataType, class SizeType> 
void	
TArray<DataType, SizeType>::Init(SizeType size)
{
	
	Release();

	if (size==0) return;

	m_Size = size;
	m_pData = new DataType [m_Size];
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
template <class DataType, class SizeType> 
void	
TArray<DataType, SizeType>::Release()
{
	if (m_pData!=NULL)
	{
		delete [] m_pData;
		m_pData = NULL;
		
		m_Size = 0;
	}
}

//----------------------------------------------------------------------
// Add Array to *this
//----------------------------------------------------------------------


//----------------------------------------------------------------------
template <class DataType, class SizeType>
void
TArray<DataType, SizeType>::operator += (const TArray<DataType, SizeType>& array)
{
	SizeType newSize = m_Size + array.m_Size;

	//------------------------------------------------
	
	//------------------------------------------------
	DataType*	pTempData = new DataType [newSize];
	
	//------------------------------------------------
	
	//------------------------------------------------
	SizeType k=0;
	for (SizeType i=0; i<m_Size; i++)
	{
		pTempData[k] = m_pData[i];		
		
		k++;
	}

	//------------------------------------------------
	
	//------------------------------------------------
	for (SizeType i=0; i<array.m_Size; i++)
	{
		pTempData[k] = array.m_pData[i];

		k++;
	}

	//------------------------------------------------
	
	//------------------------------------------------
	Release();

	//------------------------------------------------
	
	//------------------------------------------------
	m_Size		= newSize;
	m_pData		= pTempData;	
}


//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
template <class DataType, class SizeType> 
bool
TArray<DataType, SizeType>::SaveToFile(std::ofstream& file)
{
	
	file.write((const char*)&m_Size, s_SIZEOF_SizeType);

	
	if (m_pData==NULL || m_Size==0) 
		return false;

	for (SizeType i=0; i<m_Size; i++)
		m_pData[i].SaveToFile(file);

	return true;
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
template <class DataType, class SizeType> 
bool
TArray<DataType, SizeType>::LoadFromFile(std::ifstream& file)
{
	
	file.read((char*)&m_Size, s_SIZEOF_SizeType);

	if (m_Size==0) return false;
	
	
	Init(m_Size);

	for (SizeType i=0; i<m_Size; i++)
		m_pData[i].LoadFromFile(file);

	return true;
}

//----------------------------------------------------------------------
// assign
//----------------------------------------------------------------------
template <class DataType, class SizeType> 
void	
TArray<DataType, SizeType>::operator = (const TArray<DataType, SizeType>& array)
{
	
	Init( array.m_Size );

	
	for (SizeType i=0; i<m_Size; i++)
	{
		m_pData[i] = array.m_pData[i];
	}
}


#endif
