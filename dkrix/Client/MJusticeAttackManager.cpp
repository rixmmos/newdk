//----------------------------------------------------------------------
// MJusticeAttackManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MJusticeAttackManager.h"
#include "ClientConfig.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MJusticeAttackManager* g_pJusticeAttackManager = NULL;

extern DWORD g_CurrentTime;

//----------------------------------------------------------------------
//
// constructor / destructor
// 
//----------------------------------------------------------------------
MJusticeAttackManager::MJusticeAttackManager()
{
}

MJusticeAttackManager::~MJusticeAttackManager()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MJusticeAttackManager::Release()
{
	m_Creatures.clear();
}

//----------------------------------------------------------------------
// Add Creature
//----------------------------------------------------------------------
void		
MJusticeAttackManager::AddCreature(const char* pName)
{
	std::string name = pName;

	
	
	m_Creatures[name] = g_CurrentTime;
}

//----------------------------------------------------------------------
// Remove Creature
//----------------------------------------------------------------------
bool		
MJusticeAttackManager::RemoveCreature(const char* pName)
{
	TIME_MAP::iterator iTime = m_Creatures.find( std::string(pName) );

	if (iTime != m_Creatures.end())
	{
		m_Creatures.erase( iTime );

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Has Creature
//----------------------------------------------------------------------
bool
MJusticeAttackManager::HasCreature(const char* pName) const
{
	TIME_MAP::const_iterator iTime = m_Creatures.find( std::string(pName) );

	if (iTime != m_Creatures.end())
	{
		return true;
	}

	return false;
}
