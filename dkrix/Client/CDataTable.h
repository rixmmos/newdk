//----------------------------------------------------------------------
// CDataTable.h
//----------------------------------------------------------------------

#ifndef	__CDATATABLE_H__
#define	__CDATATABLE_H__

//#include "DebugInfo.h"
//#define	 new DEBUG_NEW

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
template <class Type>
class CDataTable {
	public :
		CDataTable();
		~CDataTable();

		//-------------------------------------------------------
		// Init / Release
		//-------------------------------------------------------
		void			Init(int size);
		void			Release();

		//-------------------------------------------------------
		// Size
		//-------------------------------------------------------
		int				GetSize() const		{ return m_Size; }
		
		//-------------------------------------------------------
		// Reference
		//-------------------------------------------------------		
		const Type&	operator [] (int type) const	{ return m_pTypeInfo[type]; }
		Type&	operator [] (int type)				{ return m_pTypeInfo[type]; }
		Type&	Get(int type)						{ return m_pTypeInfo[type]; }


		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);
		void			LoadFromFile(std::ifstream& file);

	protected :		
		int			m_Size;					
		Type*		m_pTypeInfo;			

		static int	s_SizeOfData;
};

//----------------------------------------------------------------------
// static
//----------------------------------------------------------------------
template <class Type>
int CDataTable<Type>::s_SizeOfData = sizeof(Type);

//----------------------------------------------------------------------
//
//    constructor/destructor
//
//----------------------------------------------------------------------
template <class Type>
CDataTable<Type>::CDataTable()
{
	m_pTypeInfo	= NULL;
	m_Size		= 0;
}

template <class Type>
CDataTable<Type>::~CDataTable()
{
	Release();
}

//----------------------------------------------------------------------
//
//  member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
template <class Type>
void
CDataTable<Type>::Init(int size)
{
	
	if (size==0) 
		return;

	
	Release();

	
	m_Size = size;
	
	m_pTypeInfo = new Type [m_Size];	
}


//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
template <class Type>
void
CDataTable<Type>::Release()
{
	if (m_pTypeInfo != NULL)
	{
		
		delete [] m_pTypeInfo;
		m_pTypeInfo = NULL;
		
		m_Size = 0;
	}
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
template <class Type>
void			
CDataTable<Type>::SaveToFile(std::ofstream& file)
{
	
	file.write((const char*)&m_Size, 4);

	
	if (m_pTypeInfo==NULL)
		return;

	
	for (int i=0; i<m_Size; i++)
	{
		file.write((const char*)&m_pTypeInfo[i], s_SizeOfData);		
	}
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
template <class Type>
void			
CDataTable<Type>::LoadFromFile(std::ifstream& file)
{
	int numSize;

	
	file.read((char*)&numSize, 4);

	
	if (m_Size != numSize)
	{
		
		Release();

		
		Init( numSize );
	}

	
	for (int i=0; i<m_Size; i++)
	{
 		file.read((char*)&m_pTypeInfo[i], s_SizeOfData);
	}
}


#endif