//----------------------------------------------------------------------
// ClientConfig.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__CLIENTCONFIG_H__
#define	__CLIENTCONFIG_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include "MString.h"

class ClientConfig {
	public :
		enum NUR_MODE
		{
			NUR_CLIENT,				
			NUR_HOMEPAGE,			
			NUR_MESSAGE_HOMEPAGE,	
			NUR_DENY				
		};

	public :
		ClientConfig();
		~ClientConfig();

		//--------------------------------------------------------
		// File I/O
		//--------------------------------------------------------
		void	SaveToFile(const char* filename);
		bool	LoadFromFile(const char* filename);

	public :
		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		FPS;						

		int		MAX_SCREENSHOT;				
		int		DELAY_PLAYER_DEAD;			
		int		DELAY_PLAYER_RESURRECT_TIMEOUT;			
		int		MAX_CLIENT_MOVE;			
		int		MAX_CREATURE_MOVE_BUFFER;	
		int		MAX_LIGHT_DRAW;				

		int		MAX_SOUNDPART;				

		//--------------------------------------------------------------
		// RequestService
		//--------------------------------------------------------------
		int		MAX_REQUEST_SERVICE;		

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		MAX_WAIT_PACKET;			
		int		MAX_PROCESS_PACKET;			
		
		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		MAX_UPDATE_ONETIME;
		int		MAX_UPDATE_ONETIME_COUNT;	

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		DELAY_GLOBAL_SAY;		

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		MUSIC_THEME;

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		DELAY_SYSTEMMESSAGE;
		int		DELAY_GAMEMESSAGE;

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		MAX_DRAWITEMNAME;

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		MAX_CHATSTRING;
		int		MAX_CHATSTRING_MINUS_1;
		int		MAX_CHATSTRING_LENGTH;
		int		MAX_CHATSTRINGLENGTH_PLUS1;

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		DELAY_CHATSTRING_KEEP;		
		int		DELAY_CHATSTRING_FADE;		
		int		VALUE_CHATSTRING_FADE;		
		int		MIN_CHATSTRING_COLOR256;	

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		int		MAX_TEXTUREPART_EFFECT;
		int		MAX_TEXTUREPART_SCREENEFFECT;
		int		MAX_TEXTUREPART_CREATURESHADOW;
		int		MAX_TEXTUREPART_IMAGEOBJECTSHADOW;
		int		MAX_TEXTUREPART_ADDON_SHADOW;
		int		MAX_TEXTUREPART_IMAGEOBJECTSPK;
		int		MAX_TEXTUREPART_EFFECTSHADOW;

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		int		FONT_HEIGHT;
		int		FONT_ITEM_HEIGHT;

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		int		MAX_HP_BAR_PIXEL;			
		int		POSITION_HP_BAR;		

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		PERCENTAGE_ITEM_SOMEWHAT_BROKEN;
		int		PERCENTAGE_ITEM_ALMOST_BROKEN;

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		int		FRAME_DRAW_ORIGINAL_SPRITE;

		//--------------------------------------------------------
		
		//--------------------------------------------------------			
		WORD	COLOR_OUTLINE_INTERACTIONOBJECT;

		// item
		COLORREF	COLOR_NAME_ITEM;
		COLORREF	COLOR_NAME_ITEM_OPTION;
		COLORREF	COLOR_NAME_ITEM_RARE_OPTION;
		WORD		COLOR_OUTLINE_ITEM;

		
		COLORREF	COLOR_NAME_VAMPIRE;
		COLORREF	COLOR_NAME_SLAYER;
		COLORREF	COLOR_NAME_NPC;

		
		WORD		COLOR_OUTLINE_NPC;
		WORD		COLOR_OUTLINE_ATTACK_POSSIBLE;
		WORD		COLOR_OUTLINE_ATTACK_IMPOSSIBLE;

		
		BYTE		COLOR_HP_BAR_R;
		BYTE		COLOR_HP_BAR_G;
		BYTE		COLOR_HP_BAR_B;

		
		BYTE		COLOR_HP_BAR_BG_R;
		BYTE		COLOR_HP_BAR_BG_G;
		BYTE		COLOR_HP_BAR_BG_B;

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		NUR_MODE	NEW_USER_REGISTERATION_MODE;
		MString		URL_HOMEPAGE;				
		MString		URL_HOMEPAGE_NEW_USER;		
		MString		URL_HOMEPAGE_BILING;

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		COLORREF	COLOR_NAME_GOOD_MORE;
		COLORREF	COLOR_NAME_GOOD;
		COLORREF	COLOR_NAME_NEUTRAL;
		COLORREF	COLOR_NAME_EVIL;
		COLORREF	COLOR_NAME_EVIL_MORE;

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		int			BLOOD_DROP_HP_PERCENT;		
		DWORD		BLOOD_DROP_GAP_TIME;		
		DWORD		BLOOD_DROP_RANDOM_TIME;		

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		DWORD		AFTER_TRADE_ITEM_DROP_DELAY;	
		DWORD		AFTER_PARTY_KICK_DELAY;			
		DWORD		TRACE_CHARACTER_LIMIT_TIME;		

		//--------------------------------------------------------
		// Repeat & Lock
		//--------------------------------------------------------
		DWORD		REPEAT_TIME;				
		DWORD		LOCK_TIME;					

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		DWORD		CLIENT_COMMUNICATION_UDP_PORT;

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		DWORD		CLIENT_COMMUNICATION_STATUS_DELAY;

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		DWORD		TRADE_ACCEPT_DELAY_TIME;

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		BYTE		REGEN_AMOUNT_BURROW;
		BYTE		REGEN_AMOUNT_CASKET;
		BYTE		REGEN_AMOUNT_VAMPIRE;

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		BYTE		MAX_SOUND_PER_SECOND;

		DWORD		UniqueItemColorSet;
		DWORD		QuestItemColorSet;

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		BYTE		HPModifyListMax;
		DWORD		HPModifyListTime;
};


extern	ClientConfig*	g_pClientConfig;

#endif

