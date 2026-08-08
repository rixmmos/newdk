//////////////////////////////////////////////////////////////////////////////
// Filename    : CLRegisterPlayerHandler.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLRegisterPlayer.h"

#ifdef __LOGIN_SERVER__
#include "Assert1.h"
#include "DB.h"
#include "GameServerGroupInfoManager.h"
#include "LCRegisterPlayerError.h"
#include "LCRegisterPlayerOK.h"
#include "LoginPlayer.h"
#include "PreparedStatement.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// When a client requests player registration, first verify the temporary
// login ID is "guest", validate the registration packet, then insert into
// the DB and respond.
//////////////////////////////////////////////////////////////////////////////
void CLRegisterPlayerHandler::execute(CLRegisterPlayer* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    __BEGIN_DEBUG

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

    // cout << "Registering Player... " << endl;

    //----------------------------------------------------------------------
    // Ensure the login user ID is "guest".
    //----------------------------------------------------------------------
    //	if (pLoginPlayer->getID() != "guest")
    //		throw InvalidProtocolException("must be guest user");

    //----------------------------------------------------------------------
    // Validate player profile fields; use NULL checks for each string.
    //----------------------------------------------------------------------
    LCRegisterPlayerError lcRegisterPlayerError;

    try {
        // cout << "Player registration : " << pPacket->toString() << endl;

        if (pPacket->getID() == "") {
            lcRegisterPlayerError.setErrorID(EMPTY_ID);
            throw string("ID field is empty");
        }

        if (pPacket->getID().size() < 4) {
            lcRegisterPlayerError.setErrorID(SMALL_ID_LENGTH);
            throw string("too small ID length");
        }

        if (pPacket->getPassword() == "") {
            lcRegisterPlayerError.setErrorID(EMPTY_PASSWORD);
            throw string("Password field is empty");
        } else {
            string password = pPacket->getPassword();
            if (password.find("\'") < password.size())
                throw string("Invalid Password");
            if (password.find("'") < password.size())
                throw string("Invalid Password");
            else if (password.find("\\") < password.size())
                throw string("Invalid Password");
            else if (password.find("\"") < password.size())
                throw string("Invalid Password");
            else if (password.find(";") < password.size())
                throw string("Invalid Password");
        }

        if (pPacket->getPassword().size() < 6) {
            lcRegisterPlayerError.setErrorID(SMALL_PASSWORD_LENGTH);
            throw string("too small password length");
        }

        if (pPacket->getName() == "") {
            lcRegisterPlayerError.setErrorID(EMPTY_NAME);
            throw string("Name field is empty");
        }

        if (pPacket->getSSN() == "") {
            lcRegisterPlayerError.setErrorID(EMPTY_SSN);
            throw string("SSN field is empty");
        }

    } catch (string& errstr) {
        pLoginPlayer->sendPacket(&lcRegisterPlayerError);

        // cout << lcRegisterPlayerError.toString() << endl;

        // For now disconnect the client on validation failure.
        // *TODO* Allow guest to retry without full disconnect.
        throw DisconnectException(lcRegisterPlayerError.toString());
    }


    //----------------------------------------------------------------------
    // Insert into the database.
    //----------------------------------------------------------------------

    Statement* pStmt = NULL;
    Result* pResult;

    try {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        //--------------------------------------------------------------------------------
        // Check for duplicate PlayerID.
        //--------------------------------------------------------------------------------
        PreparedStatement selectPlayerIDStmt(pConn, "SELECT PlayerID FROM Player WHERE PlayerID = ?");
        selectPlayerIDStmt.bindString(1, pPacket->getID());
        pResult = selectPlayerIDStmt.execute();

        if (pResult->getRowCount() != 0) {
            lcRegisterPlayerError.setErrorID(ALREADY_REGISTER_ID);
            throw DuplicatedException("   .");
        }


        //--------------------------------------------------------------------------------
        // Validate SSN (placeholder).
        //--------------------------------------------------------------------------------


        //--------------------------------------------------------------------------------
        // Check for duplicate SSN.
        //--------------------------------------------------------------------------------
        PreparedStatement selectSSNStmt(pConn, "SELECT SSN FROM Player WHERE SSN = ?");
        selectSSNStmt.bindString(1, pPacket->getSSN());
        pResult = selectSSNStmt.execute();

        if (pResult->getRowCount() != 0) {
            lcRegisterPlayerError.setErrorID(ALREADY_REGISTER_SSN);
            throw DuplicatedException("  .");
        }


        //--------------------------------------------------------------------------------
        // Insert the new player row.
        //--------------------------------------------------------------------------------
        PreparedStatement insertPlayerStmt(
            pConn, "INSERT INTO Player (PlayerID , Password , Name , Sex , SSN , Telephone , Cellular , Zipcode , "
                   "Address , Nation , Email , Homepage , Profile , Pub) VALUES (? , PASSWORD(?) , ? , ? , ? , ? , "
                   "? , ? , ? , ? , ? , ? , ? , ?)");
        insertPlayerStmt.bindString(1, pPacket->getID());
        insertPlayerStmt.bindString(2, pPacket->getPassword());
        insertPlayerStmt.bindString(3, pPacket->getName());
        insertPlayerStmt.bindString(4, Sex2String[pPacket->getSex()]);
        insertPlayerStmt.bindString(5, pPacket->getSSN());
        insertPlayerStmt.bindString(6, pPacket->getTelephone());
        insertPlayerStmt.bindString(7, pPacket->getCellular());
        insertPlayerStmt.bindString(8, pPacket->getZipCode());
        insertPlayerStmt.bindString(9, pPacket->getAddress());
        insertPlayerStmt.bindInt(10, (int)pPacket->getNation());
        insertPlayerStmt.bindString(11, pPacket->getEmail());
        insertPlayerStmt.bindString(12, pPacket->getHomepage());
        insertPlayerStmt.bindString(13, pPacket->getProfile());
        insertPlayerStmt.bindString(14, (pPacket->getPublic() == true) ? "PUBLIC" : "PRIVATE");
        pResult = insertPlayerStmt.execute();

        // After successful insert, send LCRegisterPlayerOK to the client.
        Assert(pResult == NULL);
        Assert(insertPlayerStmt.getAffectedRowCount() == 1);

        // Retrieve current world/group IDs for the new user.
        PreparedStatement selectWorldGroupStmt(
            pConn, "SELECT CurrentWorldID, CurrentServerGroupID FROM Player WHERE PlayerID = ?");
        selectWorldGroupStmt.bindString(1, pPacket->getID());
        pResult = selectWorldGroupStmt.execute();

        if (pResult->getRowCount() == 0) {
            lcRegisterPlayerError.setErrorID(ETC_ERROR);
            throw SQLQueryException("  ,   .");
        }

        WorldID_t WorldID = 0;
        ServerGroupID_t ServerGroupID = 0;

        if (pResult->next())
            throw SQLQueryException("   .");

        WorldID = pResult->getInt(1);
        ServerGroupID = pResult->getInt(2);

        pLoginPlayer->setServerGroupID(ServerGroupID);

        LCRegisterPlayerOK lcRegisterPlayerOK;

        lcRegisterPlayerOK.setGroupName(
            g_pGameServerGroupInfoManager->getGameServerGroupInfo(ServerGroupID, WorldID)->getGroupName());

        string SSN = pPacket->getSSN();
        string preSSN;
        bool isChina = false;
        // Country-specific handling (e.g., China SSN) follows.
        if (strstr(SSN.c_str(), "-") != NULL) {
            preSSN = SSN.substr(0, 6);
        }

        else {
            isChina = true;
            if (SSN.size() == 15) {
                preSSN = SSN.substr(6, 12);
            } else if (SSN.size() == 18) {
                preSSN = SSN.substr(8, 14);
            } else {
                preSSN = SSN.substr(0, 6);
            }
        }
        //        string preSSN = pPacket->getSSN().substr(0, 6).c_str();

        StringStream AdultSSN;

        time_t daytime = time(0);
        tm Timec;
        localtime_r(&daytime, &Timec);
        AdultSSN << Timec.tm_year - 20 << Timec.tm_mon << Timec.tm_mday;

        // Verify SSN indicates adult or minor.
        if (atoi(preSSN.c_str()) <= atoi(AdultSSN.toString().c_str())) {
            lcRegisterPlayerOK.setAdult(true);
        } else {
            lcRegisterPlayerOK.setAdult(false);
        }


        if (isChina) {
            lcRegisterPlayerOK.setAdult(true);
        }

        pLoginPlayer->sendPacket(&lcRegisterPlayerOK);


        pLoginPlayer->setID(pPacket->getID());


        pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);

        SAFE_DELETE(pStmt);
    } catch (DuplicatedException& de) {
        SAFE_DELETE(pStmt);

        // cout << de.toString() << endl;

        //--------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------
        pLoginPlayer->sendPacket(&lcRegisterPlayerError);

        //--------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------
        uint nFailed = pLoginPlayer->getFailureCount();

        // cout << pLoginPlayer->getID() << "'s Failure Count = " << ++nFailed << endl;

        if (nFailed > 3)
            throw DisconnectException("too many failure");

        pLoginPlayer->setFailureCount(nFailed);


        pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_REGISTER_PLAYER);

    } catch (SQLQueryException& sqe) {
        SAFE_DELETE(pStmt);


        // cout << sqe.toString() << endl;

        //--------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------
        lcRegisterPlayerError.setErrorID(ETC_ERROR);

        pLoginPlayer->sendPacket(&lcRegisterPlayerError);

        //--------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------
        uint nFailed = pLoginPlayer->getFailureCount();

        // cout << pLoginPlayer->getID() << "'s Failure Count = " << ++nFailed << endl;

        if (nFailed > 3)
            throw DisconnectException("too many failure");

        pLoginPlayer->setFailureCount(nFailed);


        pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_REGISTER_PLAYER);
    }

    __END_DEBUG

#endif

    __END_DEBUG_EX __END_CATCH
}
