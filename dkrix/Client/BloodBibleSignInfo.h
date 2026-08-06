//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodBibleSignInfo.h 
// Written By  : elca@ewestsoft.com
// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __BLOOD_BIBLE_SIGN_INFO_H__
#define __BLOOD_BIBLE_SIGN_INFO_H__

#include <vector>

#include "Packet/Types.h"
#include "Packet/Exception.h"
#include "Packet/Packet.h"

#define BLOOD_BIBLE_SIGN_SLOT_NUM 6

//////////////////////////////////////////////////////////////////////////////
// class BloodBibleSignInfo;
//////////////////////////////////////////////////////////////////////////////

class BloodBibleSignInfo 
{
public:
	BloodBibleSignInfo () throw ();
	~BloodBibleSignInfo () throw ();
	
public:
    void read (SocketInputStream & iStream) throw (ProtocolException, Error);
    void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);

	PacketSize_t getSize () const throw () { return szuint + szBYTE + szItemType * m_SignList.size(); }
	static uint getMaxSize () throw () { return szuint + szBYTE + szItemType * BLOOD_BIBLE_SIGN_SLOT_NUM; }

public:
	uint	getOpenNum() const { return m_OpenNum; }
	void	setOpenNum( uint num ) { m_OpenNum = num; }

	std::vector<ItemType_t>&	getList() { return m_SignList; }
	const std::vector<ItemType_t>& getList() const { return m_SignList; }
private:
	uint				m_OpenNum;
	BYTE				m_ListSize;
	std::vector<ItemType_t>	m_SignList;
};

#endif
