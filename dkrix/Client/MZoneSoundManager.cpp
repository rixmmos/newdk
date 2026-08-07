//-----------------------------------------------------------------------------
// MZoneSoundManager.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MZoneSoundManager.h"
#include "UserOption.h"
#include "DXLib/AudioManager.h"
#include "MSoundTable.h"
#include "DebugInfo.h"
#include "MTestDef.h"


#ifdef __GAME_CLIENT__
	#include "CSoundPartManager.h"
	#include "MZone.h"
	#include "MPlayer.h"

	#ifdef __METROTECH_TEST__
		#define OUTPUT_DEBUG_ZONESOUND_PROCESS
	#endif
#endif

//-----------------------------------------------------------------------------
// global
//-----------------------------------------------------------------------------
MZoneSoundTable*	g_pZoneSoundTable = NULL;

MZoneSoundManager*	g_pZoneSoundManager = NULL;

#ifdef __GAME_CLIENT__
	extern DWORD	g_CurrentTime;

	extern CSoundPartManager*	g_pSoundManager;
#endif

//-----------------------------------------------------------------------------
//
//						ZONESOUND_NODE
//
//-----------------------------------------------------------------------------
ZONESOUND_NODE::ZONESOUND_NODE(TYPE_SOUNDID id)
{
	m_SoundID	= id;
	m_pBuffer	= NULL;
	m_X			= -1; 
	m_Y			= -1;
	m_bLoop		= false;
	m_bContinueLoop = false;
}

ZONESOUND_NODE::~ZONESOUND_NODE()
{
	if (m_pBuffer!=NULL)
	{
#ifdef PLATFORM_WINDOWS
		m_pBuffer->Stop();
		m_pBuffer->Release();
#endif
		m_pBuffer = NULL;
	}
}

//-----------------------------------------------------------------------------
// Save To File
//-----------------------------------------------------------------------------
void					
ZONESOUND_NODE::SaveToFile(std::ofstream& file)
{
	
}

//-----------------------------------------------------------------------------
// Load From File
//-----------------------------------------------------------------------------
void					
ZONESOUND_NODE::LoadFromFile(std::ifstream& file)
{
	
}

//-----------------------------------------------------------------------------
// Play
//-----------------------------------------------------------------------------
void
ZONESOUND_NODE::Play(int x, int y, bool bLoop)
{	
#ifdef __GAME_CLIENT__
	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (!g_SDLAudio.IsInit() 
		|| m_SoundID >= g_pSoundTable->GetSize()
		|| !g_pUserOption->PlaySound)
	{
		return;
	}

	int playerX = g_pPlayer->GetX();
	int playerY = g_pPlayer->GetY();

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	int gapX = x - playerX;
	int gapY = y - playerY;

	int dist = max(abs(gapX), abs(gapY));
	
	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (m_pBuffer!=NULL
		&& m_playerX==playerX && m_playerY==playerY)
	{
		
		int oldGapX = m_X - m_playerX;
		int oldGapY = m_Y - m_playerY;

		int oldDist = max(abs(oldGapX), abs(oldGapY));
	
		
		if (oldDist < dist)
		{
			return;
		}
	}

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	m_X				= x;
	m_Y				= y;
	m_bLoop			= bLoop;
	m_bContinueLoop = bLoop;
	m_playerX		= playerX;
	m_playerY		= playerY;	

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (dist < 40)
	{			
		//-----------------------------------------------------------
		
		
		//-----------------------------------------------------------
		if (m_pBuffer==NULL)
		{
			DEBUG_ADD("[ZONESOUND_NODE] Play:New Buffer");

			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			if (g_pSoundManager->IsDataNULL(m_SoundID))
			{
				DEBUG_ADD("[ZONESOUND_NODE] Load Wave");

				
 				LPDIRECTSOUNDBUFFER pBuffer = g_SDLAudio.LoadWav( (*g_pSoundTable)[m_SoundID].Filename );

				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				if (pBuffer==NULL)
				{
					DEBUG_ADD_FORMAT("[Error] Failed to Load WAV. id=%d, fn=%s", m_SoundID, (const char*)(*g_pSoundTable)[m_SoundID].Filename );

					return;
				}
				else
				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				{
					
					LPDIRECTSOUNDBUFFER pOld;
					if ((*g_pSoundManager).SetData( m_SoundID, pBuffer, pOld )!=0xFFFF)
					{
#ifdef PLATFORM_WINDOWS
						pOld->Release();
#endif
					}
					
					//-----------------------------------------------------------
					
					//-----------------------------------------------------------
					
					m_pBuffer = g_SDLAudio.DuplicateSoundBuffer(pBuffer, false);
				}
			}
			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			else
			{
				DEBUG_ADD("[ZONESOUND_NODE] Already Exist Buffer");

				LPDIRECTSOUNDBUFFER pBuffer;
				if ((*g_pSoundManager).GetData(m_SoundID, pBuffer))
				{			
					//-----------------------------------------------------------
					
					//-----------------------------------------------------------
					
					m_pBuffer = g_SDLAudio.DuplicateSoundBuffer(pBuffer, false);
				}
			}
		}

		DEBUG_ADD("[ZONESOUND_NODE] Arrange Volume & Dist");
			
		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		if (m_pBuffer!=NULL)
		{
			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			if (gapX > 3)
			{
				g_SDLAudio.CenterToRightPan( m_pBuffer, (gapX-3) << 7 );
			}
			else if (gapX < -3)
			{
				g_SDLAudio.CenterToLeftPan( m_pBuffer, (abs(gapX+3)) << 7 );
			}
			else
			{
				g_SDLAudio.CenterPan( m_pBuffer );
			}

			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			int sub = (dist << 2) * g_pUserOption->VolumeSound;
			
			g_SDLAudio.SubVolumeFromMax(m_pBuffer, sub);

			//-----------------------------------------------------------
			
			
			//-----------------------------------------------------------
			DEBUG_ADD("[ZONESOUND_NODE] Play Buffer");
	
			if (!g_SDLAudio.IsPlay( m_pBuffer ))
			{
				g_SDLAudio.Play( m_pBuffer, m_bLoop );
			}
		}
	}
#endif
}

//-----------------------------------------------------------------------------
// Stop
//-----------------------------------------------------------------------------
void
ZONESOUND_NODE::Stop()
{
	//-----------------------------------------------------------
	//
	//-----------------------------------------------------------
	if (m_pBuffer!=NULL)
	{
		if (g_SDLAudio.IsPlay( m_pBuffer ))
		{
			g_SDLAudio.Stop( m_pBuffer );
		}
	}

	m_bContinueLoop = false;
}

//-----------------------------------------------------------------------------
// Stop
//-----------------------------------------------------------------------------
void
ZONESOUND_NODE::StopLoop()
{
	//-----------------------------------------------------------
	//
	//-----------------------------------------------------------
	if (m_pBuffer!=NULL)
	{
		if (g_SDLAudio.IsPlay( m_pBuffer ))
		{
#ifdef PLATFORM_WINDOWS
			m_pBuffer->Play(0, 0, 0);	
#endif
		}
	}

	m_bContinueLoop = false;
}

//-----------------------------------------------------------------------------
//
//								MZoneSoundTable
//
//-----------------------------------------------------------------------------
MZoneSoundTable::MZoneSoundTable()
{
}

MZoneSoundTable::~MZoneSoundTable()
{
}

//-----------------------------------------------------------------------------
//
//								MZoneSoundManager
//
//-----------------------------------------------------------------------------
MZoneSoundManager::MZoneSoundManager()
{
	m_LastX = -1;
	m_LastY = -1;
	m_LastUpdateTime = 0;
}

MZoneSoundManager::~MZoneSoundManager()
{
}

//-----------------------------------------------------------------------------
// Update Sound
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void			
MZoneSoundManager::UpdateSound()
{
#ifdef __GAME_CLIENT__
	if (g_pZone==NULL 
		|| g_pPlayer==NULL)
	{
		return;
	}
		
	//--------------------------------------------------------------------
	
	
	//--------------------------------------------------------------------
	if (g_CurrentTime - m_LastUpdateTime > 5000
		|| g_pPlayer->GetX()!=m_LastX
		|| g_pPlayer->GetY()!=m_LastY)
	{
		#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
			DEBUG_ADD_FORMAT("Get Sector(%d,%d)", g_pPlayer->GetX(), g_pPlayer->GetY());
		#endif

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		CTypeMap<ZONESOUND_NODE>::iterator iPlaySound = begin();

		while (iPlaySound != end())
		{
			ZONESOUND_NODE* pNode = iPlaySound->second;

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			pNode->UnSetContinueLoop();
			
			iPlaySound++;
		}

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		const MSector& sector = g_pZone->GetSector( g_pPlayer->GetX(), g_pPlayer->GetY() );

		const SECTORSOUND_LIST& listSectorSound = sector.GetSectorSoundList();

		SECTORSOUND_LIST::const_iterator iSound = listSectorSound.begin();

		#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
			DEBUG_ADD_FORMAT("listSectorSound. size=%d", listSectorSound.size());	
		#endif

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		while (iSound != listSectorSound.end())
		{
			const SECTORSOUND_INFO& soundInfo = *iSound;

			int zoneSoundID = soundInfo.ZoneSoundID;
			int x = soundInfo.X;
			int y = soundInfo.Y;

			#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
				DEBUG_ADD_FORMAT("SectorSoundInfo. id=%d, (%d, %d)", zoneSoundID, x, y);
			#endif

			ZONESOUND_INFO* pInfo = g_pZoneSoundTable->GetData( zoneSoundID );

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			if (pInfo!=NULL)
			{
				if (pInfo->IsShowTime())
				{
					#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
						DEBUG_ADD("Get ZONESOUND_NODE");
					#endif
	
					ZONESOUND_NODE* pSound = GetData( zoneSoundID );

					//------------------------------------------------------
					// Play
					//------------------------------------------------------
					if (pSound==NULL)
					{
						
						pSound = new ZONESOUND_NODE( pInfo->SoundID );

						
						AddData( zoneSoundID, pSound );						
					}					
					
					#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
						DEBUG_ADD("Before Play");
					#endif

					pSound->Play( x, y, pInfo->Loop );					

					#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
						DEBUG_ADD("After Play");
					#endif

					//------------------------------------------------------
					
					//------------------------------------------------------
					#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
						DEBUG_ADD("Set NextPlayTime");
					#endif

					pInfo->SetNextShowTime();
				}
				//---------------------------------------------------------
				
				//---------------------------------------------------------
				else if (!pInfo->IsShowHour())
				{
					#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
						DEBUG_ADD("[for stop]Get ZONESOUND_NODE");
					#endif

					ZONESOUND_NODE* pSound = GetData( zoneSoundID );

					//------------------------------------------------------
					// Play
					//------------------------------------------------------
					if (pSound!=NULL)
					{
						pSound->Stop();
					}	
				}
				else
				{
					ZONESOUND_NODE* pSound = GetData( zoneSoundID );

					if (pSound->IsLoop())
					{
						pSound->SetContinueLoop();
					}

					#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
						DEBUG_ADD("Skip");
					#endif
				}

			}

			iSound++;
		}

		#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
			DEBUG_ADD("UpdateSectorSound OK");
		#endif

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		iPlaySound = begin();

		while (iPlaySound != end())
		{
			ZONESOUND_NODE* pNode = iPlaySound->second;

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			if (pNode->IsLoop() && !pNode->IsContinueLoop())
			{
				pNode->StopLoop();
			}

			iPlaySound++;
		}

		#ifdef OUTPUT_DEBUG_ZONESOUND_PROCESS
			DEBUG_ADD("zoneSoundNode OK");
		#endif

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		m_LastX = g_pPlayer->GetX();
		m_LastY = g_pPlayer->GetY();
		m_LastUpdateTime = g_CurrentTime;
	}
#endif
}