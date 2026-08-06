//////////////////////////////////////////////////////////////////////////////
// Filename    : Utility.cpp

// Description :

//////////////////////////////////////////////////////////////////////////////

#include "Utility.h"

#include <stdio.h>
#include <stdlib.h>

#include <cstdarg>

#include "Assert.h"
#include "VSDateTime.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/*
VSRect::VSRect(int l, int t, int r, int b)
{
    Assert(l <= r && t <= b);

    left   = l;
    top    = t;
    right  = r;
    bottom = b;
}

bool VSRect::ptInRect(const POINT& pt) const
{
    if(left <= pt.x && pt.x <= right && top <= pt.y && pt.y <= bottom) return true;
    return false;
}

bool VSRect::ptInRect(const int x, const int y) const
{
    if(left <= x && x <= right && top <= y && y <= bottom) return true;
    return false;
}

void VSRect::set(int l, int t, int r, int b)
{
    Assert(l <= r && t <= b);

    left   = l;
    top    = t;
    right  = r;
    bottom = b;
}
*/

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
string itos(int value) {
    char buf[100] = {
        0,
    };
    sprintf(buf, "%d", value);
    return string(buf);
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
#define BASIS_DIRECTION_HIGH 2.0f
#define BASIS_DIRECTION_LOW 0.5f
Dir_t computeDirection(int originX, int originY, int destX, int destY) {
    int stepX = destX - originX;
    int stepY = destY - originY;
    float k = (stepX == 0) ? 0 : (float)(stepY) / stepX;

    if (stepY == 0) {
        if (stepX == 0)
            return DOWN;
        else if (stepX > 0)
            return RIGHT;
        else
            return LEFT;
    } else if (stepY < 0) {
        if (stepX == 0) {
            return UP;
        } else if (stepX > 0) {
            if (k < -BASIS_DIRECTION_HIGH)
                return UP;
            else if (k < -BASIS_DIRECTION_LOW)
                return RIGHTUP;
            else
                return RIGHT;
        } else {
            if (k > BASIS_DIRECTION_HIGH)
                return UP;
            else if (k > BASIS_DIRECTION_LOW)
                return LEFTUP;
            else
                return LEFT;
        }
    } else {
        if (stepX == 0) {
            return DOWN;
        } else if (stepX > 0) {
            if (k > BASIS_DIRECTION_HIGH)
                return DOWN;
            else if (k > BASIS_DIRECTION_LOW)
                return RIGHTDOWN;
            else
                return RIGHT;
        } else {
            if (k < -BASIS_DIRECTION_HIGH)
                return DOWN;
            else if (k < -BASIS_DIRECTION_LOW)
                return LEFTDOWN;
            else
                return LEFT;
        }
    }

    return DIR_NONE;
}

 

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
string getline(const string& str, size_t& pos) {
    if (pos > str.size())
        return "";

    size_t oldpos = pos;
    pos = str.find_first_of('\n', oldpos);

    if (pos == string::npos)
        pos = str.size() + 1;
    else
        pos = pos + 1;

    return str.substr(oldpos, (pos - 1) - oldpos);

     
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
string trim(const string& str) {
    if (str.size() == 0)
        return "";

    static const char* WhiteSpaces = " \t\n\r";
    size_t begin = str.find_first_not_of(WhiteSpaces);
    size_t end = str.find_last_not_of(WhiteSpaces);

    if (begin == string::npos)
        begin = 0;
    if (end == string::npos)
        end = str.size();

    return str.substr(begin, end - begin + 1);
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void getCurrentTimeEx(int& year, int& month, int& day, int& hour, int& minute, int& sec) {
    time_t cur_time = time(NULL);
    tm cur_tm;
    localtime_r(&cur_time, &cur_tm);
    // tm*    cur_tm   = localtime(&cur_time);

    year = cur_tm.tm_year + 1900;
    month = cur_tm.tm_mon + 1;
    day = cur_tm.tm_mday;
    hour = cur_tm.tm_hour;
    minute = cur_tm.tm_min;
    sec = cur_tm.tm_sec;
}

string getCurrentTimeStringEx(void) {
    int year, month, day, hour, minute, second;

    getCurrentTimeEx(year, month, day, hour, minute, second);

    string rValue;
    rValue += itos(year) + ". ";
    rValue += itos(month) + ". ";
    rValue += itos(day) + ". ";
    rValue += itos(hour) + ". ";
    rValue += itos(minute) + ". ";
    rValue += itos(second) + ". ";
    return rValue;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
 

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
int getPercentValueEx(int value, int percent) {
    static int tick = 0;

    int thousand = getPercentValue(value, percent * 10);
    int ret = (thousand + tick) / 10;

    tick++;
    if (tick > 9)
        tick = 0;

    return ret;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void filelog(const char* szFilename, const char* fmt, ...) {
    __BEGIN_TRY

    va_list valist;

    va_start(valist, fmt);

    char buffer[30000];

    int nchars = vsnprintf(buffer, 30000, fmt, valist);

    if (nchars == -1 || nchars > 30000) {
        throw("filelog() : more buffer size needed for log");
    }

    va_end(valist);

    VSDateTime current = VSDateTime::currentDateTime();

    ofstream file(szFilename, ios::out | ios::app);
    file << current.toString() << " : " << buffer << endl;
    file.close();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
DWORD makeDWORD(WORD hiWord, WORD loWord) {
    DWORD dwResult = ((DWORD)hiWord << 16) | loWord;

    return dwResult;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
WORD getHIWORD(DWORD dwValue) {
    WORD wResult = (WORD)(dwValue >> 16);

    return wResult;
}

WORD getLOWORD(DWORD dwValue) {
    DWORD dwMask = 0x00FF;

    WORD wResult = (WORD)(dwMask & dwValue);

    return wResult;
}
