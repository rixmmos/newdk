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
#include "PreparedStatement.h"
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
        Connection* pConn = g_pDatabaseManager->getConnection(WorldID);
        pStmt = pConn->createStatement();

        
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
        PreparedStatement nameStmt(pConn, "SELECT Name FROM Slayer WHERE Name = ?");
        nameStmt.bindString(1, pPacket->getName());
        pResult = nameStmt.execute();
        if (pResult->getRowCount() != 0) {
            lcCreatePCError.setErrorID(ALREADY_REGISTER_ID);
            throw DuplicatedException("  .");
        }


        PreparedStatement slotStmt(pConn, "SELECT Name FROM Slayer WHERE PlayerID =? and Slot =? AND Active='ACTIVE'");
        slotStmt.bindString(1, pLoginPlayer->getID());
        slotStmt.bindString(2, Slot2String[pPacket->getSlot()]);
        pResult = slotStmt.execute();
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
            PreparedStatement strGoalStmt(pConn, "SELECT GoalExp FROM STRBalanceInfo WHERE Level = ?");
            strGoalStmt.bindInt(1, nSTR);
            pResult = strGoalStmt.execute();
            if (pResult->next())
                nSTRGoalExp = STRGoalExp[nSTR] = pResult->getInt(1);
        }

        nSTRExp = STRAccumExp[nSTR - 1];
        if (nSTRExp == 0) {
            PreparedStatement strAccumStmt(pConn, "SELECT AccumExp FROM STRBalanceInfo WHERE Level = ?");
            strAccumStmt.bindInt(1, nSTR - 1);
            pResult = strAccumStmt.execute();
            if (pResult->next())
                nSTRExp = STRAccumExp[nSTR - 1] = pResult->getInt(1);
        }

        nDEXGoalExp = DEXGoalExp[nDEX];
        if (nDEXGoalExp == 0) {
            PreparedStatement dexGoalStmt(pConn, "SELECT GoalExp FROM DEXBalanceInfo WHERE Level = ?");
            dexGoalStmt.bindInt(1, nDEX);
            pResult = dexGoalStmt.execute();
            if (pResult->next())
                nDEXGoalExp = DEXGoalExp[nDEX] = pResult->getInt(1);
        }

        nDEXExp = DEXAccumExp[nDEX - 1];
        if (nDEXExp == 0) {
            PreparedStatement dexAccumStmt(pConn, "SELECT AccumExp FROM DEXBalanceInfo WHERE Level = ?");
            dexAccumStmt.bindInt(1, nDEX - 1);
            pResult = dexAccumStmt.execute();
            if (pResult->next())
                nDEXExp = DEXAccumExp[nDEX - 1] = pResult->getInt(1);
        }

        nINTGoalExp = INTGoalExp[nINT];
        if (nINTGoalExp == 0) {
            PreparedStatement intGoalStmt(pConn, "SELECT GoalExp FROM INTBalanceInfo WHERE Level = ?");
            intGoalStmt.bindInt(1, nINT);
            pResult = intGoalStmt.execute();
            if (pResult->next())
                nINTGoalExp = INTGoalExp[nINT] = pResult->getInt(1);
        }

        nINTExp = INTAccumExp[nINT - 1];
        if (nINTExp == 0) {
            PreparedStatement intAccumStmt(pConn, "SELECT AccumExp FROM INTBalanceInfo WHERE Level = ?");
            intAccumStmt.bindInt(1, nINT - 1);
            pResult = intAccumStmt.execute();
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

        PreparedStatement slayerInsertStmt(
            pConn,
            "INSERT INTO Slayer (Race, Name, PlayerID, Slot, ServerGroupID, Active, Sex, HairStyle, HairColor, "
            "SkinColor, Phone, STR, STRExp, STRGoalExp, DEX, DEXExp, DEXGoalExp, INTE, INTExp, INTGoalExp, `Rank`, "
            "RankExp, RankGoalExp, HP, CurrentHP, MP, CurrentMP, ZoneID, XCoord, YCoord, Sight, Gold, Alignment, "
            "Shape, HelmetColor, JacketColor, PantsColor, WeaponColor, ShieldColor, creation_date) VALUES (?, ?, "
            "?, ?, ?, 'ACTIVE', ?, ?, ?, ?, 0, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
            "?, ?, 12, 237, 138, 13, 0, 7500, ?, ?, ?, ?, ?, ?, now())");
        slayerInsertStmt.bindString(1, race);
        slayerInsertStmt.bindString(2, pPacket->getName());
        slayerInsertStmt.bindString(3, pLoginPlayer->getID());
        slayerInsertStmt.bindString(4, Slot2String[pPacket->getSlot()]);
        slayerInsertStmt.bindInt(5, (int)CurrentServerGroupID);
        slayerInsertStmt.bindString(6, Sex2String[pPacket->getSex()]);
        slayerInsertStmt.bindString(7, HairStyle2String[pPacket->getHairStyle()]);
        slayerInsertStmt.bindInt(8, (int)pPacket->getHairColor());
        slayerInsertStmt.bindInt(9, (int)pPacket->getSkinColor());
        slayerInsertStmt.bindInt(10, (int)pPacket->getSTR());
        slayerInsertStmt.bindInt(11, nSTRExp);
        slayerInsertStmt.bindInt(12, nSTRGoalExp);
        slayerInsertStmt.bindInt(13, (int)pPacket->getDEX());
        slayerInsertStmt.bindInt(14, nDEXExp);
        slayerInsertStmt.bindInt(15, nDEXGoalExp);
        slayerInsertStmt.bindInt(16, (int)pPacket->getINT());
        slayerInsertStmt.bindInt(17, nINTExp);
        slayerInsertStmt.bindInt(18, nINTGoalExp);
        slayerInsertStmt.bindInt(19, 1);
        slayerInsertStmt.bindInt(20, 0);
        slayerInsertStmt.bindInt(21, RankGoalExpSlayer);
        slayerInsertStmt.bindInt(22, (int)pPacket->getSTR() * 2);
        slayerInsertStmt.bindInt(23, (int)pPacket->getSTR() * 2);
        slayerInsertStmt.bindInt(24, (int)pPacket->getINT() * 2);
        slayerInsertStmt.bindInt(25, (int)pPacket->getINT() * 2);
        slayerInsertStmt.bindInt(26, slayerShape);
        slayerInsertStmt.bindInt(27, (int)HelmetColor);
        slayerInsertStmt.bindInt(28, (int)JacketColor);
        slayerInsertStmt.bindInt(29, (int)PantsColor);
        slayerInsertStmt.bindInt(30, (int)WeaponColor);
        slayerInsertStmt.bindInt(31, (int)ShieldColor);
        slayerInsertStmt.execute();

        
        
        
        
        if (pPacket->getRace() == RACE_VAMPIRE) {
            PreparedStatement vampireInsertStmt(
                pConn,
                "INSERT INTO Vampire ( Name, PlayerID, Slot, ServerGroupID, Active, Sex, SkinColor, STR, DEX, INTE, "
                "HP, CurrentHP, ZoneID, XCoord, YCoord, Sight, Alignment, Exp, GoalExp, `Rank`, RankExp, RankGoalExp, "
                "Shape, CoatColor) VALUES ( ?, ?, ?, ?, 'ACTIVE', ?, ?, 20, 20, 20, 50, 50, 1003, 62, "
                "64, 13, 7500, 0, ?, 1, 0, ?, ?, 377 )");
            vampireInsertStmt.bindString(1, pPacket->getName());
            vampireInsertStmt.bindString(2, pLoginPlayer->getID());
            vampireInsertStmt.bindString(3, Slot2String[pPacket->getSlot()]);
            vampireInsertStmt.bindInt(4, (int)CurrentServerGroupID);
            vampireInsertStmt.bindString(5, Sex2String[pPacket->getSex()]);
            vampireInsertStmt.bindInt(6, (int)pPacket->getSkinColor());
            vampireInsertStmt.bindInt(7, GoalExpVampire); // by sigi. 2002.12.20
            vampireInsertStmt.bindInt(8, RankGoalExpVampire);
            vampireInsertStmt.bindInt(9, vampireShape);
            vampireInsertStmt.execute();
        } else if (pPacket->getRace() == RACE_OUSTERS) {
            PreparedStatement oustersInsertStmt(
                pConn,
                "INSERT INTO Ousters ( Name, PlayerID, Slot, ServerGroupID, Active, Sex, STR, DEX, "
                "INTE, BONUS, HP, CurrentHP, MP, CurrentMP, ZoneID, XCoord, YCoord, Sight, Alignment, "
                "Exp, GoalExp, `Rank`, RankExp, RankGoalExp, CoatColor, HairColor, ArmColor, "
                "BootsColor ) Values ( ?, ?, ?, ?, 'ACTIVE', 'FEMALE', ?, ?, ?, 0, 50, "
                "50, 50, 50, 1311, 24, 73, 13, 7500, 0, ?, 1, 0,	?, 377, ?, 377, 377 )");
            oustersInsertStmt.bindString(1, pPacket->getName());
            oustersInsertStmt.bindString(2, pLoginPlayer->getID());
            oustersInsertStmt.bindString(3, Slot2String[pPacket->getSlot()]);
            oustersInsertStmt.bindInt(4, (int)CurrentServerGroupID);
            oustersInsertStmt.bindInt(5, (int)pPacket->getSTR());
            oustersInsertStmt.bindInt(6, (int)pPacket->getDEX());
            oustersInsertStmt.bindInt(7, (int)pPacket->getINT());
            oustersInsertStmt.bindInt(8, GoalExpOusters);
            oustersInsertStmt.bindInt(9, RankGoalExpOusters);
            oustersInsertStmt.bindInt(10, (int)pPacket->getHairColor());
            oustersInsertStmt.execute();
        }

        if (pPacket->getRace() == RACE_SLAYER) {
            PreparedStatement flagSetStmt(pConn, "INSERT IGNORE INTO FlagSet (OwnerID, FlagData) VALUES (?,'11110010001')");
            flagSetStmt.bindString(1, pPacket->getName());
            flagSetStmt.execute();
        } else {
            PreparedStatement flagSetStmt(pConn, "INSERT IGNORE INTO FlagSet (OwnerID, FlagData) VALUES (?,'00000000001')");
            flagSetStmt.bindString(1, pPacket->getName());
            flagSetStmt.execute();
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
