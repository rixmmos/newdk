//----------------------------------------------------------------------
// UserInformation.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__USERINFORMATION_H__
#define	__USERINFORMATION_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include <vector>
#include <bitset>
#include "MString.h"
#include "RaceType.h"

struct WAR_INFO
{
	std::string zone_name;
	std::string attack_guild_name, defense_guild_name;
	WORD	zone_id;
	DWORD left_time;
	BYTE war_type;
};

class MPetItem;

class UserInformation {
public:
    UserInformation();
    ~UserInformation();

    //--------------------------------------------------------
    // File I/O
    //--------------------------------------------------------
    //void	SaveToFile(const char* filename);
    //bool	LoadFromFile(const char* filename);

public:
    MString UserID;
    MString Character[3];
    enum Race Race[3];
    MString CharacterID;
    MString GuildName;
    BYTE GuildGrade;
    int PCSNumber;
    int FaceStyle;
    int FaceStyleSlot[3];
    int HairColor;
    int SkinColor;
    int BatColor;
    MString WhisperID;

    MString PCSUserName[3];
    int OtherPCSNumber[3];


    //
    int Slot;


    DWORD GlobalSayTime;

    bool GoreLevel;

    BOOL KeepConnection;
    BOOL IsMaster;
    DWORD ItemDropEnableTime;
    bool HasSkillRestore;
    DWORD LogoutTime;
    DWORD GameVersion;
    bool HasMagicGroundAttack;
    bool HasMagicHallu;
    bool HasMagicBloodySnake;
    bool HasMagicBloodyWarp;


    bool IsNetmarble;
    MString NetmarbleID;
    MString NetmarblePassword;
    bool bNetmarbleGoreLevel;

    int WorldID;
    int ServerID;

    std::vector<WAR_INFO> WarInfo;

    bool bMetrotech;
    MString WorldName;

    bool bChinese;
    bool bKorean;
    bool bJapanese;
    bool bEnglish;

    void SetKorean() {
        bKorean = true;
        bChinese = bJapanese = bEnglish = false;
    }
    void SetChinese() {
        bChinese = true;
        bKorean = bJapanese = bEnglish = false;
    }
    void SetJapanese() {
        bJapanese = true;
        bKorean = bChinese = bEnglish = false;
    }
    void SetEnglish() {
        bEnglish = true;
        bKorean = bChinese = bJapanese = false;
    }


    int HeadPrice;

    bool bCompetence;
    bool bCompetenceShape;

    // 2004, 7, 15, sobeit add start
    bool IsAutoLogIn;
    MString AutoLogInKeyValue;
    void* pLogInClientPlayer;
    // 2004, 7, 15, sobeit add end

    // 2004, 9, 14, sobeit add start
    bool IsNonPK;
    // 2004, 9, 14, sobeit add end
    // 2004, 10, 18, sobeit add start
    DWORD dwUnionID;
    BYTE bUnionGrade;
    // 2004, 10, 18, sobeit add end

    // 2004, 12, 14, sobeit add start
    bool IsTestServer;
    // 2004, 12, 14, sobeit add end
};


extern	UserInformation	*	g_pUserInformation;

#endif

