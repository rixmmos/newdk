//--------------------------------------------------------------------------------
//
// Filename    : Properties.cpp
// Written By  : Reiot
// Description : Cross-platform properties file handling with path conversion
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#ifdef PLATFORM_WINDOWS
#include <wtypes.h>
#endif
#include "Packet/Properties.h"
#include <stdlib.h>			// atoi()
#include <fstream>
#include <iostream>

//--------------------------------------------------------------------------------
// Helper function to convert Windows path separators to Unix format
//--------------------------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
	/* On Windows, no conversion needed - inline function for efficiency */
	static inline std::string ConvertPathSeparators(const std::string& path) {
		return path;  // Return unchanged on Windows
	}
#else
	/* On Unix/macOS, convert \\ to / */
	static inline std::string ConvertPathSeparators(const std::string& path) {
		std::string result = path;
		size_t pos = 0;
		/* Replace all \\ with / */
		while ((pos = result.find("\\\\", pos)) != std::string::npos) {
			result.replace(pos, 2, "/");
			pos += 1;  // Move past the replaced /
		}
		/* Also replace single \ with / (for edge cases) */
		pos = 0;
		while ((pos = result.find('\\', pos)) != std::string::npos) {
			result.replace(pos, 1, "/");
			pos += 1;  // Move past the replaced /
		}
		return result;
	}
#endif

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
const char Properties::Comment = '#';
const char Properties::Separator = ':';
const char * Properties::WhiteSpaces = " \t";


//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Properties::Properties () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

Properties::Properties ( const std::string & filename ) 
	throw ()
: m_Filename(filename)
{
	__BEGIN_TRY
	__END_CATCH
}
	

//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
Properties::~Properties () 
	throw ()
{	
	__BEGIN_TRY
		
	
	m_Properties.clear();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// load from file
//--------------------------------------------------------------------------------
void Properties::load () 
	throw ( IOException , Error )
{
	__BEGIN_TRY
		
	if ( m_Filename.empty() )
		throw Error("filename not specified");
		
	std::ifstream ifile( m_Filename.c_str() , std::ios::in );
	
	if ( ! ifile )
		throw FileNotExistException( m_Filename.c_str() );

	while ( true ) {

		std::string line;
		std::getline( ifile , line );

		if ( ifile.eof() )
			break;

		// Remove trailing \r (Windows CRLF files on Unix/macOS)
		if ( !line.empty() && line[line.size() - 1] == '\r' ) {
			line.erase( line.size() - 1 );
		}

		
		if ( line[0] == Comment || line.size() == 0 )
			continue;

		
		uint key_begin = line.find_first_not_of( WhiteSpaces );
		
		
		
		if ( key_begin == std::string::npos )
			continue;

		
		
		
		uint sep = line.find( Separator , key_begin );

		
		if ( sep == std::string::npos )
			throw IOException("missing separator");
		
		
		uint key_end = line.find_last_not_of( WhiteSpaces , sep - 1 );
		
		
		uint value_begin = line.find_first_not_of( WhiteSpaces , sep + 1 );
		
		
		if ( value_begin == std::string::npos )
			throw IOException("missing value");
		
		
		
		uint value_end = line.find_last_not_of( WhiteSpaces ); 

		
		
		std::string key = line.substr( key_begin , key_end - key_begin + 1 );
		std::string value = line.substr( value_begin , value_end - value_begin + 1 );

		
		setProperty( key , value );
	}
	
	ifile.close();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// save to file
//--------------------------------------------------------------------------------
void Properties::save () 
	throw ( IOException )
{
	__BEGIN_TRY
		
	if ( m_Filename.empty() )
		throw Error("filename not specified");

	std::ofstream ofile( m_Filename.c_str() , std::ios::out | std::ios::trunc );
	
	for ( std::map< std::string , std::string , StringCompare >::iterator itr = m_Properties.begin() ;
		  itr != m_Properties.end() ;
		  itr ++ )
		ofile << itr->first.c_str() << ' ' << Separator << ' ' << itr->second.c_str() << endl;
	
	ofile.close();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get property
//--------------------------------------------------------------------------------
std::string Properties::getProperty ( std::string key ) const
	throw ( NoSuchElementException )
{
	__BEGIN_TRY

		std::string value = getProperty(key.c_str());
		/* Convert path separators for cross-platform compatibility */
		return ConvertPathSeparators(value);

	__END_CATCH
}

//--------------------------------------------------------------------------------
// get property
//--------------------------------------------------------------------------------
std::string Properties::getProperty ( const char* key ) const
throw ( NoSuchElementException )
{
	__BEGIN_TRY

		std::string value;

	std::map< std::string , std::string , StringCompare >::const_iterator itr = m_Properties.find( key );

	if ( itr != m_Properties.end() )
		value = itr->second;
	else
		throw NoSuchElementException(key);

	/* Convert path separators for cross-platform compatibility */
	return ConvertPathSeparators(value);

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get property as int
//--------------------------------------------------------------------------------
int Properties::getPropertyInt ( std::string key ) const 
	throw ( NoSuchElementException )
{
	__BEGIN_TRY
		
	return atoi( getProperty(key).c_str() );
	
	__END_CATCH
}

//--------------------------------------------------------------------------------
// get property as int
//--------------------------------------------------------------------------------
int Properties::getPropertyInt ( const char* key ) const 
throw ( NoSuchElementException )
{
	__BEGIN_TRY
		
		return atoi( getProperty(key).c_str() );
	
	__END_CATCH
}


//--------------------------------------------------------------------------------
// set property
//--------------------------------------------------------------------------------
void Properties::setProperty ( std::string key , std::string value )
	throw ()
{
	__BEGIN_TRY
		
	
	m_Properties[ key ] = value;

	__END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
std::string Properties::toString () const
    throw ()
{
	__BEGIN_TRY
		
	StringStream msg;

	for ( std::map< std::string , std::string , StringCompare >::const_iterator itr = m_Properties.begin() ;
		  itr != m_Properties.end() ;
		  itr ++ ) {
		
		msg << itr->first << " : " << itr->second << "\n";
	}
	
	if ( msg.isEmpty() )
		msg << "empty properties";
	
	return msg.toString();

	__END_CATCH
}


//--------------------------------------------------------------------------------
// global variable definition
//--------------------------------------------------------------------------------
Properties * g_pConfigKorean = NULL;
Properties * g_pFileDef = NULL;
