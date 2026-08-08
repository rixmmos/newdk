// VS_UI_DESC.cpp: implementation of the C_VS_UI_DESC class.
//
//////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include <assert.h>
#define assert(e) ((void)(e))
// Disabled assert for macOS
#include "VS_UI_ExtraDialog.h"

#include "VS_UI_filepath.h"
#include <algorithm>
extern RECT g_GameRect;
#define dSTRING_LEN 2048 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C_VS_UI_DESC::C_VS_UI_DESC()
{
	m_desc_col = 0;
	m_desc_row = 0;
	m_desc_scroll = 0;
	m_desc_y_distance = 18;
	fontx  = 6;
	m_color = BLACK;
	m_pi = gpC_base->m_dialog_msg_pi;
	m_desc_size = 0;
	m_delimiter_pack = 0;
	m_delimiter_sprite = 0;
}

C_VS_UI_DESC::~C_VS_UI_DESC()
{
	if(!m_pC_inpicture.empty())
	{		
		
		std::vector<CSpritePack *>::iterator iPic = m_pC_inpicture.begin();
		while (iPic != m_pC_inpicture.end())
		{
			delete *iPic;

			iPic++;
		}

		m_pC_inpicture.clear();		
		
//		DeleteNew(m_pC_inpicture);
	}

}

//-----------------------------------------------------------------------------
// C_VS_UI_DESC::ShowDesc
//
//
//-----------------------------------------------------------------------------
void C_VS_UI_DESC::ShowDesc(int x, int y)
{
	int i;
	int delimeter_x;
	if(m_pC_inpicture.empty() || m_Sprite.empty())
		delimeter_x = 0;
	else
	{

		if(m_delimiter_pack < m_pC_inpicture.size())
			delimeter_x = (*m_pC_inpicture[m_delimiter_pack])[m_delimiter_sprite].GetWidth() + PICTURE_INDENT;
		
		for(i = 0; i < m_Sprite.size(); i++)
		{
			if(m_Sprite[i].pack_num < m_pC_inpicture.size())
			{
				Rect rect(0, 0, (*m_pC_inpicture[m_Sprite[i].pack_num])[m_Sprite[i].sprite_num].GetWidth(), (*m_pC_inpicture[m_Sprite[i].pack_num])[m_Sprite[i].sprite_num].GetHeight());
				int blt_y = (m_Sprite[i].pos - m_desc_scroll)*m_desc_y_distance;

				
				if(m_Sprite[i].pos == -300)
				{
					blt_y = (0 - m_desc_scroll)*m_desc_y_distance;
				}
				
				if(blt_y < 0)
				{
					if(blt_y + rect.h > 0)
					{
						rect.y -=blt_y;
						rect.h += blt_y;
					}
					else
					{
						rect.h = 0;
					}
				}
				
				if(blt_y + rect.h > m_desc_col*m_desc_y_distance)
				{
					if(blt_y < m_desc_col*m_desc_y_distance)
					{
						rect.h = m_desc_col*m_desc_y_distance - blt_y;
					}
					else
					{
						rect.h = 0;
					}
				}
				if (gpC_base->m_p_DDSurface_back->Lock())
				{
					S_SURFACEINFO surface_info;
#ifdef PLATFORM_WINDOWS
					SetSurfaceInfo(&surface_info, gpC_base->m_p_DDSurface_back->GetDDSD());
#else
					// SDL backend: Get surface info differently
					gpC_base->m_p_DDSurface_back->GetSurfaceInfo(&surface_info);
#endif

					RECT rt;
					rt.left = max(-(m_desc_x+x), rect.x);
					rt.top = max(-(m_desc_y + blt_y+y), rect.y);
					rt.right = min((int)(rect.x+rect.w), (int)g_GameRect.right-m_desc_x);
					rt.bottom = min((int)(rect.y+rect.h), (int)g_GameRect.bottom-(m_desc_y + blt_y));
					
					if(rt.left < rt.right && rt.top < rt.bottom)
					{
						WORD * p_dest = (WORD *)surface_info.p_surface+m_desc_x+x+rt.left;
						p_dest = (WORD *)((BYTE *)p_dest+((m_desc_y + blt_y+y)+rt.top)*surface_info.pitch);
						
						if(m_Sprite[i].pos == -300)
						{
							p_dest += 200;
						}
						
						(*m_pC_inpicture[m_Sprite[i].pack_num])[m_Sprite[i].sprite_num].BltClipWidth(p_dest, surface_info.pitch, &rt);
					}
					gpC_base->m_p_DDSurface_back->Unlock();
				}
			}
		}
	}

	g_FL2_GetDC();

	// title
	if(!m_desc_title.empty())
	{
		 
			g_PrintColorStr(m_desc_title_x+x, m_desc_title_y+y, m_desc_title.c_str(), m_title_pi, m_title_color);
	}

	
	char *p_temp;
	for(i=m_desc_scroll; i<m_desc_col+m_desc_scroll && i<m_desc.size();i++)
	{
		p_temp = (char *)m_desc[i].c_str();
		if(p_temp[0] == '\t')
			g_PrintColorStr(m_desc_x+x + delimeter_x, m_desc_y+y + m_desc_y_distance*(i - m_desc_scroll), p_temp+1, m_pi, m_color);
		else
			g_PrintColorStr(m_desc_x+x, m_desc_y+y + m_desc_y_distance*(i - m_desc_scroll), p_temp, m_pi, m_color);
	}

	g_FL2_ReleaseDC();
}

////////////////////////////////////////////////////////////////////////
// C_VS_UI_DESC::LoadDesc

////////////////////////////////////////////////////////////////////////
bool	C_VS_UI_DESC::LoadDesc(const char *szFilename, int row, int col, bool bl_title, int CoreZapID)
{
	if(!m_pack_file.IsSet())return false;

	m_desc_scroll = 0;
	m_desc.clear();
	assert(szFilename);

	m_desc_row = row; m_desc_col = col;
	m_Sprite.clear();

	if(!m_pack_file.Open(szFilename))
		return false;

	bool indent = false;
	

	int check, w=0, h=0, w2=0, pack = 0;

	if(m_ori_string.empty() && !m_rep_string.empty())
	{
		for(int i = 0; i < (int)m_rep_string.size(); i++)
		{
			if(m_rep_string[i][0] == '%')
			{
				SetSprite(0, atoi(m_rep_string[i].c_str()+1), 0);
				w2 = ((*m_pC_inpicture[m_Sprite[0].pack_num])[m_Sprite[0].sprite_num].GetWidth() + PICTURE_INDENT -1)/ fontx-1;
				h =  ((*m_pC_inpicture[m_Sprite[0].pack_num])[m_Sprite[0].sprite_num].GetHeight()-1)/ m_desc_y_distance +1;
				m_rep_string.erase(m_rep_string.begin() + i);
				i--;
			}
		}

		m_desc.insert(m_desc.begin(), m_rep_string.begin(), m_rep_string.end());
		indent = true;
	}

	char szLine[dSTRING_LEN]; 
	char temp[dSTRING_LEN];
	ZeroMemory(szLine, dSTRING_LEN);

	if(!m_Sprite.empty())
	{
	}

	while(m_pack_file.GetString(szLine, dSTRING_LEN))
	{
		std::string temp_string;

		temp_string = szLine;

		bool string_replaced = false;		for(int i = 0; i < m_ori_string.size(); i++)
		{
			int re = temp_string.find(m_ori_string[i]);
			if(re != -1)
			{
				if(m_rep_string[i] == "%delete")break;
				int re = m_rep_string[i].find("\n");
				if(re != -1)
				{
					char sz_temp_1[512], sz_temp_2[512];
					strcpy(sz_temp_1, m_rep_string[i].c_str());
					strcpy(sz_temp_2, sz_temp_1+re+1);
					sz_temp_1[re] = '\0';
					m_desc.push_back(sz_temp_1);
					m_desc.push_back(sz_temp_2);
					string_replaced = true;
					break;
				}
				else
					temp_string.replace(temp_string.begin()+re, temp_string.begin() + re + m_ori_string[i].size(), m_rep_string[i].begin(), m_rep_string[i].end());
			}
		}
		if(string_replaced) continue;

		if(temp_string[0] == '&')
		{
			pack = atoi(temp_string.c_str()+1);
			continue;
		}

		if(temp_string[0] == '(')
		{
			m_delimiter_pack = atoi(temp_string.c_str()+1);
			continue;
		}

		if(temp_string[0] == ')')
		{
			m_delimiter_sprite = atoi(temp_string.c_str()+1);
			continue;
		}

		if(temp_string[0] == '%')
		{
			int spr_id = atoi(temp_string.c_str()+1);

			if(m_ori_string.empty() && !m_rep_string.empty())
			{
				
				int HasCoreZap = (CoreZapID==-1)?0:1;

				CSpritePack *temp = new CSpritePack;
				temp->Init( 1 + HasCoreZap );		
				
				bool re = temp->LoadFromFileData( 0, spr_id, SPK_ITEM, SPKI_ITEM );
				assert(re);

				if(HasCoreZap)
				{
					bool re = temp->LoadFromFileData( 1, CoreZapID, SPK_ITEM, SPKI_ITEM );
					assert(re);
				}
			
				m_pC_inpicture.push_back(temp);

				
				if(spr_id == 371)
					SetSprite(m_pC_inpicture.size()-1, 0, -300);
				else
					SetSprite(m_pC_inpicture.size()-1, 0, 0);
				
				if(HasCoreZap)
					SetSprite(m_pC_inpicture.size()-1, 1, 0);
				// 2004, 7, 6 sobeit modify end 
				
		

				w2 = 0;
				h = ((*m_pC_inpicture[m_pC_inpicture.size()-1])[0].GetHeight() -1)/ m_desc_y_distance +1 - m_rep_string.size();
			}
			else
			{
				while(h > 0)	
				{
					h--;
					m_desc.push_back("");
				};

				int pos = m_desc.size();	
				if(bl_title)
				{
					int offset = 1;
					for(int i = 1; i < (int)m_desc.size() && !strcmp(m_desc[i].c_str(),""); i++, offset++);
					pos -= offset;
				}

				SetSprite(pack, spr_id, pos);
				if(pack < m_pC_inpicture.size())
				{
					w2 = ((*m_pC_inpicture[pack])[spr_id].GetWidth() + PICTURE_INDENT -1)/ fontx-1;
					h = ((*m_pC_inpicture[pack])[spr_id].GetHeight() -1)/ m_desc_y_distance +1;
				}
			}
			continue;
		}

		strcpy(szLine, temp_string.c_str());

		if(szLine[0] == '\t')
		{
			w2 = 0;
			m_desc.push_back(szLine);
			indent = true;
		}
		else
		{
			if(h > 0 && w2 == 0)	
			{
				while(h>0)
				{
					h--;
					m_desc.push_back("");
				}
			}
			else if(indent)		
			{
				m_desc.push_back("");
				indent = false;
			}

			int w3 = strchr(szLine, ':') - szLine + 2;
			if(w3 < 0)w3 = 0;
			if(w3 > row/2)w3 = 0;
//			else w3 = 8;
			bool loop = false;

			while(1)
			{

				if(!g_PossibleStringCut(szLine, row -w2 -(loop?w3:0)))check = 1; else check=0;
				
				strcpy(temp, szLine);

				if(h > 0 || w3 && loop)
				{
					memset(szLine, (int)' ', dSTRING_LEN);
					strcpy(szLine+w2+(loop?w3:0), temp);
					szLine[row -check] = '\0';
				}
				else
					szLine[row -w2 -(loop?w3:0) -check] = '\0';

				m_desc.push_back(szLine);

				if(strlen(temp) <= row -check -w2 - (loop?w3:0))
				{
					if(h > 0)
						if(--h == 0)w2 = 0;
					break;
				}
				if(temp[row -check -w2 - (loop?w3:0)] == ' ')strcpy(szLine, temp+row -check -w2 -(loop?w3:0) +1);
				else strcpy(szLine, temp+row -check -w2 -(loop?w3:0));

				loop = true;

				if(h > 0)
					if(--h == 0)w2 = 0;
			}
		}

		ZeroMemory(szLine, dSTRING_LEN);
		
	}
	while(h > 0)
	{
		h--;
		m_desc.push_back(" ");
	}

	m_pack_file.Release();

	if(bl_title)	
	{
		m_desc_title = m_desc[0];
		m_desc.erase(m_desc.begin());
	}

//	if(m_desc.size())###@@@
	if(!m_desc.empty())
	{
		while(!m_desc.empty() && m_desc.front() == "")
			m_desc.erase(m_desc.begin());
		while(!m_desc.empty() && m_desc.back() == "")
			m_desc.pop_back();
	}

	return TRUE;

}

////////////////////////////////////////////////////////////////////////
// C_VS_UI_DESC::SetSprite

////////////////////////////////////////////////////////////////////////
void	C_VS_UI_DESC::SetSprite(int pack, int num, int line)				
{
	DESC_SPRITE temp;
	temp.pack_num = pack;
	temp.sprite_num = num;
	temp.pos = line;
	m_Sprite.push_back(temp);
}


////////////////////////////////////////////////////////////////////////
// C_VS_UI_DESC::LoadDesc

////////////////////////////////////////////////////////////////////////
bool	C_VS_UI_DESC::LoadDescFromString(const char *szString, int row, int col, bool bl_title, int CoreZapID)
{
	if(szString == NULL)
		return false;
	m_desc_scroll = 0;
	m_desc.clear();

	m_desc_row = row; m_desc_col = col;
	m_Sprite.clear();

	bool indent = false;
	

	int check, w=0, h=0, w2=0, pack = 0;

	if(m_ori_string.empty() && !m_rep_string.empty())
	{
		for(int i = 0; i < (int)m_rep_string.size(); i++)
		{
			if(m_rep_string[i][0] == '%')
			{
				SetSprite(0, atoi(m_rep_string[i].c_str()+1), 0);
				w2 = ((*m_pC_inpicture[m_Sprite[0].pack_num])[m_Sprite[0].sprite_num].GetWidth() + PICTURE_INDENT -1)/ fontx-1;
				h =  ((*m_pC_inpicture[m_Sprite[0].pack_num])[m_Sprite[0].sprite_num].GetHeight()-1)/ m_desc_y_distance +1;
				m_rep_string.erase(m_rep_string.begin() + i);
				i--;
			}
		}

		m_desc.insert(m_desc.begin(), m_rep_string.begin(), m_rep_string.end());
		indent = true;
	}

	char szLine[dSTRING_LEN]; 
	char temp[dSTRING_LEN];
	ZeroMemory(szLine, dSTRING_LEN);

	if(!m_Sprite.empty())
	{
	}

//	while(m_pack_file.GetString(szLine, dSTRING_LEN))
	{
		std::string temp_string;

		temp_string = szString;

		for(int i = 0; i < m_ori_string.size(); i++)
		{
			int re = temp_string.find(m_ori_string[i]);
			if(re != -1)
			{
				if(m_rep_string[i] == "%delete")break;
				int re = m_rep_string[i].find("\n");
				if(re != -1)
				{
					char sz_temp_1[512], sz_temp_2[512];
					strcpy(sz_temp_1, m_rep_string[i].c_str());
					strcpy(sz_temp_2, sz_temp_1+re+1);
					sz_temp_1[re] = '\0';
					m_desc.push_back(sz_temp_1);
					m_desc.push_back(sz_temp_2);
					break;
				}
				else
					temp_string.replace(temp_string.begin()+re, temp_string.begin() + re + m_ori_string[i].size(), m_rep_string[i].begin(), m_rep_string[i].end());
			}
		}
//		if(i != m_ori_string.size())continue;
//

//		{
//			pack = atoi(temp_string.c_str()+1);
//			continue;
//		}
//

//		{
//			m_delimiter_pack = atoi(temp_string.c_str()+1);
//			continue;
//		}
//

//		{
//			m_delimiter_sprite = atoi(temp_string.c_str()+1);
//			continue;
//		}
//

//		{
//			int spr_id = atoi(temp_string.c_str()+1);
//

//			{

//				int HasCoreZap = (CoreZapID==-1)?0:1;
//
//				CSpritePack *temp = new CSpritePack;

//				
//				bool re = temp->LoadFromFileData( 0, spr_id, SPK_ITEM, SPKI_ITEM );
//				assert(re);
//
//				if(HasCoreZap)
//				{
//					bool re = temp->LoadFromFileData( 1, CoreZapID, SPK_ITEM, SPKI_ITEM );
//					assert(re);
//				}
//			
//				m_pC_inpicture.push_back(temp);
//

//				if(spr_id == 371)
//					SetSprite(m_pC_inpicture.size()-1, 0, -300);
//				else
//					SetSprite(m_pC_inpicture.size()-1, 0, 0);
//				
//				if(HasCoreZap)
//					SetSprite(m_pC_inpicture.size()-1, 1, 0);
//				// 2004, 7, 6 sobeit modify end 
//				
//		
//
//				w2 = 0;
//				h = ((*m_pC_inpicture[m_pC_inpicture.size()-1])[0].GetHeight() -1)/ m_desc_y_distance +1 - m_rep_string.size();
//			}

//			{

//				{
//					h--;
//					m_desc.push_back("");
//				};
//

//				if(bl_title)
//				{
//					for(i = 1; !strcmp(m_desc[i].c_str(),""); i++);
//					pos -= i;
//				}
//
//				SetSprite(pack, spr_id, pos);
//				if(pack < m_pC_inpicture.size())
//				{
//					w2 = ((*m_pC_inpicture[pack])[spr_id].GetWidth() + PICTURE_INDENT -1)/ fontx-1;
//					h = ((*m_pC_inpicture[pack])[spr_id].GetHeight() -1)/ m_desc_y_distance +1;
//				}
//			}
//			continue;
//		}

		strcpy(szLine, temp_string.c_str());

		if(szLine[0] == '\t')
		{
			w2 = 0;
			m_desc.push_back(szLine);
			indent = true;
		}
		else
		{
			if(h > 0 && w2 == 0)	
			{
				while(h>0)
				{
					h--;
					m_desc.push_back("");
				}
			}
			else if(indent)		
			{
				m_desc.push_back("");
				indent = false;
			}

			int w3 = strchr(szLine, ':') - szLine + 2;
			if(w3 < 0)w3 = 0;
			if(w3 > row/2)w3 = 0;
//			else w3 = 8;
			bool loop = false;

			while(1)
			{

				if(!g_PossibleStringCut(szLine, row -w2 -(loop?w3:0)))check = 1; else check=0;
				
				strcpy(temp, szLine);

				if(h > 0 || w3 && loop)
				{
					memset(szLine, (int)' ', dSTRING_LEN);
					strcpy(szLine+w2+(loop?w3:0), temp);
					szLine[row -check] = '\0';
				}
				else
					szLine[row -w2 -(loop?w3:0) -check] = '\0';

				m_desc.push_back(szLine);

				if(strlen(temp) <= row -check -w2 - (loop?w3:0))
				{
					if(h > 0)
						if(--h == 0)w2 = 0;
					break;
				}
				if(temp[row -check -w2 - (loop?w3:0)] == ' ')strcpy(szLine, temp+row -check -w2 -(loop?w3:0) +1);
				else strcpy(szLine, temp+row -check -w2 -(loop?w3:0));

				loop = true;

				if(h > 0)
					if(--h == 0)w2 = 0;
			}
		}

		ZeroMemory(szLine, dSTRING_LEN);
		
	}
	while(h > 0)
	{
		h--;
		m_desc.push_back(" ");
	}

	m_pack_file.Release();

	if(bl_title)	
	{
		m_desc_title = m_desc[0];
		m_desc.erase(m_desc.begin());
	}

//	if(m_desc.size())###@@@
	if(!m_desc.empty())
	{
		while(!m_desc.empty() && m_desc.front() == "")
			m_desc.erase(m_desc.begin());
		while(!m_desc.empty() && m_desc.back() == "")
			m_desc.pop_back();
	}

	return TRUE;

}
