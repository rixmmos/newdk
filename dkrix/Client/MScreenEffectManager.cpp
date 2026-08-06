//-----------------------------------------------------------------------------
// MScreenEffectManager.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MScreenEffectManager.h"
#include "MEffect.h"
#include "MScreenEffect.h"
#include "MEffectGeneratorTable.h"
#include "UIFunction.h"
#include "MInventory.h"

//-----------------------------------------------------------------------------
// Update
//-----------------------------------------------------------------------------
void		
MScreenEffectManager::Update()
{
	EFFECT_LIST::iterator iEffect = m_listEffect.begin();

	EFFECT_LIST::iterator iTemp;
	
//	POINT point;

	MEffect* pEffect;
	int count = m_listEffect.size();

	for (int i=0; i<count; i++)	
	{
		pEffect = *iEffect;

 
		//---------------------------------------
		//
		
		//
		//---------------------------------------
		if (pEffect->Update())
		{
			
			iEffect++;

			//-----------------------------------------------
			//
			
			
			//
			
			//
			//-----------------------------------------------
			if (g_CurrentFrame >= pEffect->GetEndLinkFrame()
				&& pEffect->GetLinkSize() != 0)
			{
				
				
				
				(*g_pEffectGeneratorTable).GenerateNext( pEffect );

				
			}			
		}
		//---------------------------------------
		//
		
		//
		//---------------------------------------
		else
		{
			iTemp = iEffect;
			iEffect++;
			
			//-----------------------------------------------
			
			//-----------------------------------------------
			if (pEffect->GetLinkSize() != 0)
			{
				(*g_pEffectGeneratorTable).GenerateNext( pEffect );
			}
 
			//-----------------------------------------------
			
			//-----------------------------------------------
			delete pEffect;

			
			m_listEffect.erase(iTemp);
		}
	
	}
}

