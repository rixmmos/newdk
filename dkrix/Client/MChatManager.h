//----------------------------------------------------------------------
// MChatManager.h
//----------------------------------------------------------------------
// Is Accept ID?


//----------------------------------------------------------------------

#ifndef __MCHATMANAGER_H__
#define __MCHATMANAGER_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include "MStringMap.h"

class MChatManager {
	public :
		MChatManager();
		~MChatManager();

		//-------------------------------------------------------
		// Accept/Ignore Mode
		//-------------------------------------------------------
		void				SetIgnoreMode()					{ m_bIgnoreMode = true; }
		void				SetAcceptMode()					{ m_bIgnoreMode = false; }
		bool				IsIgnoreMode() const			{ return m_bIgnoreMode; }
		bool				IsAcceptMode() const			{ return !m_bIgnoreMode; }

		//-------------------------------------------------------
		// ID
		//-------------------------------------------------------
		void				ClearID()						{ m_mapID.Release(); }	
		bool				AddID(const char* pID)			{ return m_mapID.Add(pID); }
		bool				RemoveID(const char* pID)		{ return m_mapID.Remove(pID); }
		bool				IsAcceptID(const char* pID)	const	
							{ 
								if (m_bIgnoreMode) 
									return m_mapID.Get(pID)!=NULL; 
								else
									return m_mapID.Get(pID)==NULL;
							}

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		bool				RemoveCurse(char* str, bool bForce = false) const;
		void				AddMask(char* str, int percent) const;


		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void				SaveToFile(const char* filename);
		void				LoadFromFile(const char* filename);		
		void				LoadFromFileCurse(const char* filename);

	protected :
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		//bool				RemoveCurseKorean(const char* str, int byteCurse, const MStringMap& mapCurse, bool* isCurse) const;
		bool				RemoveCurseKorean(const char* str, int byteCurse, const MStringMap& mapCurse, BYTE* isCurse) const;

	protected :
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		MStringMap			m_mapCurseEng;
		MStringMap			m_mapCurseKor1;
		MStringMap			m_mapCurseKor2;
		MStringMap			m_mapCurseKor3;
		MStringMap			m_mapCurseKor4;

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		bool				m_bIgnoreMode;
		MStringMap			m_mapID;

		static char			s_MaskString[256];
		static char			s_MaskString2[256];
};

extern MChatManager*		g_pChatManager;

#endif

