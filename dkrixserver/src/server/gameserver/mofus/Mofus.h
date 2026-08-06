/////////////////////////////////////////////////////////////////////////////
// Filename : Mofus.h

/////////////////////////////////////////////////////////////////////////////

#include "Types.h"


#if defined(__METRO_SERVER__)
#define __MOFUS__
#endif


#define MOFUS_ERROR_FILE "mofus_error.txt"
#define MOFUS_LOG_FILE "mofus_log.txt"
#define MOFUS_PACKET_FILE "mofus_packet.txt"

int loadPowerPoint(const string& name);
int savePowerPoint(const string& name, int amount);
void logPowerPoint(const string& name, int recvPoint, int savePoint);
