//--------------------------------------------------------------------------------
// 
// Filename    : GCRequestPowerPointResult.h 
// Written By  : bezz
// Description : 
// 
//--------------------------------------------------------------------------------

#ifndef __GC_REQUEST_POWER_POINT_RESULT_H__
#define __GC_REQUEST_POWER_POINT_RESULT_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//--------------------------------------------------------------------------------
//
// class GCRequestPowerPointResult;
//
//--------------------------------------------------------------------------------

class GCRequestPowerPointResult : public Packet
{
public:
	enum RESULT_CODE
	{
		_NO_ERROR = 0,
		SERVER_ERROR,		
		PROCESS_ERROR,		
		NO_MEMBER,			
		NO_POINT,			
		NO_MATCHING,		
		CONNECT_ERROR,		
							
							
	};
public:
	GCRequestPowerPointResult() throw();
	~GCRequestPowerPointResult() throw();

public :
    
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_REQUEST_POWER_POINT_RESULT; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szBYTE + szint + szint; }

#ifdef __DEBUG_OUTPUT__
	// get packet name
	string getPacketName() const throw() { return "GCRequestPowerPointResult"; }
	
	// get packet's debug string
	string toString() const throw();
#endif
	// get / set Error Code
	BYTE getErrorCode() const { return m_ErrorCode; }
	void setErrorCode( BYTE errorcode ) { m_ErrorCode = errorcode; }

	// get / set SumPowerPoint
	int getSumPowerPoint() const { return m_SumPowerPoint; }
	void setSumPowerPoint( int powerpoint ) { m_SumPowerPoint = powerpoint; }

	// get / set RequestPowerPoint
	int getRequestPowerPoint() const { return m_RequestPowerPoint; }
	void setRequestPowerPoint( int powerpoint ) { m_RequestPowerPoint = powerpoint; }

//--------------------------------------------------
// data members
//--------------------------------------------------
private :
	
	BYTE	m_ErrorCode;

	
	int		m_SumPowerPoint;

	
	int		m_RequestPowerPoint;
};


//--------------------------------------------------------------------------------
//
// class GCRequestPowerPointResultFactory;
//
// Factory for GCRequestPowerPointResult
//
//--------------------------------------------------------------------------------

class GCRequestPowerPointResultFactory : public PacketFactory
{
public :
	// create packet
	Packet* createPacket() throw() { return new GCRequestPowerPointResult(); }

	// get packet name
#ifdef __DEBUG_OUTPUT__
	string getPacketName() const throw() { return "GCRequestPowerPointResult"; }
#endif	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_REQUEST_POWER_POINT_RESULT; }


	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + szint + szint; }
};



//--------------------------------------------------------------------------------
//
// class GCRequestPowerPointResultHandler;
//
//--------------------------------------------------------------------------------

class GCRequestPowerPointResultHandler
{
public :
	// execute packet's handler
	static void execute(GCRequestPowerPointResult* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif

