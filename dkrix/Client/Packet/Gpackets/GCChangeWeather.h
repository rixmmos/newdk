//--------------------------------------------------------------------------------
// 
// Filename    : GCChangeWeather.h 
// Written By  : reiot
// 
//--------------------------------------------------------------------------------

#ifndef __GC_CHANGE_WEATHER_H__
#define __GC_CHANGE_WEATHER_H__

// include files
#include "../Packet.h"
#include "../PacketFactory.h"


//--------------------------------------------------------------------------------
//
// class GCChangeWeather;
//
//--------------------------------------------------------------------------------

class GCChangeWeather : public Packet {

public :
	
    
    void read ( SocketInputStream & iStream ) throw ( ProtocolException , Error );
		    
    
    void write ( SocketOutputStream & oStream ) const throw ( ProtocolException , Error );

	// execute packet's handler
	void execute ( Player * pPlayer ) throw ( ProtocolException , Error );

	// get packet id
	PacketID_t getPacketID () const throw () { return PACKET_GC_CHANGE_WEATHER; }
	
	// get packet's body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketSize () const throw () { return szWeather + szWeatherLevel; }

	#ifdef __DEBUG_OUTPUT__
		// get packet's name
		std::string getPacketName () const throw () { return "GCChangeWeather"; }
		
		// get packet's debug std::string
		std::string toString () const throw ();
	#endif

public :

	Weather getWeather () const throw () { return m_Weather; }
	void setWeather ( Weather weather ) throw () { m_Weather = weather; }

	WeatherLevel_t getWeatherLevel () const throw () { return m_WeatherLevel; }
	void setWeatherLevel ( WeatherLevel_t weatherLevel ) throw () { m_WeatherLevel = weatherLevel; }

public :

	Weather m_Weather;

	WeatherLevel_t m_WeatherLevel;

};


//--------------------------------------------------------------------------------
//
// class GCChangeWeatherFactory;
//
// Factory for GCChangeWeather
//
//--------------------------------------------------------------------------------

class GCChangeWeatherFactory : public PacketFactory {

public :
	
	// create packet
	Packet * createPacket () throw () { return new GCChangeWeather(); }

	#ifdef __DEBUG_OUTPUT__
		// get packet name
		std::string getPacketName () const throw () { return "GCChangeWeather"; }
	#endif
	
	// get packet id
	PacketID_t getPacketID () const throw () { return Packet::PACKET_GC_CHANGE_WEATHER; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	
	PacketSize_t getPacketMaxSize () const throw () { return szWeather + szWeatherLevel; }

};


//--------------------------------------------------------------------------------
//
// class GCChangeWeatherHandler;
//
//--------------------------------------------------------------------------------

class GCChangeWeatherHandler {

public :

	// execute packet's handler
	static void execute ( GCChangeWeather * pPacket , Player * pPlayer ) throw ( ProtocolException , Error );

};

#endif
