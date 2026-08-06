//----------------------------------------------------------------------
// UpdateUtility.h
//----------------------------------------------------------------------

#ifndef	__UPDATEUTILITY_H__
#define	__UPDATEUTILITY_H__

//----------------------------------------------------------------------
// Update Utility Functions
//----------------------------------------------------------------------
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		UUFHasPermission(const char* filename);


//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		UUFCreateDirectory(const char* dirName);
bool		UUFDeleteDirectory(const char* dirName);


//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		UUFCopyFile(const char* FilenameSource, const char* FilenameTarget);
bool		UUFMoveFile(const char* FilenameSource, const char* FilenameTarget);
bool		UUFDeleteFile(const char* FilenameTarget);
bool		UUFDeleteFiles(const char *path, const char *fileext);
bool		UUFRenameFile(const char* FilenameSource, const char* FilenameTarget);


//----------------------------------------------------------------------
// Append Pack (with Index)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		UUFAppendPack(const char* FilenameAdd, const char* FilenameOriginal,
						const char* FilenameIndexAdd=0, const char* FilenameIndexOriginal=0);

//----------------------------------------------------------------------
// Delete SpritePack
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//bool		UUFDeleteSpritePack(const char* FilenameID, const char* FilenameOriginal);

//----------------------------------------------------------------------
// Update SpritePack
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//bool		UUFUpdateSpritePack(const char* FilenameNew, 
//								const char* FilenameID, 
//								const char* FilenameOriginal);

//----------------------------------------------------------------------
// Append Info
//----------------------------------------------------------------------
bool		UUFAppendInfo(const char* FilenameAdd, const char* FilenameOriginal);

//----------------------------------------------------------------------
// Make SpritePack Index
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//bool		UUFMakeSpritePackIndex(const char* Filename);

#endif