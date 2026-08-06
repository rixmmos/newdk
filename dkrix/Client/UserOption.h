//----------------------------------------------------------------------
// UserOption.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__USEROPTION_H__
#define	__USEROPTION_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif

class UserOption {
	public :
		UserOption();
		~UserOption();

		//--------------------------------------------------------
		// File I/O
		//--------------------------------------------------------
		void	SaveToFile(const char* filename);
		bool	LoadFromFile(const char* filename);

	public :
		BOOL	Chinese;
		BOOL	Korean;
		BOOL	Japanese;
		BOOL	English;
		BOOL	Use3DHAL;					
		BOOL	UseSmoothCursor;			
		BOOL	DrawMinimap;				
		BOOL	DrawZoneName;				
		BOOL	DrawGameTime;				
		BOOL	DrawInterface;				
		BOOL	DrawFPS;					
		BOOL	BlendingShadow;				
		BOOL	FilteringCurse;				
		BOOL	PlayMusic;					
		BOOL	PlaySound;					
		int		VolumeMusic;				
		int		VolumeSound;				
		BOOL	UseHelpEvent;				
		BOOL	PlayWaveMusic;				
		BOOL	BloodDrop;					
		BOOL	OpenQuickSlot;				
		BOOL	UseHalfFrame;				
		BOOL	DrawTransHPBar;				
		BOOL	UseForceFeel;				
		BOOL	UseGammaControl;			
		BOOL	GammaValue;					
		BOOL	DrawChatBoxOutline;			
		
		// new interface
		char	BackupID[11];
		BOOL	UseEnterChat;
		BOOL	UseMouseSpeed;
		int		MouseSpeedValue;
		BOOL	PlayYellSound;
		BOOL	ShowChoboHelp;
		BOOL	TribeChange;
		BOOL	DenyPartyInvite;
		BOOL	DenyPartyRequest;
		BOOL	AutoHideSmoothScroll;		
		COLORREF ChattingColor;
		BYTE	ALPHA_DEPTH;
		BOOL	DefaultAlpha;
		BOOL	IsPreLoadMonster;
		BOOL	ChatWhite;					
		BOOL	UseTeenVersion;				
		BOOL	PopupChatByWhisper;			
		BOOL	NotSendMyInfo;				
		BOOL	DoNotShowWarMsg;			
		BOOL	DoNotShowLairMsg;			
		BOOL	DoNotShowHolyLandMsg;		
		int		persnalShopupdatetime;
		
		BOOL	ShowGameMoneyWithHANGUL;
		// 2004, 12, 21, sobeit add end
		
		BOOL	DoNotShowPersnalShopMsg;
		// 2005, 1, 20, sobeit add end
};


extern	UserOption*		g_pUserOption;

#endif

