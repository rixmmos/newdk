//////////////////////////////////////////////////////////////////////////////
// Filename    : GCWarScheduleList.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GCWarScheduleList.h"

#include "Assert1.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
GCWarScheduleList::GCWarScheduleList()

{
    __BEGIN_TRY

    __END_CATCH;
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCWarScheduleList::~GCWarScheduleList()

{
    __BEGIN_TRY

    WarScheduleInfoList::iterator itr = m_WarScheduleList.begin();

    for (; itr != m_WarScheduleList.end(); itr++) {
        if (*itr != NULL)
            SAFE_DELETE(*itr);
    }

    m_WarScheduleList.clear();

    __END_CATCH_NO_RETHROW;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCWarScheduleList::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    BYTE count = 0;

    iStream.read(count);

    for (int i = 0; i < count; i++) {
        WarScheduleInfo* newWarScheduleInfo;
        newWarScheduleInfo = new WarScheduleInfo;

        iStream.read(newWarScheduleInfo->warType);
        iStream.read(newWarScheduleInfo->year);
        iStream.read(newWarScheduleInfo->month);
        iStream.read(newWarScheduleInfo->day);
        iStream.read(newWarScheduleInfo->hour);
        if (newWarScheduleInfo->warType == 0) {
            for (int i = 0; i < 5; ++i) {
                BYTE szGuildName;
                iStream.read(newWarScheduleInfo->challengerGuildID[i]);
                iStream.read(szGuildName);
                if (szGuildName != 0)
                    iStream.read(newWarScheduleInfo->challengerGuildName[i], szGuildName);
            }

            BYTE szGuildName;
            iStream.read(newWarScheduleInfo->reinforceGuildID);
            iStream.read(szGuildName);
            if (szGuildName != 0)
                iStream.read(newWarScheduleInfo->reinforceGuildName, szGuildName);
        }

        addWarScheduleInfo(newWarScheduleInfo);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCWarScheduleList::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    BYTE ListNum = m_WarScheduleList.size();

    oStream.write(ListNum);

    WarScheduleInfoListItor itr = m_WarScheduleList.begin();

    for (; itr != m_WarScheduleList.end(); itr++) {
        oStream.write((*itr)->warType);
        oStream.write((*itr)->year);
        oStream.write((*itr)->month);
        oStream.write((*itr)->day);
        oStream.write((*itr)->hour);
        if ((*itr)->warType == 0) {
            for (int i = 0; i < 5; ++i) {
                oStream.write((*itr)->challengerGuildID[i]);
                BYTE szGuildName = (*itr)->challengerGuildName[i].size();
                oStream.write(szGuildName);
                oStream.write((*itr)->challengerGuildName[i]);
            }

            oStream.write((*itr)->reinforceGuildID);
            BYTE szGuildName = (*itr)->reinforceGuildName.size();
            oStream.write(szGuildName);
            oStream.write((*itr)->reinforceGuildName);
        }
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCWarScheduleList::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCWarScheduleListHandler::execute(this, pPlayer);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

PacketSize_t GCWarScheduleList::getPacketSize() const

{
    __BEGIN_TRY

    PacketSize_t size = szBYTE;

    WarScheduleInfoListItor itr = m_WarScheduleList.begin();

    for (; itr != m_WarScheduleList.end(); itr++) {
        size += szBYTE + szWORD + szBYTE + szBYTE + szBYTE;

        if ((*itr)->warType == 0) {
            for (int i = 0; i < 5; ++i) {
                size += szGuildID;
                size += szBYTE;
                size += (*itr)->challengerGuildName[i].size();
            }

            size += szGuildID;
            size += szBYTE;
            size += (*itr)->reinforceGuildName.size();
        }
    }

    return size;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////////////
string GCWarScheduleList::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "GCWarScheduleList("
        << "WarNum : " << m_WarScheduleList.size();

     

    msg << ")";

    return msg.toString();

    __END_CATCH
}

WarScheduleInfo* GCWarScheduleList::popWarScheduleInfo()

{
    __BEGIN_TRY

    if (m_WarScheduleList.empty())
        return NULL;

    WarScheduleInfo* ret = m_WarScheduleList.front();
    m_WarScheduleList.pop_front();

    return ret;

    __END_CATCH
}
