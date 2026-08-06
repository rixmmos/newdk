/////////////////////////////////////////////////////////////////////////////
// filename	: MPacketID.h
/////////////////////////////////////////////////////////////////////////////

#ifndef __MPACKET_ID_H__
#define __MPACKET_ID_H__

// MPacketID send enum
enum {
    PTC_CONNECT_ASK = 0x01, 
    PTC_LOGOUT = 0x11,      
    PTC_USERINFO = 0x20,    
    PTC_RECEIVE_OK = 0x30,  
    PTC_RESULT = 0x40,      
    PTC_ERROR = 0xFF,       

    PTC_SEND_MAX
};

// MPacketID recv enum
enum {
    PTS_CONNECT_ACCEPT = 0x01, 
    PTS_POWERPOINT = 0x20,     
    PTS_ERROR = 0xFF,          

    PTC_MAX
};

#endif
