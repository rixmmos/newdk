//--------------------------------------------------------------------------
// MString.h
//--------------------------------------------------------------------------

#ifndef	__MSTRING_H__
#define	__MSTRING_H__

#pragma warning(disable:4786)

#define	MAX_BUFFER_LENGTH		1024

#include <string.h>
#include <fstream>
class MString {
	public :
		MString();
		MString(const MString& str);
		MString(const char* str);
		virtual ~MString();

		//----------------------------------------------------
		// Init / Release
		//----------------------------------------------------
		void	Init(int len);
		void	Release();

		//----------------------------------------------------
		// assign
		//----------------------------------------------------
		void	operator = (const MString& str);
		void	operator = (const char* str);
		void	Format(const char* format, ...);

		//----------------------------------------------------
		// get
		//----------------------------------------------------
		size_t		GetLength()	const		{ return m_Length; }
		char*		GetString() const		{ return m_pString != NULL ? m_pString : s_EmptyString; }
		operator const char*() const		{ return m_pString != NULL ? m_pString : s_EmptyString; }
		operator	char*() const		{ return m_pString != NULL ? m_pString : s_EmptyString; }		

		//----------------------------------------------------
		// other operator
		//----------------------------------------------------
		bool		operator == (const char* str)		{ return strcmp(GetString(), str != NULL ? str : s_EmptyString)==0; }
		bool		operator == (const MString& str)	{ return strcmp(GetString(), str.GetString())==0; }
		bool		operator >	(const char* str)		{ return strcmp(GetString(), str != NULL ? str : s_EmptyString)>0; }
		bool		operator >	(const MString& str)	{ return strcmp(GetString(), str.GetString())>0; }
		bool		operator <	(const char* str)		{ return strcmp(GetString(), str != NULL ? str : s_EmptyString)<0; }
		bool		operator <	(const MString& str)	{ return strcmp(GetString(), str.GetString())<0; }
		bool		operator != (const char* str)		{ return strcmp(GetString(), str != NULL ? str : s_EmptyString)!=0; }
		bool		operator != (const MString& str)	{ return strcmp(GetString(), str.GetString())!=0; }

		//----------------------------------------------------
		// File I/O
		//----------------------------------------------------
		virtual void	SaveToFile(std::ofstream& file);
		virtual void	LoadFromFile(std::ifstream& file);

		//----------------------------------------------------
		// Internal
		//----------------------------------------------------
		void			ConvertToUTF8IfNeeded();	// NOTE: Now a no-op - resource files should be UTF-8

	protected :
		size_t	m_Length;
		char*	m_pString;

		static	char		s_pBuffer[MAX_BUFFER_LENGTH];
		static	char		s_EmptyString[1];
};


#endif

