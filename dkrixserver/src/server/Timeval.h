//--------------------------------------------------------------------------------
//
// Filename    : Timeval.h
// Written By  : Reiot

//
//--------------------------------------------------------------------------------

#ifndef __TIMEVAL_H__
#define __TIMEVAL_H__

// include files
#include <unistd.h>

#include <sys/time.h>

// type redefinition
typedef struct timeval Timeval;

extern Timeval gCurrentTime;

#ifdef __GAME_SERVER__
#define getCurrentTime(t) t = gCurrentTime
#define setCurrentTime() gettimeofday(&gCurrentTime, NULL)
#else
#define getCurrentTime(t) gettimeofday((&t), NULL)
#endif

bool operator>(const Timeval& left, const Timeval& right);
bool operator>=(const Timeval& left, const Timeval& right);
bool operator==(const Timeval& left, const Timeval& right);
bool operator<=(const Timeval& left, const Timeval& right);
bool operator<(const Timeval& left, const Timeval& right);



Timeval operator+(const Timeval& left, const Timeval& right);

//

//
Timeval timediff(const Timeval& left, const Timeval& right);

void getCurrentYearTime(unsigned int& currentYearTime);

#endif
