//////////////////////////////////////////////////////////////////////////////
// Filename    : CLLoginHandler.cpp
// Written By  : Reiot
// Description :
//


//
// *CAUTION*
//


//


//

//
// (1) - CREATE INDEX PlayerIDIndex ON Player (ID)
// (2) - CREATE INDEX PlayerIDPasswordIndex ON Player (ID , Password)
//

//
// *CAUTION*
//


//


//////////////////////////////////////////////////////////////////////////////


#include "CLLogin.h"

#ifdef __LOGIN_SERVER__
#include <time.h>

#include <sys/time.h>

#include "Assert1.h"
#include "DB.h"
#include "DatabaseManager.h"
#include "GameServerGroupInfoManager.h"
#include "GameServerInfoManager.h"
#include "LCLoginError.h"
#include "LCLoginOK.h"
#include "LoginPlayer.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "UserInfoManager.h"
#include "chinabilling/CBillingInfo.h"
#include "gameserver/billing/BillingPlayerManager.h"
#include "types/ServerType.h"
#ifdef __CONNECT_CBILLING_SYSTEM__
#include "chinabilling/CBillingPlayerManager.h"
#endif

#endif

#define SYMBOL_TEST_CLIENT '#'
#define SYMBOL_NET_MARBLE_CLIENT '@'

bool isAdultByBirthday(const string& birthday);
void addLoginPlayerData(const string& ID, const string& ip, const string& SSN, const string& zipcode);

bool isBlockIP(const string& ip);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CLLoginHandler::execute(CLLogin* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    // cout << pPacket->toString().c_str() << endl;

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);
    Statement* pStmt = NULL;


    pPacket->setID(trim(pPacket->getID()));

    string connectIP = pLoginPlayer->getSocket()->getHost();
    string connectMAC = pPacket->getMacAddress();
    string ID = pPacket->getID();

    // MAC address setting
    pLoginPlayer->setMacAddress(pPacket->getRareMacAddress());

    bool bFreePass = false; // by sigi. 2002.10.23j

    // web login
    bool bWebLogin = pPacket->isWebLogin();
    //	static bool bWebLogin = g_pConfig->getPropertyInt("WebLogin") != 0;

    // set web login player
    if (bWebLogin)
        pLoginPlayer->setWebLogin();

    //	cout << pPacket->toString() << endl;

    if (isBlockIP(connectIP)) {
        LCLoginError lcLoginError;
        lcLoginError.setErrorID(IP_DENYED);
        pLoginPlayer->sendPacket(&lcLoginError);

        filelog("loginfail.txt", "Error Code: IP_DENYED, 1, PlayerID : %s", pPacket->getID().c_str());
        return;
    }


    if (ID[0] == SYMBOL_TEST_CLIENT) {
        ID = ID.c_str() + 1;
        pPacket->setID(ID);


        if (bWebLogin) {
            // cout << "WebLogin" << endl;

            if (!checkWebLogin(pPacket, pPlayer)) {
                return;
            }
            // else
            // cout << "Web Login OK" << endl;
        } else {
            // cout << "not WebLogin" << endl;


            // by sigi. 2002.10.23
            if (!checkNetMarbleClient(pPacket, pPlayer)) {
                return;
            }
        }

        bFreePass = pLoginPlayer->isFreePass();
        if (!bWebLogin && bFreePass) {
            ID = ID.c_str() + 1;
            pPacket->setID(ID);
        }


        BEGIN_DB {
            PreparedStatement testClientStmt(
                g_pDatabaseManager->getConnection("DARKEDEN"),
                "INSERT INTO TestClientUser (PlayerID, IP, LoginDate) VALUES (?, ?, now())");
            testClientStmt.bindString(1, ID);
            testClientStmt.bindString(2, connectIP);
            testClientStmt.execute();
        }
        END_DB(pStmt)
    }

    else {
        if (bWebLogin) {
            // cout << "WebLogin" << endl;

            if (!checkWebLogin(pPacket, pPlayer)) {
                return;
            }
            // else
            // cout << "Web Login OK" << endl;
        } else {
            // cout << "not WebLogin" << endl;

            // by sigi. 2002.10.23
            if (!checkNetMarbleClient(pPacket, pPlayer)) {
                return;
            }
        }

        bFreePass = pLoginPlayer->isFreePass();

        /*
        if (bFreePass)
        {
            ID = ID.c_str()+1;
            pPacket->setID(ID);
        }
        */
    }

    string PASSWORD = pPacket->getPassword();
    string SSN = "";
    ServerGroupID_t CurrentServerGroupID = 0;
    string logon = "";
    string access = "";
    string zipcode = "";

    string lastIP = "";
    string lastMacAddress = "";


    PayType payType;
    string payPlayDate;
    string familyPayPlayDate;
    uint payPlayHours;
    uint payPlayFlag;
    bool bAdult = false;

    try {
        ////////////////////////////////////////////////////////////


        ////////////////////////////////////////////////////////////
        bool bError = false;

        if (ID.find_first_of("'\\", 0) < ID.size())
            bError = true;
        if (PASSWORD.find_first_of("'\\", 0) < PASSWORD.size())
            bError = true;

        if (bError) {
            // cout << "Error" << endl;
            LCLoginError lcLoginError;
            lcLoginError.setErrorID(INVALID_ID_PASSWORD);
            pLoginPlayer->sendPacket(&lcLoginError);

            filelog("loginfail.txt", "Error Code: INVALID_ID_PASSWORD, 2, PlayerID : %s", pPacket->getID().c_str());
            return;
        }

        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        Result* pResult = NULL;


        // A PreparedStatement owns the Result that execute() hands back and
        // deletes it in its destructor, so the statement must outlive every use
        // of pResult. Select the query here and construct the statement in the
        // same scope as pResult: declaring it inside the branches below frees
        // the Result at the branch's closing brace and leaves pResult dangling
        // for the rest of this function (Bug 18-B).
        const char* loginSql = NULL;
        bool bBindPassword = false;

        if (bWebLogin) {
            loginSql = "SELECT PlayerID, SSN, CurrentServerGroupID, LogOn, Access, LoginIP, "
                       "PayType, PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate "
                       "FROM Player WHERE PlayerID = ?";
        } else if (bFreePass) // by sigi. 2002.10.23
        {
            loginSql = "SELECT PlayerID, CurrentServerGroupID, LogOn, Access, LoginIP, PayType, "
                       "PayPlayDate, PayPlayHours, PayPlayFlag, FamilyPayPlayDate FROM Player "
                       "WHERE PlayerID = ?";
        } else if (g_pConfig->hasKey("DB_VERSION") && g_pConfig->getProperty("DB_VERSION")[0] == '4') {
            loginSql = "SELECT PlayerID, SSN, CurrentServerGroupID, LogOn, Access, "
                       "ZipCode, LoginIP, PayType, PayPlayDate, PayPlayHours, "
                       "PayPlayFlag, FamilyPayPlayDate FROM Player WHERE PlayerID = ? "
                       "AND Password = OLD_PASSWORD(?)";
            bBindPassword = true;
        } else {
            loginSql = "SELECT PlayerID, SSN, CurrentServerGroupID, LogOn, Access, ZipCode, "
                       "LoginIP, PayType, PayPlayDate, PayPlayHours, PayPlayFlag, "
                       "FamilyPayPlayDate FROM Player WHERE PlayerID = ? AND "
                       "Password = ?";
            bBindPassword = true;
        }

        PreparedStatement loginStmt(pConn, loginSql);
        loginStmt.bindString(1, ID);
        if (bBindPassword) {
            loginStmt.bindString(2, PASSWORD);
        }
        pResult = loginStmt.execute();

        // by sigi. 2002.10.30

        bool bNoPlayer = ((pResult->getRowCount() == 0) && !bFreePass);


        if (bNoPlayer) // pResult->getRowCount() == 0)
        {
            // cout << "no Result : " << ID.c_str() << endl;

            LCLoginError lcLoginError;
            lcLoginError.setErrorID(INVALID_ID_PASSWORD);
            pLoginPlayer->sendPacket(&lcLoginError);
            filelog("loginfail.txt", "Error Code: INVALID_ID_PASSWORD, 3, PlayerID : %s", pPacket->getID().c_str());


            uint nFailed = pLoginPlayer->getFailureCount();


            if (nFailed > 3) {
                SAFE_DELETE(pStmt);
                throw DisconnectException("too many failure");
            }

            pLoginPlayer->setFailureCount(nFailed);
            pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);

            return;
        }


        else {
            int i = 0;

            if (bWebLogin) {
                pResult->next();

                pPacket->setID(pResult->getString(++i));
                ID = pPacket->getID(); // by sigi. 2002.12.21
                SSN = pResult->getString(++i);
                CurrentServerGroupID = pResult->getInt(++i);
                logon = pResult->getString(++i);
                access = pResult->getString(++i);
                zipcode = "000-000";
                lastIP = pResult->getString(++i);
                payType = (PayType)pResult->getInt(++i);
                payPlayDate = pResult->getString(++i);
                payPlayHours = pResult->getInt(++i);
                payPlayFlag = pResult->getInt(++i);
                familyPayPlayDate = pResult->getString(++i);

            } else if (bFreePass) {
                if (pResult->getRowCount() == 0) {
                    /*
                    cout << "NetMarble New Player: " << ID.c_str() << endl;

                    pStmt->executeQuery("INSERT INTO Player (PlayerID, Password, Name, SSN, Event) Values ('%s', '%s',
                    '%s', '123456-1122339', 0)", ID.c_str(), PASSWORD.c_str(), ID.c_str());

                    CurrentServerGroupID = 0;
                    logon                = "LOGOFF";
                    access               = "ALLOW";
                    zipcode				=  "000-000";
                    lastIP               = "255.255.255.255";
                    payType              = PAY_TYPE_PERIOD;
                    payPlayDate          = "2002-07-15 00:00:00";
                    payPlayHours         = 0;
                    payPlayFlag          = 0;
                    */


                    LCLoginError lcLoginError;
                    lcLoginError.setErrorID(ETC_ERROR);
                    pLoginPlayer->sendPacket(&lcLoginError);
                    pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);
                    filelog("loginfail.txt", "Error Code: ETC_ERROR, 4, PlayerID : %s", pPacket->getID().c_str());
                    SAFE_DELETE(pStmt);
                } else {
                    // cout << "NetMarble Player: " << ID.c_str() << endl;

                    pResult->next();

                    pPacket->setID(pResult->getString(++i));
                    ID = pPacket->getID(); // by sigi. 2002.12.21
                    CurrentServerGroupID = pResult->getInt(++i);
                    logon = pResult->getString(++i);
                    access = pResult->getString(++i);
                    zipcode = "000-000";
                    lastIP = pResult->getString(++i);
                    //					/*inthesky*/lastMacAddress    = pResult->getString(++i);
                    payType = (PayType)pResult->getInt(++i);
                    payPlayDate = pResult->getString(++i);
                    payPlayHours = pResult->getInt(++i);
                    payPlayFlag = pResult->getInt(++i);
                    familyPayPlayDate = pResult->getString(++i);
                }
            } else {
                // cout << "Normal Player: " << ID.c_str() << endl;

                pResult->next();

                pPacket->setID(pResult->getString(++i));
                ID = pPacket->getID(); // by sigi. 2002.12.21
                SSN = pResult->getString(++i);
                CurrentServerGroupID = pResult->getInt(++i);
                logon = pResult->getString(++i);
                access = pResult->getString(++i);
                zipcode = pResult->getString(++i);
                lastIP = pResult->getString(++i);
                //				/*inthesky*/lastMacAddress	 = pResult->getString(++i);
                payType = (PayType)pResult->getInt(++i);
                payPlayDate = pResult->getString(++i);
                payPlayHours = pResult->getInt(++i);
                payPlayFlag = pResult->getInt(++i);
                familyPayPlayDate = pResult->getString(++i);
            }

            pLoginPlayer->setServerGroupID(CurrentServerGroupID);

            //			if (access == "DENY" || access == "WAIT")
            if (access != "ALLOW") {
                LCLoginError lcLoginError;
                lcLoginError.setErrorID(ETC_ERROR);
                pLoginPlayer->sendPacket(&lcLoginError);
                pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);
                filelog("loginfail.txt", "Error Code: ETC_ERROR, 5, PlayerID : %s", pPacket->getID().c_str());
                SAFE_DELETE(pStmt);
                return;
            }

#ifdef __THAILAND_SERVER__
            // add by inthesky for THAILAND child guard rule
            bool bChildGuardArea = onChildGuardTimeArea(g_pConfig->getPropertyInt("CHILDGUARD_START_TIME"),g_pConf

			cout << "Global ChildGuard Policy : " << g_pConfig->getProperty("CHILDGUARD") << endl;
			cout << "ChildGuard Start Time : " << (int)g_pConfig->getPropertyInt("CHILDGUARD_START_TIME") << endl;
			cout << "ChildGuard End Time : " << (int)g_pConfig->getPropertyInt("CHILDGUARD_END_TIME") << endl;

			if(bChildGuardArea)     cout << "ChildGuard System : RUN" << endl;
			else                    cout << "ChildGuard System : STOP" << endl;

			if(bPermission) cout << "(" << ID << ") Permission : ALLOW" << endl;
			else            cout << "(" << ID << ") Permission : DENY" << endl;

			if (!bPermission && bChildGuardArea )
			{
                LCLoginError lcLoginError;
                lcLoginError.setErrorID(CHILDGUARD_DENYED);
                pLoginPlayer->sendPacket(&lcLoginError);
                pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);
                SAFE_DELETE(pStmt);

                return;

			}
#endif


#ifdef __PAY_SYSTEM_LOGIN__
			
			if (!pLoginPlayer->loginPayPlay(payType, 
											payPlayDate, payPlayHours, payPlayFlag,
											connectIP, ID))
			{
                LCLoginError lcLoginError;
                lcLoginError.setErrorID(NOT_PAY_ACCOUNT);
                pLoginPlayer->sendPacket(&lcLoginError);
                pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);
                filelog("loginfail.txt", "Error Code: NOT_PAY_ACCOUNT, 6, PlayerID : %s", pPacket->getID().c_str());
                SAFE_DELETE(pStmt);
                return;
			}
#elif defined(__PAY_SYSTEM_FREE_LIMIT__)

            if (pLoginPlayer->loginPayPlay(payType, payPlayDate, payPlayHours, payPlayFlag, connectIP, ID)) {
            }
#else // elif defined(__PAY_SYSTEM_ZONE__)
            pLoginPlayer->setPayPlayValue(payType, payPlayDate, payPlayHours, payPlayFlag, familyPayPlayDate);
#endif

			
			bool bSameIP = false;
			if (logon == "LOGON" || 
				logon == "GAME")
			{
                // if (logon=="LOGON" && connectIP==lastIP)
                //{
                //}

                // else


                if (logon == "LOGON" || connectIP != lastIP) // || !pPacket->checkMacAddress(lastMacAddress))
                {
                    LCLoginError lcLoginError;
                    // lcLoginError.setMessage("already connected");
                    lcLoginError.setErrorID(ALREADY_CONNECTED);
                    pLoginPlayer->sendPacket(&lcLoginError);
                    filelog("loginfail.txt", "Error Code: ALREADY_CONNECTED, 7, PlayerID : %s",
                            pPacket->getID().c_str());


                    uint nFailed = pLoginPlayer->getFailureCount();

                    if (nFailed > 3) {
                        SAFE_DELETE(pStmt);
                        throw DisconnectException("too many failure");
                    }

                    SAFE_DELETE(pStmt);
                    pLoginPlayer->setFailureCount(nFailed);
                    pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);
                    return;

                    // bSameIP = false;
                }

                else {
                    bSameIP = true;
                }
			}

			
			
			
			if (bSameIP)
			{
                if (!bFreePass || bWebLogin) // by sigi. 2002.10.23
                {
                    if (strstr(SSN.c_str(), "-") != NULL) {
                        bAdult = isAdultByBirthday(SSN.substr(0, 6));
                    }

                    else {
#ifdef __CHINA_SERVER__
                        bAdult = true;
#else
                        if (SSN.size() == 15) {
                            bAdult = isAdultByBirthday(SSN.substr(6, 12));
                        } else if (SSN.size() == 18) {
                            bAdult = isAdultByBirthday(SSN.substr(8, 14));
                        } else {
                            bAdult = false;
                        }
#endif
                    }
                }


                pLoginPlayer->setID(ID);
                pLoginPlayer->setSSN(SSN);
                pLoginPlayer->setZipcode(zipcode);


                pLoginPlayer->setAdult(bAdult);

                pLoginPlayer->sendLGKickCharacter();

                SAFE_DELETE(pStmt);
                return;
			}

			//if (logon == "LOGOFF" || bSameIP)
			if (logon == "LOGOFF"
				|| logon=="LOGON")
			{
                __BEGIN_DEBUG


                // by sigi. 2002.5.15
                PreparedStatement logOnStmt(pConn,
                                            "UPDATE Player SET LogOn = 'LOGON', LoginIP = ?, CurrentLoginServerID=?, "
                                            "LastLoginDate=now() WHERE PlayerID = ? AND LogOn='LOGOFF'");
                logOnStmt.bindString(1, connectIP);
                logOnStmt.bindInt(2, g_pConfig->getPropertyInt("LoginServerID"));
                logOnStmt.bindString(3, ID);
                logOnStmt.execute();
                int affectedRowCount = logOnStmt.getAffectedRowCount();


                // pStmt->executeQuery("UPDATE PlayerIPList SET IP1=IP2, Date1=Date2, IP2=IP3, Date2=Date3, IP3=IP4,
                // Date3=Date4, IP4=IP5, Date4=Date5, IP5='%s', Date5=now() WHERE PlayerID='%s'",connectIP.c_str(),
                // ID.c_str());

                // if (pStmt->getAffectedRowCount()==0)
                //{
                //	pStmt->executeQuery("INSERT IGNORE INTO PlayerIPList (PlayerID) Values('%s')",
                //							ID.c_str());
                // }


                if (affectedRowCount == 0) {
                    // if (pStmt->getAffectedRowCount()==0)
                    //{
                    LCLoginError lcLoginError;
                    // lcLoginError.setMessage("already connected");
                    lcLoginError.setErrorID(ALREADY_CONNECTED);
                    pLoginPlayer->sendPacket(&lcLoginError);
                    filelog("loginfail.txt", "Error Code: ALREADY_CONNECTED, 8, PlayerID : %s",
                            pPacket->getID().c_str());

                    SAFE_DELETE(pStmt);
                    pLoginPlayer->setPlayerStatus(LPS_BEGIN_SESSION);

                    return;
                    //}
                }
                /*
                }
                */

                __END_DEBUG


                pLoginPlayer->setID(ID);


                // #ifdef __CONNECT_BILLING_SYSTEM__
                //  by sigi. 2002.11.21
                // pLoginPlayer->setBillingSession();

                // pLoginPlayer->sendBillingLogin();
// #endif
#ifdef __CONNECT_CBILLING_SYSTEM__

                g_pCBillingPlayerManager->sendLogin(pLoginPlayer);
#endif


                //				pLoginPlayer->sendLGKickCharacter();


                LCLoginOK lcLoginOK;
                lcLoginOK.setFamily(false);

                if (!bFreePass || bWebLogin) // by sigi. 2002.10.23
                {
                    if (strstr(SSN.c_str(), "-") != NULL) {
                        bAdult = isAdultByBirthday(SSN.substr(0, 6));
                    }

                    else {
#ifdef __CHINA_SERVER__
                        bAdult = true;
#else
                        if (SSN.size() == 15) {
                            bAdult = isAdultByBirthday(SSN.substr(6, 12));
                        } else if (SSN.size() == 18) {
                            bAdult = isAdultByBirthday(SSN.substr(8, 14));
                        } else {
                            bAdult = false;
                        }
#endif
                    }
                }

                if (g_pConfig->getPropertyInt("IsNetMarble") == 1)
                    bAdult = pPacket->isAdult();

                lcLoginOK.setAdult(bAdult);

#ifndef __CONNECT_BILLING_SYSTEM__

#ifdef __CHINA_SERVER__
                lcLoginOK.setLastDays(0xffff);
#else

                //				if (pLoginPlayer->getPayType() == 0 || pLoginPlayer->isFamilyPayAvailable() )
                if (pLoginPlayer->getPayType() == 0)
                    lcLoginOK.setLastDays(0xfffe);
                else {
                    int lastDays = VSDateTime::currentDateTime().daysTo(pLoginPlayer->getPayPlayAvailableDateTime());
                    int lastSecs = VSDateTime::currentDateTime().secsTo(pLoginPlayer->getPayPlayAvailableDateTime());

                    int familyLastDays =
                        VSDateTime::currentDateTime().daysTo(pLoginPlayer->getFamilyPayPlayAvailableDateTime());
                    int familyLastSecs =
                        VSDateTime::currentDateTime().secsTo(pLoginPlayer->getFamilyPayPlayAvailableDateTime());

                    if (lastSecs < 0 && familyLastSecs < 0) {
                        if (pLoginPlayer->getPayPlayAvailableHours() > 0)
                            lcLoginOK.setLastDays(0xfffe);
                        else
                            lcLoginOK.setLastDays(0xfffe);
                    } else {
                        if (lastSecs < familyLastSecs) {
                            lcLoginOK.setFamily(true);
                            lcLoginOK.setLastDays(familyLastDays);

                            cout << "Family " << endl;
                        } else {
                            lcLoginOK.setFamily(false);
                            lcLoginOK.setLastDays(lastDays);
                            cout << "Premium " << endl;
                        }
                    }
                }


                if (lcLoginOK.getLastDays() > 1000)
                    filelog("PayPlayDateLog.txt", "UserID : %s , LastDays : %ld", ID.c_str(), lcLoginOK.getLastDays());


                {
                    PreparedStatement eventStmt(pConn, "SELECT PlayerID FROM Event200501Main WHERE PlayerID = ? AND "
                                                       "RecvPremiumDate = '0000-00-00'");
                    eventStmt.bindString(1, pLoginPlayer->getID());
                    // Scoped to eventStmt, which owns this Result and frees it at the
                    // closing brace below. Reassigning the function-scope pResult here
                    // would leave it dangling for the rest of execute() -- exactly the
                    // shape of Bug 18-B, which this same function used to have.
                    Result* pEventResult = eventStmt.execute();

                    if (pEventResult->next()) {
                        PreparedStatement payPlayDateStmt(pConn,
                                                          "UPDATE Player SET PayPlayDate = IF (PayPlayDate < NOW(), "
                                                          "NOW() + INTERVAL 7 DAY, PayPlayDate + INTERVAL 7 DAY ) "
                                                          "WHERE PlayerID = ?");
                        payPlayDateStmt.bindString(1, pLoginPlayer->getID());
                        payPlayDateStmt.execute();


                        PreparedStatement recvPremiumStmt(pConn, "UPDATE Event200501Main SET RecvPremiumDate = NOW() "
                                                                 "WHERE PlayerID = ?");
                        recvPremiumStmt.bindString(1, pLoginPlayer->getID());
                        recvPremiumStmt.execute();


                        lcLoginOK.setLastDays(0xfffd);
                    }
                }
#endif
#endif

#ifdef __NETMARBLE_SERVER__

                PreparedStatement agreementStmt(pConn,
                                                "SELECT PlayerID FROM PrivateAgreementRemain WHERE PlayerID = ?");
                agreementStmt.bindString(1, pLoginPlayer->getID());
                pResult = agreementStmt.execute();

                if (pResult->next()) {
                    pLoginPlayer->setAgree(false);
                    cout << "false - " << pLoginPlayer->getID() << endl;
                } else {
                    pLoginPlayer->setAgree(true);
                    cout << "true - " << pLoginPlayer->getID() << endl;
                }
#endif

                pLoginPlayer->sendPacket(&lcLoginOK);
                pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);
			}
        } // end of if (pResult->getRowCount() == 0) else

        SAFE_DELETE(pStmt);
    } catch (SQLQueryException& sqe) {
        SAFE_DELETE(pStmt);
        throw Error(sqe.toString());
    }


    addLoginPlayerData(ID, connectIP, SSN, zipcode);

#endif

    __END_DEBUG_EX __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////////////
bool isAdultByBirthday(const string& birthday) {
    StringStream AdultSSN;

    time_t daytime = time(0);
    tm Timec;
    localtime_r(&daytime, &Timec);

    AdultSSN << Timec.tm_year - 18;
    // tm_mon - range 0 to 11
    if ((Timec.tm_mon + 1) < 10)
        AdultSSN << "0";
    AdultSSN << (Timec.tm_mon + 1);
    if (Timec.tm_mday < 10)
        AdultSSN << "0";
    AdultSSN << Timec.tm_mday;

    // cout << "SSN = " << birthday.c_str() << " ADULTSSN = " << AdultSSN.toString().c_str() << endl;


    if (atoi(birthday.c_str()) <= atoi(AdultSSN.toString().c_str())) {
        return true;
    }


    return false;
}


//////////////////////////////////////////////////////////////////////////////
//
// add LoginPlayerdata
//


//
//////////////////////////////////////////////////////////////////////////////
void addLoginPlayerData(const string& ID, const string& ip, const string& SSN, const string& zipcode) {
#ifdef __LOGIN_SERVER__

    Statement* pStmt2 = NULL;


    // UPDATE Player Set LoginFlagDay=1, LoginFlagWeek=1, LoginFlagMonth=1 WHERE PlayerID='%s'


    BEGIN_DB {
        int year, month, day, hour, minute, second;
        getCurrentTimeEx(year, month, day, hour, minute, second);
        string currentDT = VSDateTime::currentDateTime().toDateTime();

        /*
        StringStream sql;
        sql << "INSERT INTO LoginPlayerData (Year,Month,Day,Hour,Minute,Second,PlayerID,SSN,ZipCode,Date,Time) VALUES ("
            << year << "," << month << "," << day << "," << hour << "," << minute << "," << second << ",'"
            << ID << "','" << SSN << "','" << zipcode << "','"
            << currentDT.substr(0, 10 ).c_str() << "','" << currentDT.substr(11 ).c_str() << "')";
        */

        Connection* pConn = g_pDatabaseManager->getUserInfoConnection();
        PreparedStatement insertLoginPlayerDataStmt(
            pConn, "INSERT INTO LoginPlayerData (PlayerID,IP,Date,Time) VALUES (?, ?, ?, ?)");
        insertLoginPlayerDataStmt.bindString(1, ID);
        insertLoginPlayerDataStmt.bindString(2, ip);
        insertLoginPlayerDataStmt.bindString(3, currentDT.substr(0, 10));
        insertLoginPlayerDataStmt.bindString(4, currentDT.substr(11));
        insertLoginPlayerDataStmt.execute();
    }
    END_DB(pStmt2)

#endif
}

bool CLLoginHandler::checkNetMarbleClient(CLLogin* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX
#ifdef __LOGIN_SERVER__

        bool isNetmarble = pPacket->isNetmarble();

    if (isNetmarble) // by sigi. 2002.10.23
    {
        LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

        bool bFreePass = checkFreePass(pPacket, pPlayer);

        if (!bFreePass) {
            LCLoginError lcLoginError;
            lcLoginError.setErrorID(INVALID_ID_PASSWORD);
            pLoginPlayer->sendPacket(&lcLoginError);
            filelog("loginfail.txt", "Error Code: INVALID_ID_PASSWORD, 9, PlayerID : %s", pPacket->getID().c_str());

            return false;
        }


        pLoginPlayer->setFreePass(true);


        // cout << "NetMarble Login OK" << endl;
    }

#endif
    __END_DEBUG_EX __END_CATCH

        return true;
}


bool CLLoginHandler::checkFreePass(CLLogin* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    // LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);


    Statement* pStmt = NULL;

    try {
        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
            Result* pResult = NULL;

            PreparedStatement passwordStmt(pConn, "SELECT Password FROM Player WHERE PlayerID = ?");
            passwordStmt.bindString(1, pPacket->getID());
            pResult = passwordStmt.execute();

            if (pResult->next()) {
                string password = pResult->getString(1);

                if (password == pPacket->getPassword()) {
                    // cout << "password OK" << endl;
                    return true;
                } else {
                    // By tiancaiamao: for debug, comment it!!!
                    cout << "password wrong: " << password << " != " << pPacket->getPassword().c_str() << endl;
                }
            } else {
                // cout << "ID wrong: " << pPacket->getID().c_str() << endl;


                cout << "NetMarble New Player: " << pPacket->getID().c_str() << endl;


                // 2003.04.30 by bezz, DEW
                PreparedStatement newPlayerStmt(pConn, "INSERT IGNORE INTO Player (PlayerID, Password, Name, SSN, "
                                                       "SpecialEventCount, Event, creation_date) Values (?, ?, ?, "
                                                       "'123456-1122339', 2, 0, CURDATE())");
                newPlayerStmt.bindString(1, pPacket->getID());
                newPlayerStmt.bindString(2, pPacket->getPassword());
                newPlayerStmt.bindString(3, pPacket->getID());
                newPlayerStmt.execute();

                // string  connectIP  = pPlayer->getSocket()->getHost();

                // pStmt->executeQuery("INSERT IGNORE INTO PlayerIPList (PlayerID) Values('%s')",
                //						pPacket->getID().c_str());


                return true;
            }
        }
        END_DB(pStmt)
    } catch (Throwable& t) {
        return false;
    }


#endif
    __END_DEBUG_EX __END_CATCH

        return false;
}

bool isBlockIP(const string& ip) {
#ifdef __LOGIN_SERVER__

    Statement* pStmt = NULL;

    BEGIN_DB {
        size_t i = ip.find_first_of('.', 0);
        size_t j = ip.find_first_of('.', i + 1);
        size_t k = ip.find_first_of('.', j + 1);

        /*
         * ip = 61.78.53.228
         * classA = 61
         * classB = 61.78
         * classC = 61.78.53
         */
        string classA = ip.substr(0, i);
        string classB = ip.substr(0, j);
        string classC = ip.substr(0, k);
        //		int classD    = atoi(ip.substr(k+1, ip.size()-k-1).c_str());

        PreparedStatement ipBlockStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                      "SELECT class, first, last FROM IPBlockInfo WHERE	(IP = ? AND "
                                      "class=1) OR (IP = ? AND class=2) OR (IP = ?)");
        ipBlockStmt.bindString(1, classA);
        ipBlockStmt.bindString(2, classB);
        ipBlockStmt.bindString(3, classC);
        Result* pResult = ipBlockStmt.execute();

        while (pResult->next()) {
            int ipClass = pResult->getInt(1);
            int first = pResult->getInt(2);
            int last = pResult->getInt(3);
            int index;

            switch (ipClass) {
            case 0:
                index = atoi(ip.substr(k + 1, ip.size() - k - 1).c_str());
                break;

            case 1:
                index = atoi(ip.substr(i + 1, j - i - 1).c_str());
                break;

            case 2:
                index = atoi(ip.substr(j + 1, k - j - 1).c_str());
                break;
            default:
                index = -1;
                break;
            }

            if (index < 0)
                return true;

            if (index >= first && index <= last)
                return true;

            /*			if (ipClass == 0 )
                        {
                            if (classD >= first && classD <= last )
                            {
                                return true;
                            }
                        }
                        else
                        {
                            return true;
                        }*/
        }
    }
    END_DB(pStmt)

    return false;

#endif
}

bool CLLoginHandler::checkWebLogin(CLLogin* pPacket, Player* pPlayer) {
    __BEGIN_TRY

#ifdef __LOGIN_SERVER__

    Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

    Statement* pStmt = NULL;

    try {
        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
            Result* pResult = NULL;

            PreparedStatement webLoginKeyStmt(pConn,
                                              "SELECT LoginKey, CreateTime, now() FROM WebLogin WHERE PlayerID = ?");
            webLoginKeyStmt.bindString(1, pPacket->getID());
            pResult = webLoginKeyStmt.execute();

            if (pResult->next()) {
                string key = pResult->getString(1);
                VSDateTime vsCreate(pResult->getString(2));
                VSDateTime vsNow(pResult->getString(3));

                // cout << "PlayerID: " << pPacket->getID().c_str() << endl;
                // cout << "*DB)     Key: " << key.c_str() << ", create: " << vsCreate.toString().c_str() << ", now:" <<
                // vsNow.toString().c_str() << endl; cout << "*Packet) Key: " << pPacket->getPassword() << endl;

                // check key
                if (key != pPacket->getPassword()) {
                    LCLoginError lcLoginError;
                    lcLoginError.setErrorID(INVALID_ID_PASSWORD);
                    pLoginPlayer->sendPacket(&lcLoginError);
                    filelog("loginfail.txt", "Error Code: INVALID_ID_PASSWORD, 10, PlayerID : %s",
                            pPacket->getID().c_str());
                    filelog("keydiff.txt", "db key: %s, packet key: %s, Player ID: %s", key.c_str(),
                            pPacket->getPassword().c_str(), pPacket->getID().c_str());

                    cout << "33333" << endl;
                    return false;
                }

                // check date time.
                if (vsCreate.secsTo(vsNow) > 300) {
                    LCLoginError lcLoginError;
                    lcLoginError.setErrorID(KEY_EXPIRED);
                    pLoginPlayer->sendPacket(&lcLoginError);
                    filelog("loginfail.txt", "Error Code: KEY_EXPIRED, 12, PlayerID : %s", pPacket->getID().c_str());

                    return false;
                }


                pLoginPlayer->setFreePass(true);


                PreparedStatement deleteWebLoginStmt(pConn, "DELETE FROM WebLogin WHERE PlayerID = ?");
                deleteWebLoginStmt.bindString(1, pPacket->getID());
                deleteWebLoginStmt.execute();
            } else {
                LCLoginError lcLoginError;
                lcLoginError.setErrorID(NOT_FOUND_KEY);
                pLoginPlayer->sendPacket(&lcLoginError);
                filelog("loginfail.txt", "Error Code: NOT_FOUND_KEY, 11, PlayerID : %s", pPacket->getID().c_str());

                // cout << "4" << endl;
                return false;
            }
        }
        END_DB(pStmt)
    } catch (Throwable& t) {
        // cout << "5" << endl;
        return false;
    }

#endif
    __END_CATCH

    return true;
}

#ifdef __THAILAND_SERVER__
bool CLLoginHandler::onChildGuardTimeArea(int pm, int am, string enable) {
    bool returnValue = false;
    tm Timem;
    time_t daytime = time(0);
    localtime_r(&daytime, &Timem);

    int Hour = Timem.tm_hour;
    int Min = Timem.tm_min;

    int timeValue = (Hour * 100) + Min;
    bool bSwitch = (enable == "ENABLE" || enable == "enable" || enable == "Enable");

    if ((timeValue >= pm && timeValue <= am) && bSwitch) {
        returnValue = true;
    } else if ((timeValue <= pm && timeValue <= am) && bSwitch) {
        if (am > 1200)
            returnValue = false;
        else
            returnValue = true;
    } else if ((timeValue <= pm && timeValue <= am) && bSwitch) {
        returnValue = false;
    } else if ((timeValue >= pm && timeValue >= am) && bSwitch) {
        if (am > 1200)
            returnValue = false;
        else
            returnValue = true;
    }


    return returnValue;
}
#endif
