//--------------------------------------------------------------------------------
// 
// Filename    : GCUsePowerPointResult.h 
// Written By  : bezz
// Description : 
// 
//--------------------------------------------------------------------------------

#ifndef __GC_USE_POWER_POINT_RESULT_H__
#define __GC_USE_POWER_POINT_RESULT_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//--------------------------------------------------------------------------------
//
// class GCUsePowerPointResult;
//
//--------------------------------------------------------------------------------

class GCUsePowerPointResult : public Packet
{
public:

	enum RESULT_CODE
	{
		_NO_ERROR = 0,
		NOT_ENOUGH_POWER_POINT,		
		NOT_ENOUGH_INVENTORY_SPACE	
	};

	enum ITEM_CODE
	{
		CANDY = 0,				
		RESURRECTION_SCROLL,	
		ELIXIR_SCROLL,			
		MEGAPHONE,				
		NAMING_PEN,				
		SIGNPOST,				
		BLACK_RICE_CAKE_SOUP	
	};

public:
	GCUsePowerPointResult() throw();
	~GCUsePowerPointResult() throw();

public :
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_USE_POWER_POINT_RESULT; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szBYTE + szBYTE + szDWORD; }
#ifdef __DEBUG_OUTPUT__
	// get packet name
	string getPacketName() const throw() { return "GCUsePowerPointResult"; }
	
	// get packet's debug string
	string toString() const throw();
#endif
	// get / set Error Code
	BYTE getErrorCode() const { return m_ErrorCode; }
	void setErrorCode( BYTE errorcode ) { m_ErrorCode = errorcode; }

	// get / set Item Code
	BYTE getItemCode() const { return m_ItemCode; }
	void setItemCode( BYTE itemcode ) { m_ItemCode = itemcode; }

	// get / set Power Point
	uint getPowerPoint() const { return m_PowerPoint; }
	void setPowerPoint( BYTE powerpoint ) { m_PowerPoint = powerpoint; }

//--------------------------------------------------
// data members
//--------------------------------------------------
private :
	
	BYTE	m_ErrorCode;

	
	BYTE	m_ItemCode;

	
	uint	m_PowerPoint;
};


//--------------------------------------------------------------------------------
//
// class GCUsePowerPointResultFactory;
//
// Factory for GCUsePowerPointResult
//
//--------------------------------------------------------------------------------

class GCUsePowerPointResultFactory : public PacketFactory
{
public :
	// create packet
	Packet* createPacket() throw() { return new GCUsePowerPointResult(); }
#ifdef __DEBUG_OUTPUT__
	// get packet name
	string getPacketName() const throw() { return "GCUsePowerPointResult"; }
#endif
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_USE_POWER_POINT_RESULT; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + szBYTE + szDWORD; }
};



//--------------------------------------------------------------------------------
//
// class GCUsePowerPointResultHandler;
//
//--------------------------------------------------------------------------------

class GCUsePowerPointResultHandler
{
public :
	// execute packet's handler
	static void execute(GCUsePowerPointResult* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif

