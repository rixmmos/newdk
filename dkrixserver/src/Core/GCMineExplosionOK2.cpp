//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMineExplosionOK2.cpp
// Written By  : elca@ewestsoft.com


//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "GCMineExplosionOK2.h"


//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCMineExplosionOK2::GCMineExplosionOK2()

{
    __BEGIN_TRY

    m_CListNum = 0;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GCMineExplosionOK2::~GCMineExplosionOK2()

{
    __BEGIN_TRY
    __END_CATCH_NO_RETHROW
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCMineExplosionOK2::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    
    iStream.read(m_X);
    iStream.read(m_Y);
    iStream.read(m_Dir);
    iStream.read(m_ItemType);
    iStream.read(m_CListNum);

    ObjectID_t m_Value;
    int i;

    for (i = 0; i < m_CListNum; i++) {
        iStream.read(m_Value);
        m_CList.push_back(m_Value);
    }


    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCMineExplosionOK2::write(SocketOutputStream& oStream) const {
    __BEGIN_TRY

    
    oStream.write(m_X);
    oStream.write(m_Y);
    oStream.write(m_Dir);
    oStream.write(m_ItemType);
    oStream.write(m_CListNum);

    for (list<ObjectID_t>::const_iterator itr = m_CList.begin(); itr != m_CList.end(); itr++) {
        oStream.write(*itr);
    }


    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// GCMineExplosionOK2::addListElement()
//

//
//////////////////////////////////////////////////////////////////////
void GCMineExplosionOK2::addCListElement(ObjectID_t ObjectID)

{
    __BEGIN_TRY

    
    m_CList.push_back(ObjectID);

    
    m_CListNum++;

    __END_CATCH
}

 


//////////////////////////////////////////////////////////////////////
//
// execute packet's handler
//
//////////////////////////////////////////////////////////////////////
void GCMineExplosionOK2::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCMineExplosionOK2Handler::execute(this, pPlayer);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string GCMineExplosionOK2::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "GCMineExplosionOK2(X:" << (int)m_X << ",Y:" << (int)m_Y << ",Dir:" << (int)m_Dir
        << ",ItemType:" << (int)m_ItemType << ",CListNum: " << (int)m_CListNum << " CListSet(";

    for (list<ObjectID_t>::const_iterator itr = m_CList.begin(); itr != m_CList.end(); itr++) {
        msg << (int)(*itr) << ",";
    }

    msg << ")";


    return msg.toString();

    __END_CATCH
}
