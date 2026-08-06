//////////////////////////////////////////////////////////////////////////////
// Filename    : CLCreatePCHandler.cc
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLCreatePC.h"

#ifdef __LOGIN_SERVER__
#include <string.h>

#include <list>

#include "Assert.h"
#include "DB.h"
#include "GameServerInfoManager.h"
#include "LCCreatePCError.h"
#include "LCCreatePCOK.h"
#include "LoginPlayer.h"
#include "PCSlayerInfo.h"
#include "chinabilling/CBillingInfo.h"
#endif

bool isAvailableID(const char* pID);

#ifdef __THAILAND_SERVER__
// tis620 charset filter functions
int extNumberic(string srcStr);
int extEnglish(string srcStr);
int extAsciiSpecial(string srcStr);
int extTis620Normal(string srcStr);
bool isAllowString(string str);
#endif

#ifdef __CHINA_SERVER__
// GB2312(simple chinese) charset filter functions
int extNumberic(string srcStr);
int extEnglish(string srcStr);
int extGb2312Normal(string srcStr);
int extGb2312Special(string srcStr);
int extAsciiSpecial(string srcStr);
bool isAllowString(string str);
#endif


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CLCreatePCHandler::execute(CLCreatePC* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);
    Statement* pStmt = NULL;
    Result* pResult = NULL;
    LCCreatePCError lcCreatePCError;
    WorldID_t WorldID = pLoginPlayer->getWorldID();

#ifdef __CONNECT_CBILLING_SYSTEM__
    if (pLoginPlayer->isCBillingVerified()) {
        if (!pLoginPlayer->isPayPlayer()) {
            lcCreatePCError.setErrorID(CANNOT_CREATE_PC_BILLING);
            pLoginPlayer->sendPacket(&lcCreatePCError);
            return;
        }
    } else {
        lcCreatePCError.setErrorID(CANNOT_AUTHORIZE_BILLING);
        pLoginPlayer->sendPacket(&lcCreatePCError);
        return;
    }
#endif

    try {
        pStmt = g_pDatabaseManager->getConnection(WorldID)->createStatement();

        
        // NONE, ZONE***, INV***, QUICK...
        // string text = pPacket->getName();

        if (!isAvailableID(pPacket->getName().c_str())) {
            lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
            throw DuplicatedException("  .");
        }

         
#if defined(__THAILAND_SERVER__) || defined(__CHINA_SERVER__)

         

         

        string tmpStr = pPacket->getName();
        bool isAllowStr = isAllowString(tmpStr);

        if (isAllowStr == false) {
            lcCreatePCError.setErrorID(ETC_ERROR);
            throw DuplicatedException("    ");
        }

#endif


        
        ///*
        pResult = pStmt->executeQuery("SELECT Name FROM Slayer WHERE Name = '%s'", pPacket->getName().c_str());
        if (pResult->getRowCount() != 0) {
            lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
            throw DuplicatedException("  .");
        }

        
        pResult = pStmt->executeQuery("SELECT Name FROM Slayer WHERE PlayerID ='%s' and Slot ='%s' AND Active='ACTIVE'",
                                      pLoginPlayer->getID().c_str(), Slot2String[pPacket->getSlot()].c_str());
        if (pResult->getRowCount() != 0) {
            lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
            throw DuplicatedException("  .");
        }
        //*/
        
         


        


        bool bInvalidAttr = false;
        int nSTR = pPacket->getSTR();
        int nSTRExp = 0;
        int nSTRGoalExp = 0;
        int nDEX = pPacket->getDEX();
        int nDEXExp = 0;
        int nDEXGoalExp = 0;
        int nINT = pPacket->getINT();
        int nINTExp = 0;
        int nINTGoalExp = 0;

        // int  Rank         = 1;
        // int  RankExp      = 0;

        //
        static int RankGoalExpSlayer = -1;
        static int GoalExpVampire = -1; // by sigi. 2002.12.20
        static int RankGoalExpVampire = -1;
        static int GoalExpOusters = -1;
        static int RankGoalExpOusters = -1;

        if (RankGoalExpSlayer == -1) {
            pResult = pStmt->executeQuery("SELECT GoalExp FROM RankEXPInfo WHERE Level=1 AND RankType=0");
            if (pResult->next())
                RankGoalExpSlayer = pResult->getInt(1);
        }

        if (GoalExpVampire == -1) // by sigi. 2002.12.20
        {
            pResult = pStmt->executeQuery("SELECT GoalExp FROM VampEXPBalanceInfo WHERE Level=1");
            if (pResult->next())
                GoalExpVampire = pResult->getInt(1);
        }

        if (GoalExpOusters == -1) {
            pResult = pStmt->executeQuery("SELECT GoalExp FROM OustersEXPBalanceInfo WHERE Level=1");
            if (pResult->next())
                GoalExpOusters = pResult->getInt(1);
        }

        if (RankGoalExpVampire == -1) {
            pResult = pStmt->executeQuery("SELECT GoalExp FROM RankEXPInfo WHERE Level=1 AND RankType=1");
            if (pResult->next())
                RankGoalExpVampire = pResult->getInt(1);
        }

        if (RankGoalExpOusters == -1) {
            pResult = pStmt->executeQuery("SELECT GoalExp FROM RankEXPInfo WHERE Level=1 AND RankType=2");
            if (pResult->next())
                RankGoalExpOusters = pResult->getInt(1);
        }

        if (pPacket->getRace() == RACE_SLAYER) {
            if (nSTR < 5 || nSTR > 20)
                bInvalidAttr = true;
            if (nDEX < 5 || nDEX > 20)
                bInvalidAttr = true;
            if (nINT < 5 || nINT > 20)
                bInvalidAttr = true;
            if (nSTR + nDEX + nINT > 30)
                bInvalidAttr = true;

            // cout << "Slayer: " << nSTR << ", " << nDEX << ", " << nINT << endl;
        } else if (pPacket->getRace() == RACE_VAMPIRE) 
        {
            if (nSTR != 20 || nDEX != 20 || nINT != 20) {
                bInvalidAttr = true;
            } else {
                
                
                // by sigi. 2002.11.7
                nSTR = 5 + rand() % 16; // 5~20
                nDEX = 5 + rand() % (21 - nSTR);
                nINT = 30 - nSTR - nDEX;

                pPacket->setSTR(nSTR);
                pPacket->setDEX(nDEX);
                pPacket->setINT(nINT);
            }
            //

            // cout << "Vampire: " << nSTR << ", " << nDEX << ", " << nINT << endl;
        } else if (pPacket->getRace() == RACE_OUSTERS) {
            /*			if ( nSTR < 10 ) bInvalidAttr = true;
                        if ( nDEX < 10 ) bInvalidAttr = true;
                        if ( nINT < 10 ) bInvalidAttr = true;*/

            if (nSTR < 10 || nDEX < 10 || nINT < 10) {
                filelog("CreatePC.log", "Illegal PC Create [%s:%s] : %u/%u/%u", pPlayer->getID().c_str(),
                        pPacket->getName().c_str(), nSTR, nDEX, nINT);
            }

            if (nSTR + nDEX + nINT != 45)
                bInvalidAttr = true;
        }

        if (bInvalidAttr) {
            SAFE_DELETE(pStmt);
            throw InvalidProtocolException("CLCreatePCHandler::too large character attribute");
        }

        
        ServerGroupID_t CurrentServerGroupID = pPlayer->getServerGroupID();

        
        
        static int STRGoalExp[100] = {
            0,
        };
        static int STRAccumExp[100] = {
            0,
        };
        static int DEXGoalExp[100] = {
            0,
        };
        static int DEXAccumExp[100] = {
            0,
        };
        static int INTGoalExp[100] = {
            0,
        };
        static int INTAccumExp[100] = {
            0,
        };

        nSTRGoalExp = STRGoalExp[nSTR];
        if (nSTRGoalExp == 0) {
            pResult = pStmt->executeQuery("SELECT GoalExp FROM STRBalanceInfo WHERE Level = %d", nSTR);
            if (pResult->next())
                nSTRGoalExp = STRGoalExp[nSTR] = pResult->getInt(1);
        }

        nSTRExp = STRAccumExp[nSTR - 1];
        if (nSTRExp == 0) {
            pResult = pStmt->executeQuery("SELECT AccumExp FROM STRBalanceInfo WHERE Level = %d", nSTR - 1);
            if (pResult->next())
                nSTRExp = STRAccumExp[nSTR - 1] = pResult->getInt(1);
        }

        nDEXGoalExp = DEXGoalExp[nDEX];
        if (nDEXGoalExp == 0) {
            pResult = pStmt->executeQuery("SELECT GoalExp FROM DEXBalanceInfo WHERE Level = %d", nDEX);
            if (pResult->next())
                nDEXGoalExp = DEXGoalExp[nDEX] = pResult->getInt(1);
        }

        nDEXExp = DEXAccumExp[nDEX - 1];
        if (nDEXExp == 0) {
            pResult = pStmt->executeQuery("SELECT AccumExp FROM DEXBalanceInfo WHERE Level = %d", nDEX - 1);
            if (pResult->next())
                nDEXExp = DEXAccumExp[nDEX - 1] = pResult->getInt(1);
        }

        nINTGoalExp = INTGoalExp[nINT];
        if (nINTGoalExp == 0) {
            pResult = pStmt->executeQuery("SELECT GoalExp FROM INTBalanceInfo WHERE Level = %d", nINT);
            if (pResult->next())
                nINTGoalExp = INTGoalExp[nINT] = pResult->getInt(1);
        }

        nINTExp = INTAccumExp[nINT - 1];
        if (nINTExp == 0) {
            pResult = pStmt->executeQuery("SELECT AccumExp FROM INTBalanceInfo WHERE Level = %d", nINT - 1);
            if (pResult->next())
                nINTExp = INTAccumExp[nINT - 1] = pResult->getInt(1);
        }

        
        DWORD slayerShape = (pPacket->getSex() == 1 ? 1 : 0);
        DWORD vampireShape = slayerShape;

        slayerShape |= (pPacket->getHairStyle() << PCSlayerInfo::SLAYER_BIT_HAIRSTYLE1);

        Color_t HelmetColor = 0;
        Color_t JacketColor = 0;
        Color_t PantsColor = 0;
        Color_t WeaponColor = 0;
        Color_t ShieldColor = 0;

        /*
        StringStream slayerSQL;
        slayerSQL << "INSERT INTO Slayer ("
            << " Race, Name, PlayerID, Slot, ServerGroupID, Active,"
            << " Sex, HairStyle, HairColor, SkinColor, Phone, "
            << " STR, STRExp, STRGoalExp, DEX, DEXExp, DEXGoalExp, INTE, INTExp, INTGoalExp, HP, CurrentHP, MP,
        CurrentMP,"
            << " ZoneID, XCoord, YCoord, Sight, Gold, Alignment,"
            << " Shape, HelmetColor, JacketColor, PantsColor, WeaponColor, ShieldColor,"
            << " creation_date) VALUES ('"
            << "SLAYER" << "', '"
            << pPacket->getName() << "', '"
            << pLoginPlayer->getID() << "', '"
            << Slot2String[pPacket->getSlot()] << "', "
            << (int)CurrentServerGroupID << " , "
            << "'ACTIVE', '"
            << Sex2String[pPacket->getSex()] << "', '"
            << HairStyle2String[pPacket->getHairStyle()] << "', "
            << (int)pPacket->getHairColor() << ", "
            << (int)pPacket->getSkinColor() << ", '"
            << (int)0 << "', "
            << (int)pPacket->getSTR() << ", "
            << nSTRExp << ", "
            << nSTRGoalExp << ", "
            << (int)pPacket->getDEX() << ", "
            << nDEXExp << ", "
            << nDEXGoalExp << ", "
            << (int)pPacket->getINT() << ", "
            << nINTExp << ", "
            << nINTGoalExp << ", "
            << (int)pPacket->getSTR()*2 << ","
            << (int)pPacket->getSTR()*2 << ","
            << (int)pPacket->getINT()*2 << ","
            << (int)pPacket->getINT()*2 << ","
            << "2101, 65, 45, 13, 0, 7500, "
            << slayerShape << ", "
            << (int)HelmetColor << ", "
            << (int)JacketColor << ", "
            << (int)PantsColor << ", "
            << (int)WeaponColor << ", "
            << (int)ShieldColor << ", "
            << "now() "
            << ")";

        StringStream vampireSQL;
        vampireSQL << "INSERT INTO Vampire ("
            << " Name, PlayerID, Slot, ServerGroupID, Active,"
            << " Sex, HairColor, SkinColor,"
            << " STR, DEX, INTE, HP, CurrentHP,"
            << " ZoneID, XCoord, YCoord, Sight, Alignment, Exp, GoalExp, Shape) VALUES ('"
            << pPacket->getName() << "', '"
            << pLoginPlayer->getID() << "', '"
            << Slot2String[pPacket->getSlot()] << "', "
            << (int)CurrentServerGroupID << " , "
            << "'ACTIVE', '"
            << Sex2String[pPacket->getSex()] << "', "
            << (int)pPacket->getHairColor() << ", "
            << (int)pPacket->getSkinColor() << ", "
            << "20, 20, 20, 50, 50, "
            << "2020, 233, 55, 13, 7500, 0, 125, "
            << vampireShape
            << ")";

        pStmt->executeQuery(slayerSQL.toString());
        pStmt->executeQuery(vampireSQL.toString());
        */

        
        // by sigi. 2002.10.31
        string race;
        switch (pPacket->getRace()) {
        case RACE_SLAYER:
            race = "SLAYER";
            break;
        case RACE_VAMPIRE:
            race = "VAMPIRE";
            break;
        case RACE_OUSTERS:
            race = "OUSTERS";
            break;
        default:
            lcCreatePCError.setErrorID(ETC_ERROR);
            pLoginPlayer->sendPacket(&lcCreatePCError); 
            return;
        }

        pStmt->executeQuery(
            "INSERT INTO Slayer (Race, Name, PlayerID, Slot, ServerGroupID, Active, Sex, HairStyle, HairColor, "
            "SkinColor, Phone, STR, STRExp, STRGoalExp, DEX, DEXExp, DEXGoalExp, INTE, INTExp, INTGoalExp, `Rank`, "
            "RankExp, RankGoalExp, HP, CurrentHP, MP, CurrentMP, ZoneID, XCoord, YCoord, Sight, Gold, Alignment, "
            "Shape, HelmetColor, JacketColor, PantsColor, WeaponColor, ShieldColor, creation_date) VALUES ('%s', '%s', "
            "'%s', '%s', %d, 'ACTIVE', '%s', '%s', %d, %d, 0, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
            "%d, %d, 12, 237, 138, 13, 0, 7500, %d, %d, %d, %d, %d, %d, now())",
            race.c_str(), pPacket->getName().c_str(), pLoginPlayer->getID().c_str(),
            Slot2String[pPacket->getSlot()].c_str(), (int)CurrentServerGroupID, Sex2String[pPacket->getSex()].c_str(),
            HairStyle2String[pPacket->getHairStyle()].c_str(), (int)pPacket->getHairColor(),
            (int)pPacket->getSkinColor(), (int)pPacket->getSTR(), nSTRExp, nSTRGoalExp, (int)pPacket->getDEX(), nDEXExp,
            nDEXGoalExp, (int)pPacket->getINT(), nINTExp, nINTGoalExp, 1, 0, RankGoalExpSlayer,
            (int)pPacket->getSTR() * 2, (int)pPacket->getSTR() * 2, (int)pPacket->getINT() * 2,
            (int)pPacket->getINT() * 2, slayerShape, (int)HelmetColor, (int)JacketColor, (int)PantsColor,
            (int)WeaponColor, (int)ShieldColor);

        
        
        
        
        if (pPacket->getRace() == RACE_VAMPIRE) {
            pStmt->executeQuery(
                "INSERT INTO Vampire ( Name, PlayerID, Slot, ServerGroupID, Active, Sex, SkinColor, STR, DEX, INTE, "
                "HP, CurrentHP, ZoneID, XCoord, YCoord, Sight, Alignment, Exp, GoalExp, `Rank`, RankExp, RankGoalExp, "
                "Shape, CoatColor) VALUES ( '%s', '%s', '%s', %d, 'ACTIVE', '%s', %d, 20, 20, 20, 50, 50, 1003, 62, "
                "64, 13, 7500, 0, %d, 1, 0, %d, %d, 377 )",
                pPacket->getName().c_str(), pLoginPlayer->getID().c_str(), Slot2String[pPacket->getSlot()].c_str(),
                (int)CurrentServerGroupID, Sex2String[pPacket->getSex()].c_str(), (int)pPacket->getSkinColor(),
                GoalExpVampire, // by sigi. 2002.12.20
                RankGoalExpVampire, vampireShape);
        } else if (pPacket->getRace() == RACE_OUSTERS) {
            pStmt->executeQuery("INSERT INTO Ousters ( Name, PlayerID, Slot, ServerGroupID, Active, Sex, STR, DEX, "
                                "INTE, BONUS, HP, CurrentHP, MP, CurrentMP, ZoneID, XCoord, YCoord, Sight, Alignment, "
                                "Exp, GoalExp, `Rank`, RankExp, RankGoalExp, CoatColor, HairColor, ArmColor, "
                                "BootsColor ) Values ( '%s', '%s', '%s', %d, 'ACTIVE', 'FEMALE', %d, %d, %d, 0, 50, "
                                "50, 50, 50, 1311, 24, 73, 13, 7500, 0, %d, 1, 0,	%d, 377, %d, 377, 377 )",
                                pPacket->getName().c_str(), pLoginPlayer->getID().c_str(),
                                Slot2String[pPacket->getSlot()].c_str(), (int)CurrentServerGroupID,
                                (int)pPacket->getSTR(), (int)pPacket->getDEX(), (int)pPacket->getINT(), GoalExpOusters,
                                RankGoalExpOusters, (int)pPacket->getHairColor());
        }

        if (pPacket->getRace() == RACE_SLAYER) {
            pStmt->executeQuery("INSERT IGNORE INTO FlagSet (OwnerID, FlagData) VALUES ('%s','11110010001')",
                                pPacket->getName().c_str());
        } else {
            pStmt->executeQuery("INSERT IGNORE INTO FlagSet (OwnerID, FlagData) VALUES ('%s','00000000001')",
                                pPacket->getName().c_str());
        }

        
        LCCreatePCOK lcCreatePCOK;
        pLoginPlayer->sendPacket(&lcCreatePCOK);
        pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);

        SAFE_DELETE(pStmt);
    } catch (DuplicatedException& de) {
        SAFE_DELETE(pStmt);
        pLoginPlayer->sendPacket(&lcCreatePCError); 
    } catch (SQLQueryException& sqe) {
        SAFE_DELETE(pStmt);
        lcCreatePCError.setErrorID(ETC_ERROR);
        pLoginPlayer->sendPacket(&lcCreatePCError); 
    }

#endif

    __END_DEBUG_EX __END_CATCH
}

bool isAvailableID(const char* pID) {
    const int maxInvalidID = 10;
    static const char* invalidID[maxInvalidID] = {"NONE",   "", "", "", "",
                                                  "", "", "", "",   "GM"};

    if (pID == NULL || pID[0] == '\0') {
        return false;
    }

    int length = 0;
    for (const char* p = pID; *p != '\0'; ++p) {
        if (!((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z'))) {
            return false;
        }
        ++length;
    }

    if (length > 10) {
        return false;
    }

    for (int i = 0; i < maxInvalidID; i++) {
        if (invalidID[i] == NULL || invalidID[i][0] == '\0') {
            continue;
        }

        if (strstr(pID, invalidID[i]) != NULL) {
            return false;
        }
    }

    return true;
}

#ifdef __THAILAND_SERVER__
int extNumberic(string srcStr) {
    unsigned char ch;
    int nNumChar = 0;

    if (srcStr.size() <= 0)
        return 0;
    else {
        for (unsigned int i = 0; i < srcStr.size(); i++) {
            ch = srcStr[i];
            if (ch >= '0' && ch <= '9') {
                if (i == 0) {
                    return -1;
                } else {
                    nNumChar++;
                }
            }
        }
    }

    return nNumChar;
}
int extEnglish(string srcStr) {
    unsigned char ch;
    int nEngChar = 0;

    if (srcStr.size() <= 0)
        return 0;
    else {
        for (unsigned int i = 0; i < srcStr.size(); i++) {
            ch = srcStr[i];
            if (ch >= 97 && ch <= 122) {
                nEngChar++;
            } else if (ch >= 65 && ch <= 90) {
                nEngChar++;
            }
        }
    }

    return nEngChar;
}
int extAsciiSpecial(string srcStr) {
    unsigned char ch;
    int nASPCChar = 0;

    if (srcStr.size() <= 0)
        return 0;
    else {
        for (int i = 0; i < srcStr.size(); i++) {
            ch = srcStr[i];

            if (ch >= 0x00 && ch <= 47) {
                nASPCChar++;
            } else if (ch >= 58 && ch <= 64) {
                nASPCChar++;
            } else if (ch >= 91 && ch <= 96) {
                nASPCChar++;
            } else if (ch >= 123 && ch <= 160) {
                nASPCChar++;
            } else if (ch >= 219 && ch <= 222) {
                nASPCChar++;
            } else if (ch >= 252) {
                nASPCChar++;
            }
        }
    }

    return nASPCChar;
}
int extTis620Normal(string srcStr) {
    unsigned char ch;
    int nNorChar = 0;

    if (srcStr.size() <= 0)
        return 0;
    else {
        for (int i = 0; i < srcStr.size(); i++) {
            ch = srcStr[i];

            if (ch >= 161 && ch <= 218) {
                nNorChar++;
            } else if (ch >= 223 && ch <= 251) {
                nNorChar++;
            }
        }
    }

    return nNorChar;
}

bool isAllowString(string str) {
    bool isAllow = true;

    if (str.size() <= 0)
        isAllow = false;
    else {
        int nEng = extEnglish(str);
        int nNum = extNumberic(str);
        int nASpc = extAsciiSpecial(str);
        int nNor = extTis620Normal(str);

        if (nNum == -1)
            isAllow = false; 
        if (nEng && nNor)
            isAllow = false; 
        if (nASpc)
            isAllow = false; 
    }

    return isAllow;
}

#endif


#ifdef __CHINA_SERVER__




int extNumberic(string srcStr) {
    unsigned char ch;
    int nNumChar = 0;

    if (srcStr.size() <= 0)
        return 0;
    else {
        for (unsigned int i = 0; i < srcStr.size(); i++) {
            ch = srcStr[i];
            if (ch >= '0' && ch <= '9') {
                if (i == 0) {
                    return -1;
                } else {
                    nNumChar++;
                }
            }
        }
    }

    return nNumChar;
}





int extEnglish(string srcStr) {
    unsigned char ch;
    int nEngChar = 0;

    if (srcStr.size() <= 0)
        return 0;
    else {
        for (unsigned int i = 0; i < srcStr.size(); i++) {
            ch = srcStr[i];
            if (ch >= 97 && ch <= 122) {
                nEngChar++;
            } else if (ch >= 65 && ch <= 90) {
                nEngChar++;
            }
        }
    }

    return nEngChar;
}

int extGb2312Normal(string srcStr) {
    unsigned char ch;
    int nNormalChar = 0;

    if (srcStr.size() <= 0)
        return 0;
    else {
        for (unsigned int i = 0; i < srcStr.size(); i++) {
            ch = srcStr[i];
            if (ch >= 0xB0 && ch <= 0xF7) {
                unsigned char ch2 = srcStr[i + 1];
                if (ch2 > 0xA0 && ch < 0xFF) {
                    nNormalChar++;
                    i++;
                }
            } else if ((ch >= 0xA1 && ch <= 0xAF) || (ch >= 0xF8 && ch <= 0xFE)) {
                unsigned char ch2 = srcStr[i + 1];
                if (ch2 >= 0xA0 && ch <= 0xFF) {
                    i++;
                }
            }
        }
    }
    return nNormalChar;
}
int extGb2312Special(string srcStr) {
    unsigned char ch;
    int nSpecialChar = 0;

    if (srcStr.size() <= 0)
        return 0;
    else {
        for (unsigned int i = 0; i < srcStr.size(); i++) {
            ch = srcStr[i];
            if (ch >= 0xB0 && ch <= 0xF7) {
                unsigned char ch2 = srcStr[i + 1];
                if (ch2 > 0xA0 && ch < 0xFF) {
                    i++;
                }
            } else if ((ch >= 0xA1 && ch <= 0xAF) || (ch >= 0xF8 && ch <= 0xFE)) {
                unsigned char ch2 = srcStr[i + 1];
                if (ch2 >= 0xA0 && ch <= 0xFF) {
                    nSpecialChar++;
                    i++;
                }
            }
        }
    }
    return nSpecialChar;
}
int extAsciiSpecial(string srcStr) {
    unsigned char ch;
    int nASPCChar = 0;

    if (srcStr.size() <= 0)
        return 0;
    else {
        for (int i = 0; i < srcStr.size(); i++) {
            ch = srcStr[i];

            if (ch >= 0x00 && ch <= 47) {
                nASPCChar++;
            } else if (ch >= 58 && ch <= 64) {
                nASPCChar++;
            } else if (ch >= 91 && ch <= 96) {
                nASPCChar++;
            } else if (ch >= 123 && ch <= 127) {
                nASPCChar++;
            }
        }
    }

    return nASPCChar;
}
bool isAllowString(string str) {
    bool isAllow = true;

    if (str.size() <= 0)
        isAllow = false;
    else {
        int nEng = extEnglish(str);
        int nNum = extNumberic(str);
        int nNor = extGb2312Normal(str);
        int nSpc = extGb2312Special(str);
        int nASpc = extAsciiSpecial(str);

        if (nNum == -1)
            isAllow = false; 
        if (nSpc)
            isAllow = false; 
        if (nEng && nNor)
            isAllow = false; 
        if (nASpc)
            isAllow = false; 
    }

    return isAllow;
}

#endif
