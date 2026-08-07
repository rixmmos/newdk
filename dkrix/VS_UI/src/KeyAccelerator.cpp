//----------------------------------------------------------------------
// KeyAccelerator.cpp
//----------------------------------------------------------------------

#include "client_PCH.h"
#include <fstream>
#include "KeyAccelerator.h"
#include "DXLib/InputManager.h"
#include <cstdio>
//----------------------------------------------------------------------
// define functions
//----------------------------------------------------------------------
#define READ_CHECK_EOF( value, temp, bytes )		\
		{											\
			file.read((char*)&temp, bytes);			\
			if (!file.eof())						\
			{										\
				value = temp;						\
				return false;						\
			}										\
			else									\
			{										\
				return true;						\
			}										\
		}
//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
KeyAccelerator*		g_pKeyAccelerator = NULL;

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
KeyAccelerator::KeyAccelerator()
{
}

KeyAccelerator::~KeyAccelerator()
{
}

//----------------------------------------------------------------------
// Init ( max )
//----------------------------------------------------------------------
void
KeyAccelerator::Init( int max )
{
	m_Keys.clear();
	m_Accelerators.clear();

	// Use resize() instead of reserve() to actually allocate elements
	m_Accelerators.resize( max, 0 );
}

//----------------------------------------------------------------------
// Set AcceleratorKey ( accel, key )
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
KeyAccelerator::SetAcceleratorKey(BYTE accel, WORD key)
{
	// Check against size() not capacity(), and resize if needed
	if (accel >= m_Accelerators.size())
	{
		m_Accelerators.resize(accel + 1, 0);
	}

	KEY_MAP::iterator iKey = m_Keys.find( m_Accelerators[accel] );

	if (iKey != m_Keys.end())
	{
		
		m_Keys.erase( iKey );
	}

	m_Keys[key] = accel;

	m_Accelerators[accel] = key;
}

//----------------------------------------------------------------------
// Get Accelerator ( key )
//----------------------------------------------------------------------

//----------------------------------------------------------------------
BYTE
KeyAccelerator::GetAccelerator(WORD key) const
{
	KEY_MAP::const_iterator iKey = m_Keys.find( key );

	if (iKey==m_Keys.end())
	{
		return 0;
	}

	return iKey->second;
}

//----------------------------------------------------------------------
// Get AcceleratorSimilar ( key )
//----------------------------------------------------------------------



//----------------------------------------------------------------------
BYTE				
KeyAccelerator::GetAcceleratorSimilar(WORD key) const
{
	
	KEY_MAP::const_iterator iKey = m_Keys.find( key );

	if (iKey!=m_Keys.end())
	{
		return iKey->second;
	}

	WORD checkKey;	

	int bHasControl	= ACCEL_HAS_CONTROL(key);
	int bHasAlt		= ACCEL_HAS_ALT(key);
	int bHasShift	= ACCEL_HAS_SHIFT(key);

	//------------------------------------------------------------
	
	//------------------------------------------------------------
//	if (!bHasControl && !bHasAlt && !bHasShift)
//	{
//		return 0;
//	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (bHasControl)
	{
		checkKey = ACCEL_REMOVE_CONTROL( key );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (bHasAlt)
	{
		checkKey = ACCEL_REMOVE_ALT( key );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (bHasShift)
	{
		checkKey = ACCEL_REMOVE_SHIFT( key );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;
	}

	WORD keyOnly = ACCEL_GET_KEY( key );

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (bHasControl && bHasAlt && bHasShift)
	{
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		checkKey = ACCEL_ADD_CONTROL( keyOnly );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;	

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		checkKey = ACCEL_ADD_ALT( keyOnly );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;		

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		checkKey = ACCEL_ADD_SHIFT( keyOnly );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	iKey = m_Keys.find( keyOnly );
	if (iKey!=m_Keys.end())	return iKey->second;

	return 0;
}

//----------------------------------------------------------------------
// Get Key ( accel )
//----------------------------------------------------------------------
WORD
KeyAccelerator::GetKey(BYTE accel) const
{
	if (accel < m_Accelerators.capacity())
	{	
		return m_Accelerators[accel];
	}

	return 0;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void				
KeyAccelerator::SaveToFile(FILE *file)
{
	int num = m_Accelerators.capacity();

	fwrite((const void*)&num, 1, 4, file);

	// Only the key-accelerator relationship needs to be saved.
	// Because accel is in order, so there is no need to save.
	// So we only need to store the key.
	for (int accel=1; accel<num; accel++)
	{
		WORD key = m_Accelerators[accel];
		fwrite((const void*)&key, 1, 2, file);
	}
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
void				
KeyAccelerator::LoadFromFile(FILE *file)
{
	int num;
	fread((void*)&num, 1, 4, file);

	Init( num );

	WORD key;

	for (int accel=1; accel<num; accel++)
	{
		int sz;
		sz = fread((void*)&key, 1, 2, file);
		if (sz == 2) {
			SetAcceleratorKey(accel, key);
		}
	}
}
