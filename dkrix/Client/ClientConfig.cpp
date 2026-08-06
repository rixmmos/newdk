//----------------------------------------------------------------------
// ClientConfig.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "ClientConfig.h"
#include "MusicDef.h"
#include <fstream>
using namespace std;

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
ClientConfig*	g_pClientConfig = NULL;

//----------------------------------------------------------------------
// define functions
//----------------------------------------------------------------------
#define READ_CHECK_EOF( value, temp, bytes )		\
		{											\
			file.read((char*)&temp, bytes);			\
			if (!file.eof())						\
			{										\
				value = temp;						\
			}										\
			else									\
			{										\
				return true;						\
			}										\
		}

//----------------------------------------------------------------------
// 
// constructor
//
//----------------------------------------------------------------------
ClientConfig::ClientConfig()
{	
	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	FPS						= 16;
 
	
	MAX_SCREENSHOT			= 100;

	
	DELAY_PLAYER_DEAD		= 10 * 1000;	

	DELAY_PLAYER_RESURRECT_TIMEOUT = 60 * 1000;		
	
	
	MAX_CLIENT_MOVE			= 6;

	
	MAX_CREATURE_MOVE_BUFFER	= 2;
	
	
	MAX_LIGHT_DRAW			= 50;

	
	MAX_SOUNDPART			= 100;

	MAX_REQUEST_SERVICE		= 10;

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	
	
	//--------------------------------------------------------------
	MAX_UPDATE_ONETIME		= 6;

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	
	
	
	
	//--------------------------------------------------------------
	MAX_UPDATE_ONETIME_COUNT		= 10;		

	DELAY_GLOBAL_SAY				= 12*1000;	

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	DELAY_SYSTEMMESSAGE		= 5000;
	DELAY_GAMEMESSAGE		= 5000;

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	MAX_DRAWITEMNAME			= 20;

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	MAX_WAIT_PACKET			= 15000;	

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	MAX_PROCESS_PACKET			= 11;		

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	MUSIC_THEME				= MUSIC_TREASURE;

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	MAX_CHATSTRING				= 5;
	MAX_CHATSTRING_MINUS_1		= 4;
	MAX_CHATSTRING_LENGTH		= 20;
	MAX_CHATSTRINGLENGTH_PLUS1	= 21;

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	DELAY_CHATSTRING_KEEP		= 1500;		
	DELAY_CHATSTRING_FADE		= 250;		
	VALUE_CHATSTRING_FADE		= 8;		
	MIN_CHATSTRING_COLOR256		= 96;		
	
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	MAX_TEXTUREPART_EFFECT				= 25;
	MAX_TEXTUREPART_CREATURESHADOW		= 100;
	MAX_TEXTUREPART_IMAGEOBJECTSHADOW	= 100;
	MAX_TEXTUREPART_ADDON_SHADOW			= 200;
	MAX_TEXTUREPART_IMAGEOBJECTSPK		= 20;
	MAX_TEXTUREPART_SCREENEFFECT		= 25;

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	FONT_HEIGHT				= 20;
	FONT_ITEM_HEIGHT			= 15;

	//--------------------------------------------------------
	// HP Bar
	//--------------------------------------------------------
	MAX_HP_BAR_PIXEL			= 120;		
	POSITION_HP_BAR			= -10;		

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	PERCENTAGE_ITEM_SOMEWHAT_BROKEN	= 25;		
	PERCENTAGE_ITEM_ALMOST_BROKEN		= 10;		

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	FRAME_DRAW_ORIGINAL_SPRITE			= 8;		

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	COLOR_OUTLINE_INTERACTIONOBJECT	= (0 << 11) | (31 << 6) | 0;	// GREEN

	// item
	COLOR_NAME_ITEM					= RGB(31<<3, 31<<3, 31<<3);	// WHITE
	COLOR_NAME_ITEM_OPTION			= RGB(31<<3, 31<<3, 5<<3);	
	COLOR_NAME_ITEM_RARE_OPTION		= RGB(19<<3, 19<<3, 31<<3);	
	COLOR_OUTLINE_ITEM				= (23 << 11) | (23 << 6) | 23;

	
	COLOR_NAME_VAMPIRE				= RGB(31<<3, 10<<3, 10<<3);	// RED
	COLOR_NAME_SLAYER				= RGB(23<<3, 23<<3, 31<<3);	// BLUE
	COLOR_NAME_NPC					= RGB(5<<3, 27<<3, 5<<3);	// GREEN

	
	COLOR_OUTLINE_NPC				= (5 << 11) | (23 << 6) | 5;
	COLOR_OUTLINE_ATTACK_POSSIBLE	= (23 << 11) | (5 << 6) | 5;
	COLOR_OUTLINE_ATTACK_IMPOSSIBLE	= (5 << 11) | (5 << 6) | 18;

	// hp bar
	COLOR_HP_BAR_R					= 20;	// Red (darker for better visibility)
	COLOR_HP_BAR_G					= 2;	// Small green
	COLOR_HP_BAR_B					= 2;	// Small blue

	// hp bar background (max HP)
	COLOR_HP_BAR_BG_R				= 8;	// Dark red
	COLOR_HP_BAR_BG_G				= 8;	// Dark green
	COLOR_HP_BAR_BG_B				= 8;	// Dark blue (dark gray)

	//---------------------------------------------------------------------
	
	//---------------------------------------------------------------------
	
	
	
	
	NEW_USER_REGISTERATION_MODE	= ClientConfig::NUR_HOMEPAGE;


//	URL_HOMEPAGE_BILING				= "http://member.t2cn.com/billing";
	URL_HOMEPAGE					= "http://www.darkeden.com";				
	URL_HOMEPAGE_NEW_USER			= "http://www.darkeden.com/pg/new_reg.html";		
	URL_HOMEPAGE_BILING				= "http://www.darkeden.com/pg/buy_index.html";

	//---------------------------------------------------------------------
	
	//---------------------------------------------------------------------
	COLOR_NAME_GOOD_MORE		= RGB(50, 50, 200);	
	COLOR_NAME_GOOD				= RGB(25, 210, 220);
	COLOR_NAME_NEUTRAL			= RGB(128, 128, 128);
	COLOR_NAME_EVIL				= RGB(200, 50, 50);
	COLOR_NAME_EVIL_MORE		= RGB(110, 25, 25);

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	BLOOD_DROP_HP_PERCENT		= 30;		
	BLOOD_DROP_GAP_TIME			= 3000;		
	BLOOD_DROP_RANDOM_TIME		= 3000;		

	//--------------------------------------------------------
	//
	//--------------------------------------------------------
	MAX_TEXTUREPART_EFFECTSHADOW	= 3;

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	AFTER_TRADE_ITEM_DROP_DELAY = 3000;	

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	AFTER_PARTY_KICK_DELAY	= 60*60*1000;	

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	CLIENT_COMMUNICATION_UDP_PORT	= 9858;

	CLIENT_COMMUNICATION_STATUS_DELAY = 5000;

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	TRADE_ACCEPT_DELAY_TIME = 5000;

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	REGEN_AMOUNT_BURROW		= 4;
	REGEN_AMOUNT_CASKET		= 10;
	REGEN_AMOUNT_VAMPIRE	= 2;

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	MAX_SOUND_PER_SECOND	= 6;

	REPEAT_TIME				= 10*60*1000;				
	LOCK_TIME				= 10*60*1000;				
	
	
	UniqueItemColorSet		= 405;
	QuestItemColorSet		= 345;

	
	HPModifyListMax			= 10;
	HPModifyListTime		= 10000;
	TRACE_CHARACTER_LIMIT_TIME		= 5*60*1000;		
}

ClientConfig::~ClientConfig()
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

ClientConfig::SaveToFile(const char* filename)
{
	std::ofstream file(filename, std::ios::binary);

	file.write((const char*)&FPS, 4);

	file.write((const char*)&MAX_SCREENSHOT, 4);

	file.write((const char*)&DELAY_PLAYER_DEAD, 4);
	file.write((const char*)&MAX_CLIENT_MOVE, 4);
	file.write((const char*)&MAX_CREATURE_MOVE_BUFFER, 4);
	file.write((const char*)&MAX_LIGHT_DRAW, 4);

	file.write((const char*)&MAX_SOUNDPART, 4);

	file.write((const char*)&MAX_UPDATE_ONETIME, 4);
	file.write((const char*)&MAX_UPDATE_ONETIME_COUNT, 4);

	file.write((const char*)&DELAY_GLOBAL_SAY, 4);	

	file.write((const char*)&DELAY_SYSTEMMESSAGE, 4);	
	file.write((const char*)&DELAY_GAMEMESSAGE, 4);	

	file.write((const char*)&MAX_DRAWITEMNAME, 4);	

	file.write((const char*)&MAX_WAIT_PACKET, 4);
	file.write((const char*)&MAX_PROCESS_PACKET, 4);
	
	file.write((const char*)&MUSIC_THEME, 4);
	
	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	file.write((const char*)&MAX_CHATSTRING, 4);
	file.write((const char*)&MAX_CHATSTRING_MINUS_1, 4);
	file.write((const char*)&MAX_CHATSTRING_LENGTH, 4);
	file.write((const char*)&MAX_CHATSTRINGLENGTH_PLUS1, 4);

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	file.write((const char*)&DELAY_CHATSTRING_KEEP, 4);		
	file.write((const char*)&DELAY_CHATSTRING_FADE, 4);		
	file.write((const char*)&VALUE_CHATSTRING_FADE, 4);		
	file.write((const char*)&MIN_CHATSTRING_COLOR256, 4);		

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	file.write((const char*)&MAX_TEXTUREPART_EFFECT, 4);
	file.write((const char*)&MAX_TEXTUREPART_CREATURESHADOW, 4);
	file.write((const char*)&MAX_TEXTUREPART_IMAGEOBJECTSHADOW, 4);
	file.write((const char*)&MAX_TEXTUREPART_ADDON_SHADOW, 4);
	file.write((const char*)&MAX_TEXTUREPART_IMAGEOBJECTSPK, 4);

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	file.write((const char*)&FONT_HEIGHT, 4);
	file.write((const char*)&FONT_ITEM_HEIGHT, 4);

	//--------------------------------------------------------
	// HP Bar
	//--------------------------------------------------------
	file.write((const char*)&MAX_HP_BAR_PIXEL, 4);			
	file.write((const char*)&POSITION_HP_BAR, 4);		

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	file.write((const char*)&PERCENTAGE_ITEM_SOMEWHAT_BROKEN, 4);
	file.write((const char*)&PERCENTAGE_ITEM_ALMOST_BROKEN, 4);

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	file.write((const char*)&FRAME_DRAW_ORIGINAL_SPRITE, 4);
		
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	file.write((const char*)&COLOR_OUTLINE_INTERACTIONOBJECT, 2);

	// item
	file.write((const char*)&COLOR_NAME_ITEM, 4);
	file.write((const char*)&COLOR_NAME_ITEM_OPTION, 4);
	file.write((const char*)&COLOR_NAME_ITEM_RARE_OPTION, 4);
	file.write((const char*)&COLOR_OUTLINE_ITEM, 2);

	
	file.write((const char*)&COLOR_NAME_VAMPIRE, 4);
	file.write((const char*)&COLOR_NAME_SLAYER, 4);
	file.write((const char*)&COLOR_NAME_NPC, 4);

	
	file.write((const char*)&COLOR_OUTLINE_NPC, 2);
	file.write((const char*)&COLOR_OUTLINE_ATTACK_POSSIBLE, 2);
	file.write((const char*)&COLOR_OUTLINE_ATTACK_IMPOSSIBLE, 2);

	
	file.write((const char*)&COLOR_HP_BAR_R, 1);
	file.write((const char*)&COLOR_HP_BAR_G, 1);
	file.write((const char*)&COLOR_HP_BAR_B, 1);
	file.write((const char*)&COLOR_HP_BAR_BG_R, 1);
	file.write((const char*)&COLOR_HP_BAR_BG_G, 1);
	file.write((const char*)&COLOR_HP_BAR_BG_B, 1);

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	file.write((const char*)&NEW_USER_REGISTERATION_MODE, sizeof(NUR_MODE));
	URL_HOMEPAGE.SaveToFile( file );				
	URL_HOMEPAGE_NEW_USER.SaveToFile( file );		
	URL_HOMEPAGE_BILING.SaveToFile( file );

	file.write((const char*)&COLOR_NAME_GOOD_MORE, 4);
	file.write((const char*)&COLOR_NAME_GOOD, 4);
	file.write((const char*)&COLOR_NAME_NEUTRAL, 4);
	file.write((const char*)&COLOR_NAME_EVIL, 4);
	file.write((const char*)&COLOR_NAME_EVIL_MORE, 4);	

	file.write((const char*)&BLOOD_DROP_HP_PERCENT, 4);
	file.write((const char*)&BLOOD_DROP_GAP_TIME, 4);
	file.write((const char*)&BLOOD_DROP_RANDOM_TIME, 4);	
	file.write((const char*)&MAX_TEXTUREPART_EFFECTSHADOW, 4);
	file.write((const char*)&MAX_TEXTUREPART_SCREENEFFECT, 4);	

	file.write((const char*)&MAX_REQUEST_SERVICE, 4);

	file.write((const char*)&AFTER_TRADE_ITEM_DROP_DELAY, 4);

	file.write((const char*)&DELAY_PLAYER_RESURRECT_TIMEOUT, 4);
	
	file.write((const char*)&AFTER_PARTY_KICK_DELAY, 4);

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	file.write((const char*)&CLIENT_COMMUNICATION_UDP_PORT, 4);
	file.write((const char*)&CLIENT_COMMUNICATION_STATUS_DELAY, 4);

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	file.write((const char*)&TRADE_ACCEPT_DELAY_TIME, 4);

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	file.write((const char*)&REGEN_AMOUNT_BURROW, 1);
	file.write((const char*)&REGEN_AMOUNT_CASKET, 1);
	file.write((const char*)&REGEN_AMOUNT_VAMPIRE, 1);	

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	file.write((const char*)&MAX_SOUND_PER_SECOND, 1);	

	
	file.write((const char*)&REPEAT_TIME, 4);
	file.write((const char*)&LOCK_TIME, 4);

	
	file.write((const char*)&UniqueItemColorSet, 4);
	file.write((const char*)&QuestItemColorSet, 4);

	
	file.write((const char*)&HPModifyListMax, 1);
	file.write((const char*)&HPModifyListTime, 4);
	file.write((const char*)&TRACE_CHARACTER_LIMIT_TIME, 4);
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
bool	
ClientConfig::LoadFromFile(const char* filename)
{
	std::ifstream file(filename, std::ios::binary);	

	if (!file || !file.is_open())
	{
		return false;
	}
	
	file.read((char*)&FPS, 4);

	file.read((char*)&MAX_SCREENSHOT, 4);

	file.read((char*)&DELAY_PLAYER_DEAD, 4);
	file.read((char*)&MAX_CLIENT_MOVE, 4);
	file.read((char*)&MAX_CREATURE_MOVE_BUFFER, 4);
	file.read((char*)&MAX_LIGHT_DRAW, 4);

	file.read((char*)&MAX_SOUNDPART, 4);

	file.read((char*)&MAX_UPDATE_ONETIME, 4);
	file.read((char*)&MAX_UPDATE_ONETIME_COUNT, 4);

	file.read((char*)&DELAY_GLOBAL_SAY, 4);	

	file.read((char*)&DELAY_SYSTEMMESSAGE, 4);	
	file.read((char*)&DELAY_GAMEMESSAGE, 4);	

	file.read((char*)&MAX_DRAWITEMNAME, 4);

	file.read((char*)&MAX_WAIT_PACKET, 4);
	file.read((char*)&MAX_PROCESS_PACKET, 4);

	file.read((char*)&MUSIC_THEME, 4);

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	file.read((char*)&MAX_CHATSTRING, 4);
	file.read((char*)&MAX_CHATSTRING_MINUS_1, 4);
	file.read((char*)&MAX_CHATSTRING_LENGTH, 4);
	file.read((char*)&MAX_CHATSTRINGLENGTH_PLUS1, 4);

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	file.read((char*)&DELAY_CHATSTRING_KEEP, 4);		
	file.read((char*)&DELAY_CHATSTRING_FADE, 4);		
	file.read((char*)&VALUE_CHATSTRING_FADE, 4);		
	file.read((char*)&MIN_CHATSTRING_COLOR256, 4);		

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	file.read((char*)&MAX_TEXTUREPART_EFFECT, 4);
	file.read((char*)&MAX_TEXTUREPART_CREATURESHADOW, 4);
	file.read((char*)&MAX_TEXTUREPART_IMAGEOBJECTSHADOW, 4);
	file.read((char*)&MAX_TEXTUREPART_ADDON_SHADOW, 4);
	file.read((char*)&MAX_TEXTUREPART_IMAGEOBJECTSPK, 4);

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	file.read((char*)&FONT_HEIGHT, 4);
	file.read((char*)&FONT_ITEM_HEIGHT, 4);

	//--------------------------------------------------------
	// HP Bar
	//--------------------------------------------------------
	file.read((char*)&MAX_HP_BAR_PIXEL, 4);			
	file.read((char*)&POSITION_HP_BAR, 4);		

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	file.read((char*)&PERCENTAGE_ITEM_SOMEWHAT_BROKEN, 4);
	file.read((char*)&PERCENTAGE_ITEM_ALMOST_BROKEN, 4);

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	file.read((char*)&FRAME_DRAW_ORIGINAL_SPRITE, 4);

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	file.read((char*)&COLOR_OUTLINE_INTERACTIONOBJECT, 2);

	// item
	file.read((char*)&COLOR_NAME_ITEM, 4);
	file.read((char*)&COLOR_NAME_ITEM_OPTION, 4);
	file.read((char*)&COLOR_NAME_ITEM_RARE_OPTION, 4);
	file.read((char*)&COLOR_OUTLINE_ITEM, 2);

	
	file.read((char*)&COLOR_NAME_VAMPIRE, 4);
	file.read((char*)&COLOR_NAME_SLAYER, 4);
	file.read((char*)&COLOR_NAME_NPC, 4);

	
	file.read((char*)&COLOR_OUTLINE_NPC, 2);
	file.read((char*)&COLOR_OUTLINE_ATTACK_POSSIBLE, 2);
	file.read((char*)&COLOR_OUTLINE_ATTACK_IMPOSSIBLE, 2);

	
	file.read((char*)&COLOR_HP_BAR_R, 1);
	file.read((char*)&COLOR_HP_BAR_G, 1);
	file.read((char*)&COLOR_HP_BAR_B, 1);

	
	if (file.peek() != EOF && file.good()) {
		file.read((char*)&COLOR_HP_BAR_BG_R, 1);
		file.read((char*)&COLOR_HP_BAR_BG_G, 1);
		file.read((char*)&COLOR_HP_BAR_BG_B, 1);
	} else {
		
		COLOR_HP_BAR_BG_R = 8;
		COLOR_HP_BAR_BG_G = 8;
		COLOR_HP_BAR_BG_B = 8;
	}

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	file.read((char*)&NEW_USER_REGISTERATION_MODE, sizeof(NUR_MODE));
	URL_HOMEPAGE.LoadFromFile( file );				
	URL_HOMEPAGE_NEW_USER.LoadFromFile( file );		
	URL_HOMEPAGE_BILING.LoadFromFile( file );

	URL_HOMEPAGE					= "http://bbstest.web11.zcidc.com/bbs/index.asp";				
	URL_HOMEPAGE_NEW_USER			= "http://bbstest.web11.zcidc.com/bbs/index.asp";		
	URL_HOMEPAGE_BILING				= "http://bbstest.web11.zcidc.com/bbs/index.asp";
	
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	DWORD temp;
	READ_CHECK_EOF( COLOR_NAME_GOOD_MORE, temp, 4 )
	READ_CHECK_EOF( COLOR_NAME_GOOD, temp, 4 )
	READ_CHECK_EOF( COLOR_NAME_NEUTRAL, temp, 4 )
	READ_CHECK_EOF( COLOR_NAME_EVIL, temp, 4 )
	READ_CHECK_EOF( COLOR_NAME_EVIL_MORE, temp, 4 )	

	READ_CHECK_EOF( BLOOD_DROP_HP_PERCENT, temp, 4 )
	READ_CHECK_EOF( BLOOD_DROP_GAP_TIME, temp, 4 )
	READ_CHECK_EOF( BLOOD_DROP_RANDOM_TIME, temp, 4 )	
	READ_CHECK_EOF( MAX_TEXTUREPART_EFFECTSHADOW, temp, 4 )
	READ_CHECK_EOF( MAX_TEXTUREPART_SCREENEFFECT, temp, 4 )	

	READ_CHECK_EOF( MAX_REQUEST_SERVICE, temp, 4 )

	READ_CHECK_EOF( AFTER_TRADE_ITEM_DROP_DELAY, temp, 4 )

	READ_CHECK_EOF( DELAY_PLAYER_RESURRECT_TIMEOUT, temp, 4 )
	
	READ_CHECK_EOF( AFTER_PARTY_KICK_DELAY, temp, 4 )

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	READ_CHECK_EOF( CLIENT_COMMUNICATION_UDP_PORT, temp, 4 )

	READ_CHECK_EOF( CLIENT_COMMUNICATION_STATUS_DELAY, temp, 4 )
	
	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	READ_CHECK_EOF( TRADE_ACCEPT_DELAY_TIME, temp, 4)

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	BYTE temp1;
	READ_CHECK_EOF( REGEN_AMOUNT_BURROW, temp1, 1)
	READ_CHECK_EOF( REGEN_AMOUNT_CASKET, temp1, 1)
	READ_CHECK_EOF( REGEN_AMOUNT_VAMPIRE, temp1, 1)	

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------	
	READ_CHECK_EOF( MAX_SOUND_PER_SECOND, temp1, 1)

	
	READ_CHECK_EOF( REPEAT_TIME, temp, 4)
	READ_CHECK_EOF( LOCK_TIME, temp, 4)

	
	READ_CHECK_EOF( UniqueItemColorSet, temp, 4);
	READ_CHECK_EOF( QuestItemColorSet, temp, 4);

	
	
	READ_CHECK_EOF( HPModifyListMax, temp1, 1);
	READ_CHECK_EOF( HPModifyListTime, temp, 4);
	READ_CHECK_EOF( TRACE_CHARACTER_LIMIT_TIME, temp, 4 );
	
	return true;
}
