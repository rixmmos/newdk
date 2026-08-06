//----------------------------------------------------------------------
// MChatManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MChatManager.h"


#ifdef __GAME_CLIENT__
	#include "MGameStringTable.h"
	#include "UserOption.h"
#endif


//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MChatManager*		g_pChatManager = NULL;

//----------------------------------------------------------------------
// Static
//----------------------------------------------------------------------
char MChatManager::s_MaskString[256] = //"^^; -_-; !_!; o_O; *_*; m_m; u_u; p_q; =_=; -_+; $_$; v_v; Y_Y; o_o; O_O; w_w; #_#; ._.; n_n; &_&; @_@; 0_0; _-_; +_+; +_=; ~_~;";
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
		
char MChatManager::s_MaskString2[256] = 
		"#&*%!$#%&*@!%&$&#*&$@#*!%$&@#&*%!$#%&*@!%&$&#*&$@&#!%$*&%*#@#*!%$&@#&*%!$#%&*@!%&$&#*&$@#*!%$&@#&*!$#%&*@!%&$&#*&$@#*!%$&@#&*%!$#%&!$#%&*@!%&$&#*&$@#*!%$&@#&*%!$#%&*";
		//".....................................................................................................................................................................";

//----------------------------------------------------------------------
//
// constructor / destructor
// 
//----------------------------------------------------------------------
MChatManager::MChatManager()
{
	m_bIgnoreMode = false;
}

MChatManager::~MChatManager()
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
MChatManager::SaveToFile(const char* filename)
{
	std::ofstream file(filename, ios::binary);

	m_mapCurseEng.SaveToFile( file );
	m_mapCurseKor1.SaveToFile( file );
	m_mapCurseKor2.SaveToFile( file );
	m_mapCurseKor3.SaveToFile( file );
	m_mapCurseKor4.SaveToFile( file );
	m_mapID.SaveToFile( file );
	
	file.close();
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
void				
MChatManager::LoadFromFile(const char* filename)
{
	std::ifstream file(filename, ios::binary);

	m_mapCurseEng.LoadFromFile( file );
	m_mapCurseKor1.LoadFromFile( file );
	m_mapCurseKor2.LoadFromFile( file );
	m_mapCurseKor3.LoadFromFile( file );
	m_mapCurseKor4.LoadFromFile( file );
	m_mapID.LoadFromFile( file );

	file.close();
}

//----------------------------------------------------------------------
// Load From File Curse
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void				
MChatManager::LoadFromFileCurse(const char* filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{
		return;
	}

	char str[256];

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	while (!file.eof())
	{
		file >> str;

		bool bEng = true;
		bool bKor = true;

		char* strTemp = str;
		char ch;

		//-----------------------------------------------------
		
		//-----------------------------------------------------
		while (ch=*strTemp++)
		{
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			if (ch & 0x80)
			{
				if (*strTemp=='\0')
				{
					bKor = false;
					break;
				}

				strTemp++;

				bEng = false;
			}
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			else if (ch>='a' && ch<='z')
			{
				bKor = false;
			}
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			else				
			{
				bEng = false;
				bKor = false;

				break;
			}
		}

		//-----------------------------------------------------
		
		//-----------------------------------------------------
		if (bEng)
		{
			m_mapCurseEng.Add(str);
		}
		//-----------------------------------------------------
		
		//-----------------------------------------------------
		else if (bKor)
		{
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			switch (strlen(str))
			{
				case 2 : m_mapCurseKor1.Add(str); break;
				case 4 : m_mapCurseKor2.Add(str); break;
				case 6 : m_mapCurseKor3.Add(str); break;
				case 8 : m_mapCurseKor4.Add(str); break;
			}
		}
	}

	file.close();
}

//----------------------------------------------------------------------
// Remove Curse
//----------------------------------------------------------------------
bool				
MChatManager::RemoveCurse(char* str, bool bForce) const
{
	#ifdef __GAME_CLIENT__
		if (!g_pUserOption->FilteringCurse && bForce == false || str==NULL)
		{
			return false;
		}
	#else
		if (str==NULL)
		{
			return false;
		}
	#endif


	bool existCurseEng = false;
	bool existCurseKor = false;

	int len = strlen(str);
	int i;
	int index;
	
	//const char* strMask = //"^^; -_-; !_!; o_O; *_*; m_m; u_u; p_q; =_=; -_+; $_$; v_v; Y_Y; o_o; O_O; w_w; #_#; ._.; n_n; &_&; @_@; 0_0; _-_; +_+; +_=; ~_~;";
	//	"#&*%!$#%&*@!%&$&#*&$@#*!%$&@#&*%!$#%&*@!%&$&#*&$@&#!%$*&%*#@#*!%$&@#&*%!$#%&*@!%&$&#*&$@#*!%$&@#&*%!$#%&*@!%&$&#*&$@#*!%$&@";

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	char*	strFiltered = new char [len+1];		

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	int*	indexFiltered = new int [len+1];		

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	//bool*	isCurse = new bool [len+1];
	BYTE*	isCurse = new BYTE [len+1];
	

	//------------------------------------------------------------
	//
	
	//
	//------------------------------------------------------------
	
	
	//------------------------------------------------------------
	// hi, hello! f.u.c.k!~~!
	// --> hihellofuck
	
	// 
	
	//------------------------------------------------------------
	char*	strFilteredPtr = strFiltered;
	int*	indexFilteredPtr = indexFiltered;
	BYTE*	isCursePtr = isCurse;
	
	char*	strOrg = str;	
	
	char	ch;
	const char toLower = 'a'-'A';

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	i = 0;
	index = 0;
	while (ch = *strOrg++)//, ch != '\0')
	{
		//----------------------------------------------
		
		//----------------------------------------------
		if (ch >= 'a' && ch <= 'z')
		{
			*strFilteredPtr++	= ch;
			*indexFilteredPtr++ = i;
			*isCursePtr++		= false;	
			index++;
		}		
		//----------------------------------------------
		
		//----------------------------------------------
		else if (ch >= 'A' && ch <= 'Z')
		{
			*strFilteredPtr++	= ch + toLower;
			*indexFilteredPtr++ = i;
			*isCursePtr++		= false;	
			index++;
		}
		
		//----------------------------------------------
		
		//----------------------------------------------

		i++;
	}
	*strFilteredPtr = '\0';
	
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (index!=0)
	{
		MStringMap::const_iterator iString = m_mapCurseEng.begin();
		
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		while (iString != m_mapCurseEng.end())
		{
			const MString* pString = iString->second;

			if (pString!=NULL)
			{
				strFilteredPtr = strFiltered;

				char* pFind = NULL;

				//---------------------------------------------------
				
				//---------------------------------------------------
				while (pFind = strstr( strFilteredPtr, pString->GetString() ))
				{					
					int lenCurse = pString->GetLength();

					//---------------------------------------------------
					
					//---------------------------------------------------
					
					//memset( isCurse+(pFind-strFilteredPtr), true, lenCurse);
					memset( isCurse+(pFind-strFilteredPtr), lenCurse, lenCurse);
					
					
					//---------------------------------------------------
					
					//---------------------------------------------------
					strFilteredPtr = pFind + lenCurse;

					existCurseEng = true;	
				}
			}

			iString++;
		}

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		for (int i=0; i<index; i++)
		{
			if ( isCurse[i] )
			{
				
				str[ indexFiltered[i] ] = s_MaskString[ i ];
			}
		}

	}

	
	//------------------------------------------------------------
	
	
	
	//------------------------------------------------------------
	
	
	
	//
	
	
	
	
	//
	
	//
	
	//
	//------------------------------------------------------------
	strFilteredPtr = strFiltered;
	indexFilteredPtr = indexFiltered;
	isCursePtr = isCurse;
	
	strOrg = str;	
	
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	i = 0;
	index = 0;
	while (ch = *strOrg++)//, ch != '\0')
	{
		//----------------------------------------------
		
		//----------------------------------------------
		if (ch & 0x80)
		{
			char chNext = *strOrg++;

			//----------------------------------------------
			
			//----------------------------------------------
			if (chNext=='\0')
			{
				
				break;
			}
			
			//----------------------------------------------
			
			//----------------------------------------------
			*strFilteredPtr++	= ch;
			*indexFilteredPtr++ = i++;
			*isCursePtr++		= false;	

			*strFilteredPtr++	= chNext;
			*indexFilteredPtr++ = i;
			*isCursePtr++		= false;	

			index+=2;
		}		
	
		//----------------------------------------------
		
		//----------------------------------------------

		i++;
	}
	*strFilteredPtr = '\0';

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (RemoveCurseKorean(strFiltered, 2, m_mapCurseKor1, isCurse))
	{
		existCurseKor = true;	
	}

	if (RemoveCurseKorean(strFiltered, 4, m_mapCurseKor2, isCurse))
	{
		existCurseKor = true;	
	}

	if (RemoveCurseKorean(strFiltered, 6, m_mapCurseKor3, isCurse))
	{
		existCurseKor = true;	
	}

	if (RemoveCurseKorean(strFiltered, 8, m_mapCurseKor4, isCurse))
	{
		existCurseKor = true;	
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (existCurseKor)
	{
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		
		for (int i=0; i<index; i++)
		{
			if ( isCurse[i] )
			{
				
				
#ifdef __GAME_CLIENT__
				int j = 0;
				switch(isCurse[i])
				{
				case 2:
					{
						char* pChangeString = (*g_pGameStringTable)[UI_STRING_MESSAGE_REMOVE_CURSE_1].GetString();
						for(j = 0; j<(*g_pGameStringTable)[UI_STRING_MESSAGE_REMOVE_CURSE_1].GetLength() ; j++)
						{
							if( (i+j) < len && indexFiltered[i+j] < len && indexFiltered[i+j]>-1)
								str[ indexFiltered[i+j] ] = pChangeString[j];
						}
					}
					i+= 1;
					break;
				case 4:
					{
						char* pChangeString = (*g_pGameStringTable)[UI_STRING_MESSAGE_REMOVE_CURSE_2].GetString();
						for(j = 0; j<(*g_pGameStringTable)[UI_STRING_MESSAGE_REMOVE_CURSE_2].GetLength() ; j++)
						{
							if( (i+j) < len && indexFiltered[i+j] < len && indexFiltered[i+j]>-1)
								str[ indexFiltered[i+j] ] = pChangeString[j];
						}
					}
					i+= 3;
					break;
				case 6:
					{
						char* pChangeString = (*g_pGameStringTable)[UI_STRING_MESSAGE_REMOVE_CURSE_3].GetString();
						for(j = 0; j<(*g_pGameStringTable)[UI_STRING_MESSAGE_REMOVE_CURSE_3].GetLength() ; j++)
						{
							if( (i+j) < len && indexFiltered[i+j] < len && indexFiltered[i+j]>-1)
								str[ indexFiltered[i+j] ] = pChangeString[j];
						}
					}
					i+= 5;
					break;
				case 8:
					{
						char* pChangeString = (*g_pGameStringTable)[UI_STRING_MESSAGE_REMOVE_CURSE_4].GetString();
						for(j = 0; j<(*g_pGameStringTable)[UI_STRING_MESSAGE_REMOVE_CURSE_4].GetLength() ; j++)
						{
							if( (i+j) < len && indexFiltered[i+j] < len && indexFiltered[i+j]>-1)
								str[ indexFiltered[i+j] ] = pChangeString[j];
						}
					}
					i+= 7;
					break;
				default:
					str[ indexFiltered[i] ] = s_MaskString[ i ];
					break;
				}
#else
				
				str[ indexFiltered[i] ] = s_MaskString[ i ];
#endif

				
			}
		}
	}

	delete [] strFiltered;
	delete [] indexFiltered;
	delete [] isCurse;	

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (existCurseEng || existCurseKor)
	{
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// RemoveCurseKorean
//----------------------------------------------------------------------




//----------------------------------------------------------------------




//----------------------------------------------------------------------
bool				
MChatManager::RemoveCurseKorean(const char* strKor, 
								int byteCurse, const MStringMap& mapCurse, 
								BYTE* isCurse) const
{
	int len = strlen(strKor);

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (len < byteCurse)
	{
		return false;
	}

	bool existCurse = false;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	char* strCheck = new char [len+1];
	strcpy( strCheck, strKor );
	
	char* strCheckPtr = strCheck;
	
	int maxCheck = len - (byteCurse-2);

	for (int i=0; i<maxCheck; i+=2)
	{
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		char* strCheckNull = strCheckPtr + byteCurse;
		char previousNull = *strCheckNull;
		*strCheckNull = '\0';

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		MString tempStr(strCheckPtr);
		MStringMap::const_iterator iString = mapCurse.find( &tempStr );

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		if (iString != mapCurse.end())
		{
			
			
			//memset( isCurse+(strCheckPtr-strCheck), true, byteCurse);
			memset( isCurse+(strCheckPtr-strCheck), byteCurse, byteCurse);
			

			existCurse = true;
		}		
	
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		*strCheckNull = previousNull;

		strCheckPtr += 2;
	}

	delete [] strCheck;

	return existCurse;
}

//----------------------------------------------------------------------
// Add Mask
//----------------------------------------------------------------------




//----------------------------------------------------------------------
void
MChatManager::AddMask(char* str, int percent) const
{
#ifdef OUTPUT_DEBUG
	percent = max( percent, 75 );
#endif

	if (percent >= 100)
	{
		return;
	}

	char ch;

	// 0 ~ 100 --> 0 ~ 63
	int pro = percent * 63 / 100;
	
	int index = rand() & 0x0F;

	while (ch = *str, ch != NULL)
	{
		int maskLen = 0;

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		if (ch==' ')
		{			
		}
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		else if (ch & 0x80)
		{
			maskLen = 2;			
		}
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		else 
		{
			maskLen = 1;
		}		

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		if (maskLen!=0)
		{
			int bMask = (rand() & 0x3F) >= pro;	

			if (bMask)
			{
				for (int i=0; i<maskLen; i++)
				{
					if (*str != NULL)
					{			
						*str = s_MaskString2[index++];
						str++;
					}					
				}
			}
			else
			{
				str += maskLen;
			}
		}
		else
		{
			str++;
		}
	}	
}
