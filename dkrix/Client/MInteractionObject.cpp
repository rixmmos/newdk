//----------------------------------------------------------------------
// MInteractionObject.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MInteractionObject.h"
#include "MInteractionObjectTable.h"
#include "ClientDef.h"
#include "MSoundTable.h"


//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
MInteractionObject::MInteractionObject()
{
	// type
	m_ObjectType	= TYPE_INTERACTIONOBJECT;	
	
	m_bAnimation	= true;
}

MInteractionObject::MInteractionObject(TYPE_INTERACTIONOBJECTTYPE iaotype, TYPE_OBJECTID id, TYPE_OBJECTID ImageObjectID, TYPE_SPRITEID SpriteID, int pX, int pY, TYPE_SECTORPOSITION viewpoint, bool trans, BYTE type)
{
	
	m_ID			= id;

	m_InteractionObjectType	 = iaotype;

	// type
	m_ObjectType	= TYPE_INTERACTIONOBJECT;	
	m_ImageObjectID = ImageObjectID;

	m_bAnimation	= true;

	// data
	m_SpriteID		= SpriteID;
	m_PixelX		= pX;
	m_PixelY		= pY;	
	m_Viewpoint		= viewpoint;
	m_bTrans		= trans;
	m_BltType	= type;
}

MInteractionObject::~MInteractionObject() 
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void	
MInteractionObject::SaveToFile(ofstream& file)
{	
	MAnimationObject::SaveToFile(file);	

	file.write((const char*)&m_InteractionObjectType, SIZE_INTERACTIONOBJECTTYPE);
}
		
//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
void	
MInteractionObject::LoadFromFile(ifstream& file)
{
	MAnimationObject::LoadFromFile(file);

	file.read((char*)&m_InteractionObjectType, SIZE_INTERACTIONOBJECTTYPE);
}

//----------------------------------------------------------------------
// Set Action
//----------------------------------------------------------------------
void
MInteractionObject::SetAction(BYTE action)
{ 
	 
}

//----------------------------------------------------------------------
// Set NextAction
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void			
MInteractionObject::SetNextAction(BYTE action)
{
	 
}

//----------------------------------------------------------------------
// ChangeAction Door
//----------------------------------------------------------------------
void
MInteractionObject::ChangeActionDoor()
{
	switch (m_CurrentFrame)
	{
		// Open
		case 0 :
			g_pZone->UnSetBlockAllSector( m_X, m_Y );
		break;

		// Closed
		case 1 :
			g_pZone->SetBlockAllSector( m_X, m_Y );
		break;
	}
}

//----------------------------------------------------------------------
// ChangeAction Door
//----------------------------------------------------------------------
void
MInteractionObject::ChangeActionTrap()
{
	switch (m_CurrentFrame)
	{
		// Hidden
		case 0 :			
		break;

		// Discovered
		case 1 :
		break;
	}
}

//----------------------------------------------------------------------
// ChangeAction Switch
//----------------------------------------------------------------------
void
MInteractionObject::ChangeActionSwitch()
{
	switch (m_CurrentFrame)
	{
		// UP
		case 0 :
			
		break;

		// DOWN
		case 1 :
		break;
	}
}