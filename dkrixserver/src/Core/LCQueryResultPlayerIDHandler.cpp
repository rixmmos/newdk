//--------------------------------------------------------------------------------
//
// Filename    : LCQueryResultPlayerIDHandler.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "LCQueryResultPlayerID.h"

#ifdef __GAME_CLIENT__
#include "CLQueryPlayerID.h"
#include "CLRegisterPlayer.h"
#include "ClientPlayer.h"
#endif

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void LCQueryResultPlayerIDHandler::execute(LCQueryResultPlayerID* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_CLIENT__

        ClientPlayer* pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

#if __LINUX__

    if (pPacket->isExist())
        cout << pPacket->getPlayerID() << " is exist" << endl;
    else
        cout << pPacket->getPlayerID() << " is not exist" << endl;

    string cmd;

    do {
        cout << endl
             << "+------------------------+" << endl
             << "| 1. query player id     |" << endl
             << "| 2. register new player |" << endl
             << "+------------------------+" << endl
             << endl
             << "select > ";

        getline(cin, cmd);

    } while (cmd != "1" && cmd != "2");

    if (cmd == "1") {
        //----------------------------------------------------------------------
        
        //----------------------------------------------------------------------
        cout << "+----------+" << endl << "| QUERY ID |" << endl << "+----------+" << endl << endl << "Query> ";

        string id;
        getline(cin, id);

        CLQueryPlayerID clQueryPlayerID;
        clQueryPlayerID.setPlayerID(id);

        pClientPlayer->sendPacket(&clQueryPlayerID);

        
        pClientPlayer->setPlayerStatus(CPS_AFTER_SENDING_CL_QUERY_PLAYER_ID);

    } else {
        //----------------------------------------------------------------------
        
        //----------------------------------------------------------------------
        CLRegisterPlayer clRegisterPlayer;

        cout << "+---------------------+" << endl
             << "| REGISTER NEW PLAYER |" << endl
             << "+---------------------+" << endl
             << endl;

        cout << " : ";
        string id;
        getline(cin, id);
        clRegisterPlayer.setID(id);

        cout << " : ";
        string password;
        getline(cin, password);
        clRegisterPlayer.setPassword(password);

        cout << " : ";
        string name;
        getline(cin, name);
        clRegisterPlayer.setName(name);

        cout << " (/) : ";
        string _sex;
        getline(cin, _sex);
        Sex sex = (_sex == "") ? MALE : FEMALE;
        clRegisterPlayer.setSex(sex);

        cout << " : ";
        string ssn;
        getline(cin, ssn);
        clRegisterPlayer.setSSN(ssn);

        clRegisterPlayer.setTelephone("02-222-3333");
        clRegisterPlayer.setCellular("011-222-3333");
        clRegisterPlayer.setZipCode("700-441");
        clRegisterPlayer.setAddress("    300");
        clRegisterPlayer.setNation(KOREA);
        clRegisterPlayer.setEmail("jhkim@mbc.com");
        clRegisterPlayer.setHomepage("www.jhkim.com");
        clRegisterPlayer.setProfile("  ");
        clRegisterPlayer.setPublic(false);

        // cout << clRegisterPlayer.toString() << endl;

        
        pClientPlayer->sendPacket(&clRegisterPlayer);

        
        pClientPlayer->setPlayerStatus(CPS_AFTER_SENDING_CL_REGISTER_PLAYER);
    }

#endif

#endif

    __END_DEBUG_EX __END_CATCH
}
