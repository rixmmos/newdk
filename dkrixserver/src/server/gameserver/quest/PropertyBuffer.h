////////////////////////////////////////////////////////////////////////////////
// Filename    : PropertyBuffer.h
// Written By  :
// Description :





////////////////////////////////////////////////////////////////////////////////

#ifndef __PROPERTY_BUFFER_H__
#define __PROPERTY_BUFFER_H__

#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class PropertyBuffer;
//////////////////////////////////////////////////////////////////////////////

class PropertyBuffer {
public:
    static const char Comment;
    static const char Separator;
    static const char EOL;
    static const char* WhiteSpaces;
    static const char* SpaceTab;

public:
    PropertyBuffer(const string& buffer);
    ~PropertyBuffer();

public:
    string getProperty(string key);
    int getPropertyInt(string key);

    bool getProperty(string key, string& value);
    bool getPropertyInt(string key, int& value);

    string toString() const;

private:
    size_t m_Index;
    string m_Buffer;
};

#endif
