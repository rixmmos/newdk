#ifndef __BILLING_INFO_H__
#define __BILLING_INFO_H__

#include <string>
using namespace std;

#include "types/ServerType.h"

#ifdef __NETMARBLE_SERVER__
#define __CONNECT_BILLING_SYSTEM__
#endif
// #define __COUT_BILLING_SYSTEM__

#ifdef __GAME_SERVER__
#define LOGFILE_BILLING_PLAYER "billingPlayerGame.txt"
#define LOGFILE_BILLING_PACKET "billingPacketGame.txt"
#elif defined(__LOGIN_SERVER__)
#define LOGFILE_BILLING_PLAYER "billingPlayerLogin.txt"
#define LOGFILE_BILLING_PACKET "billingPacketLogin.txt"
#else
#define LOGFILE_BILLING_PLAYER "billingPlayer.txt"
#define LOGFILE_BILLING_PACKET "billingPacker.txt"
#endif

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
enum BillingPacketType {
    BILLING_PACKET_LOGIN = 0x00000001,       
    BILLING_PACKET_LOGOUT = 0x00000002,      
    BILLING_PACKET_POLLING = 0x00000003,     
    BILLING_PACKET_INIT = 0x00000004,        
    BILLING_PACKET_REMAIN = 0x00000005,      
    BILLING_PACKET_CHECK = 0x00000006,       
    BILLING_PACKET_LOGIN_CHECK = 0x00000008, 

    BILLING_PACKET_MAX
};

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
enum BillingResultLogin {
    BILLING_RESULT_LOGIN_OK = 0,              
    BILLING_RESULT_LOGIN_DB_ERROR = 2,        
    BILLING_RESULT_LOGIN_NETWORK_ERROR = 3,   
    BILLING_RESULT_LOGIN_NO_ACCOUNT = 10,     
    BILLING_RESULT_LOGIN_NO_CASH = 11,        
    BILLING_RESULT_LOGIN_NO_SESSION = 12,     
    BILLING_RESULT_LOGIN_BAD_PACKET = 20,     
    BILLING_RESULT_LOGIN_COM_ERROR = 25,      // COM Error
    BILLING_RESULT_LOGIN_NO_RESPONSE = 26,    
    BILLING_RESULT_LOGIN_NO_MACHINE = 27,     
    BILLING_RESULT_LOGIN_BAD_GAME_NO = 30,    
    BILLING_RESULT_LOGIN_ACCOUNT_ERROR = 31,  
                                              
    BILLING_RESULT_LOGIN_DENY = 33,           
    BILLING_RESULT_LOGIN_TIME_OVER = 34,      
    BILLING_RESULT_LOGIN_BUSY = 35,           
    BILLING_RESULT_LOGIN_UNKNOWN_ERROR = 100, 
    BILLING_RESULT_LOGIN_IP_COM_ERROR = 200,  
    BILLING_RESULT_LOGIN_IP_ERROR = 201,      
    BILLING_RESULT_LOGIN_KEY_COM_ERROR = 210, 
    BILLING_RESULT_LOGIN_NO_KEY = 211,        

    BILLING_RESULT_LOGIN_MAX
};

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
enum BillingResultRemain {
    BILLING_RESULT_REMAIN_DISCONNECT = -4, 
    BILLING_RESULT_REMAIN_RESERVE = -3,    
    BILLING_RESULT_REMAIN_DUPLICATE = -2,  
    BILLING_RESULT_REMAIN_REFUND = -1,     
    BILLING_RESULT_REMAIN_NONE = 0,        
    BILLING_RESULT_REMAIN_TIME = 1,        

    BILLING_RESULT_REMAIN_MAX
};

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
enum BillingResultCheck {
    BILLING_RESULT_CHECK_PLAYING = 0,      
    BILLING_RESULT_CHECK_DISCONNECTED = 1, 

    BILLING_RESULT_CHECK_MAX
};

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
enum BillingUserCC {
    BILLING_USER_METROTECH, 
    BILLING_USER_NETMARBLE, 

    BILLING_USER_MAX
};

const string BillingUserCC2String[BILLING_USER_MAX] = {
    "DE", 
    "NM"  
};

////////////////////////////////////////////////////////////////////////////////
// BillingInfo == _AUTH_GAME
////////////////////////////////////////////////////////////////////////////////
struct _AUTH_GAME {
    mutable int Packet_Type; 
    mutable int Result;      
    char S_KEY[32];          
    char Session[32];        // guid
    char User_CC[4];         
    char User_No[20];        
    char User_ID[40];        
    char User_IP[24];        
    char User_Gender;        
    char User_Status[3];     
    char User_PayType[2];    
    mutable int User_Age;    
    mutable int Game_No;     
    char Bill_PayType[2];    
    char Bill_Method[2];     
    char Expire_Date[12];    
    mutable int Remain_Time; 
};

typedef _AUTH_GAME BillingInfo;

const int szBillingInfo = sizeof(BillingInfo);

#endif
