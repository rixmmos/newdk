//----------------------------------------------------------------------
// UpdateUtility.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include <io.h>
#include <direct.h>
#include "UpdateUtility.h"
//#include "SPKFileLib.h"
#include "CFileIndexTable.h"


#define	NULL	0

//----------------------------------------------------------------------

//----------------------------------------------------------------------
typedef	unsigned short		TYPE_PACKSIZE;
#define	SIZE_PACKSIZE		2


#define	SIZE_BUFFER			4096


//----------------------------------------------------------------------
// Has Permission
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
UUFHasPermission(const char* filename)
{	
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (filename==NULL)
	{	
		return false;
	}

	//------------------------------------------------------------
	
	
	//------------------------------------------------------------
	if (filename[0]=='\\' || filename[0]=='/')
	{
		return false;
	}

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (strchr( filename, ':' )!=NULL)
	{
		return false;
	}
	
	//------------------------------------------------------------
	
	//------------------------------------------------------------
//	if (strstr( filename, ".." )!=NULL)
//	{
//		return false;
//	}
   

	return true;
}

//----------------------------------------------------------------------
// Create Directory
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
UUFCreateDirectory(const char* dirName)
{
	
	if (UUFHasPermission( dirName ))
	{
		
		if (_mkdir( dirName )==0)
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------
// Remove Directory
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
UUFDeleteDirectory(const char* dirName)
{
	if (UUFHasPermission( dirName ))
	{
		
		if (_rmdir( dirName )==0)
		{
			return true;
		}
		
		else //if (errno!=ENOENT)
		{
			char CWD[_MAX_PATH];

			
			GetCurrentDirectory(_MAX_PATH, CWD);
			
			if (_chdir( dirName ) == 0)
			{
				//---------------------------------------------------
				
				//---------------------------------------------------
				struct _finddata_t	FileData;
				long				hFile;

				
				if( (hFile = _findfirst( "*.*", &FileData )) != -1L )					
				{
					do
					{
						
						if (FileData.name[0] != '.')
						{
							remove( FileData.name );
						}
					}
					while (_findnext( hFile, &FileData ) == 0);

					
					_findclose( hFile );			
				}
				else
				{
					return false;
				}
				
//				_chdir( "..\\" );
				SetCurrentDirectory(CWD);
				if (_rmdir( dirName )==0)
				{
					
					return true;
				}
			}		
			else
			{
				return false;
			}
		}		
	}

	return false;
}

//----------------------------------------------------------------------
// Remove Files
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
UUFDeleteFiles(const char *path, const char *fileext)
{
	char cwd[512];
	_getcwd(cwd, 512);

	if (_chdir( path ) == 0)
	{
		//---------------------------------------------------
		
		//---------------------------------------------------
		struct _finddata_t	FileData;
		long				hFile;
		
		
		if( (hFile = _findfirst( fileext, &FileData )) != -1L )					
		{
			do
			{
				
				if (FileData.name[0] != '.')
				{
					remove( FileData.name );
				}
			}
			while (_findnext( hFile, &FileData ) == 0);
			
			
			_findclose( hFile );			
		}
		
	}		
	_chdir(cwd);

	return true;
}

//----------------------------------------------------------------------
// Copy File
//----------------------------------------------------------------------


//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
UUFCopyFile(const char* FilenameSource, const char* FilenameTarget)
{				
	if (UUFHasPermission( FilenameTarget ) && UUFHasPermission( FilenameSource))				
	{
		std::ifstream fileSource(FilenameSource, ios::binary);
		std::ofstream fileTarget(FilenameTarget, ios::binary);
		
		
		char buffer[SIZE_BUFFER];
		int n;
		
		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		while (1)
		{
			fileSource.read(buffer, SIZE_BUFFER);
			
			n = fileSource.gcount();

			if (n > 0)
			{		
				fileTarget.write(buffer, n);
			}
			else
			{
				break;
			}
		}

		fileSource.close();
		fileTarget.close();		

		return true;
	}

	return false;
}
					
//----------------------------------------------------------------------
// Move File
//----------------------------------------------------------------------


//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
UUFMoveFile(const char* FilenameSource, const char* FilenameTarget)
{				
	if (UUFHasPermission( FilenameTarget ) && UUFHasPermission( FilenameSource))				
	{
		ifstream file;

		file.open(FilenameTarget, ios::binary | );
		
		if (file.is_open())
		{
			file.close();

			
			if (remove( FilenameTarget )!=0)
			{
				
				return false;
			}
		}		
	
		
		if (rename( FilenameSource, FilenameTarget )==0)
		{
			return true;
		}		
	}

	return false;
}

//----------------------------------------------------------------------
// Delete File
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
UUFDeleteFile(const char* FilenameTarget)
{
	
	if (UUFHasPermission( FilenameTarget ))
	{
		if (remove( FilenameTarget )==0)
		{
			return true;
		}
	}

	return false;
}


//----------------------------------------------------------------------
// Rename File
//----------------------------------------------------------------------
// SourceFile : old filename
// TargetFile : new filename
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool
UUFRenameFile(const char* FilenameSource, const char* FilenameTarget)
{
	
	if (UUFHasPermission( FilenameTarget ))
	{
		if (rename( FilenameSource, FilenameTarget )==0)
		{
			return true;
		}
	}

	return false;
}


//----------------------------------------------------------------------
// Append Pack
//----------------------------------------------------------------------
// SourceFile		: Pack(to add)
// TargetFile		: Pack(original)
// SourceIndexFile	: PackIndex(to add)
// TargetIndexFile	: PackIndex(original)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool
UUFAppendPack(const char* FilenameAdd, const char* FilenameOriginal,
			const char* FilenameIndexAdd, const char* FilenameIndexOriginal)
{	
	//--------------------------------------------------
	
	//--------------------------------------------------
	bool bAppendIndex;

	if (FilenameIndexAdd==NULL && FilenameIndexOriginal==NULL)
	{
		bAppendIndex = false;
	}
	else
	{
		bAppendIndex = true;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	if (UUFHasPermission( FilenameOriginal ) 
		&& UUFHasPermission( FilenameAdd)

		
		&& (!bAppendIndex ||
			UUFHasPermission( FilenameIndexOriginal ) 
			&& UUFHasPermission( FilenameIndexAdd )
		))		
	{
		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		//if (!IsFileSizeBeforeOK())
		//{
			
		//	return false;
		//}

		//---------------------------------------------------------------
		//
		
		//
		//---------------------------------------------------------------
		
		std::ifstream addFile(FilenameAdd, ios::binary);
		class fstream originalFile(FilenameOriginal, ios::in | ios::out | ios::binary);

		TYPE_PACKSIZE	sourceCount, targetCount;
		
		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		addFile.read((char*)&sourceCount, SIZE_PACKSIZE);

		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		originalFile.seekp(0, ios::end);

		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		long originalPackFileSize = originalFile.tellp();

		
		char buffer[SIZE_BUFFER];
		int n;
		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		while (1)
		{
			addFile.read(buffer, SIZE_BUFFER);
			
			n = addFile.gcount();

			if (n > 0)
			{		
				originalFile.write(buffer, n);
			}
			else
			{
				break;
			}
		}

		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		originalFile.seekg(0, ios::beg);
		originalFile.read((char*)&targetCount, SIZE_PACKSIZE);

		targetCount += sourceCount;

		originalFile.seekp(0, ios::beg);				
		originalFile.write((const char*)&targetCount, SIZE_PACKSIZE);

		
		addFile.close();
		originalFile.close();

		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		//if (!IsFileSizeAfterOK())
		//{
			
			
		//	return false;
		//}

		
		//---------------------------------------------------------------
		//
		
		//
		//---------------------------------------------------------------	
		
		
		if (bAppendIndex)
		{
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			//if (!IsFileSizeBeforeOK())
			//{
				
			//	return false;
			//}


			
			std::ifstream addIndexFile(FilenameIndexAdd, ios::binary);
			class fstream originalIndexFile(FilenameIndexOriginal, ios::in | ios::out | ios::binary);	

			TYPE_PACKSIZE	targetCount;

			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			CFileIndexTable	sourceIndexFile;
			sourceIndexFile.LoadFromFile( addIndexFile );

			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			originalIndexFile.seekp(0, ios::end);
			
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			long targetEnd = originalPackFileSize - SIZE_PACKSIZE;

			//---------------------------------------------------------------
			
			
			//---------------------------------------------------------------
			long sourceIndex;
			for (int i=0; i<sourceIndexFile.GetSize(); i++)
			{
				sourceIndex = targetEnd + sourceIndexFile[i];
				originalIndexFile.write((const char*)&sourceIndex, 4);
			}								

			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			originalIndexFile.seekg(0, ios::beg);
			originalIndexFile.read((char*)&targetCount, SIZE_PACKSIZE);

			targetCount += sourceIndexFile.GetSize();

			originalIndexFile.seekp(0, ios::beg);				
			originalIndexFile.write((const char*)&targetCount, SIZE_PACKSIZE);

			
			addIndexFile.close();
			originalIndexFile.close();

			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			//if (!IsFileSizeAfterOK())
			//{
				
				
			//	return false;
			//}	
		}

		return true;
	}

	return false;
}



//----------------------------------------------------------------------
// Append Info
//----------------------------------------------------------------------


//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool
UUFAppendInfo(const char* FilenameAdd, const char* FilenameOriginal)
{
	if (UUFHasPermission( FilenameOriginal ) && UUFHasPermission( FilenameAdd))				
	{			
		//------------------------------------------
		
		//------------------------------------------
		//if (!IsFileSizeBeforeOK())
		//{
			
		//	return false;
		//}

		
		std::ifstream sourceFile(FilenameAdd, ios::binary);
		class fstream targetFile(FilenameOriginal, ios::in | ios::out | ios::binary);
		

		int sourceCount, targetCount;
		
		sourceFile.read((char*)&sourceCount, 4);

		
		targetFile.seekp(0, ios::end);

		
		char buffer[SIZE_BUFFER];
		int n;
		
		while (1)
		{
			sourceFile.read(buffer, SIZE_BUFFER);
			
			n = sourceFile.gcount();

			if (n > 0)
			{		
				targetFile.write(buffer, n);
			}
			else
			{
				break;
			}
		}

		
		targetFile.seekg(0, ios::beg);
		targetFile.read((char*)&targetCount, 4);

		targetCount += sourceCount;

		targetFile.seekp(0, ios::beg);				
		targetFile.write((const char*)&targetCount, 4);

		
		sourceFile.close();
		targetFile.close();


		//------------------------------------------
		
		//------------------------------------------
		//if (!IsFileSizeAfterOK())
		//{
			
			
		//	return false;
		//}
	}

	return true;
}		

//----------------------------------------------------------------------
// Update SpritePack
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 