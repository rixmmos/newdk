//--------------------------------------------------------------------------------
//
// Filename    : main.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Exception.h"
#include "Properties.h"
#include "Types.h"
#include "UpdateServer.h"


#include <dirent.h>
#include <stdio.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>

#include <sys/resource.h>
#include <sys/stat.h>

#include "StringStream.h"
#include "Update.h"
#include "UpdateManager.h"

//	scandir
//	readdir
//	seekdir
//	getcwd
//

//	strcasecmp( "LISt" , "LIST" ) == 0

class SourceInfo {
public:
    string getName() {
        return m_Name;
    }
    void setName(string Name) {
        m_Name = Name;
    }

    string getDirectory() {
        return m_Directory;
    }
    void setDirectory(string Directory) {
        m_Directory = Directory;
    }

private:
    string m_Name;
    string m_Directory;
};

class VersionInfo {
public:
    string getName() {
        return m_Name;
    }
    void setName(string Name) {
        m_Name = Name;
    }

    string getDirectory() {
        return m_Directory;
    }
    void setDirectory(string Directory) {
        m_Directory = Directory;
    }

    int getVersion() {
        return m_Version;
    }
    void setVersion(int Version) {
        m_Version = Version;
    }

    int getSize() {
        return m_Size;
    }
    void setSize(int Size) {
        m_Size = Size;
    }

private:
    string m_Name;
    string m_Directory;
    int m_Size;
    int m_Version;
};

list<SourceInfo*> SourceInfoList;
list<VersionInfo*> VersionInfoList;


class isSameName {
public:
    // constructor
    isSameName(string Name) : m_Name(Name) {}


    bool operator()(VersionInfo* pVersionInfo) throw() {
        
        if (strcasecmp(m_Name.c_str(), pVersionInfo->getName().c_str()) == 0) {
            return true;
        } else {
            return false;
        }
    }

private:
    // Name identifier
    string m_Name;
};

class isSource {
public:
    // constructor
    isSource(string Name) : m_Name(Name) {}


    bool operator()(SourceInfo* pSourceInfo) throw() {
        
        if (strcasecmp(m_Name.c_str(), pSourceInfo->getName().c_str()) == 0) {
            return true;
        } else {
            return false;
        }
    }

private:
    // Name identifier
    string m_Name;
};


/*
void main(void) {

    struct dirent * Testdir;
    int count = 0;

    readdir( 0, Testdir, count );
    scandir

    //cout << " DirName : " << TestDir->d_Name << endl;

}
*/
 


#include <dirent.h>

int myselect(const struct dirent* entry) {
    const char* filename = entry->d_name;
    if (*filename == '.') {
        return 0;
    } else {
        return 1;
    }
}

void scan_Source(const string& Directory) {
    //	struct dirent **namelist;

    DIR* pDIR = opendir(Directory.c_str());
    struct dirent* versionDir;
    //	//cout << "Open Dir : " << Directory << endl;

    while ((versionDir = readdir(pDIR)) != NULL) {
        string vName = versionDir->d_name;

        if (versionDir->d_ino != 0 && vName[0] != '.') {
            //			//cout << "versionDir : " << versionDir->d_name<< endl;

            string DirectoryName = Directory + "/";
            string SubDirectory = DirectoryName + versionDir->d_name;

            struct stat List;
            stat(SubDirectory.c_str(), &List);
            //			//cout << versionDir->d_name << " " << List.st_size << endl;

            replace(DirectoryName.begin(), DirectoryName.end(), '/', '\\');
            //			//cout << "Path : " << array << " Path Size : " << size << endl;

            if (S_ISDIR(List.st_mode)) {
                //				//cout << versionDir->d_name << " is Directory " << endl;


                //			string SubDirectory = DirectoryName + "/" + namelist[n]->d_name;

                //				//cout << "SubDirectory : " << SubDirectory << endl;
                scan_Source(SubDirectory);
                //			//cout << get_current_dir_name() << endl;
            } else {
                SourceInfo* pSourceInfo = new SourceInfo();
                pSourceInfo->setName(versionDir->d_name);
                pSourceInfo->setDirectory(DirectoryName.substr(g_pConfig->getProperty("SourceDirectory").size() + 1,
                                                               DirectoryName.size() - 1));
                SourceInfoList.push_back(pSourceInfo);

                 
            }
        }
    }
    closedir(pDIR);
}

void scan_Version(const string& Directory, int version) {
    //	struct dirent **namelist;

    DIR* pDIR = opendir(Directory.c_str());
    struct dirent* versionDir;
    //	//cout << "Open Dir : " << Directory << endl;

    while ((versionDir = readdir(pDIR)) != NULL) {
        string vName = versionDir->d_name;

        if (vName == "index.dat") {
            string removeName = Directory + "/" + "index.dat";
            // cout << "Remove index.dat Directory : " << removeName << endl;
            remove(removeName.c_str());
        }

        if (versionDir->d_ino != 0 && vName[0] != '.' && vName != "index.dat") {
            //			//cout << "versionDir : " << versionDir->d_name<< endl;
            string DirectoryName = Directory;

            string SubDirectory = DirectoryName + "/" + versionDir->d_name;

            /*
            replace ( DirectoryName.begin(), DirectoryName.end(), '/', '\\' );
//			//cout << "ChangeName : " << DirectoryName << endl;
            */

            struct stat List;
            stat(SubDirectory.c_str(), &List);
            //			//cout << versionDir->d_name << " " << List.st_size << endl;

            //			//cout << "Path : " << array << " Path Size : " << size << endl;

            if (S_ISDIR(List.st_mode)) {
                int version = atoi(vName.substr(1, 5).c_str());

                //				//cout << versionDir->d_name << " is Directory " << endl;


                //			string SubDirectory = DirectoryName + "/" + namelist[n]->d_name;

                //				//cout << "SubDirectory : " << SubDirectory << endl;
                scan_Version(SubDirectory, version);
                //			//cout << get_current_dir_name() << endl;
            } else {
                VersionInfo* pVersionInfo = new VersionInfo();
                pVersionInfo->setName(versionDir->d_name);

                list<SourceInfo*>::iterator itr2 =
                    find_if(SourceInfoList.begin(), SourceInfoList.end(), isSource(pVersionInfo->getName()));

                pVersionInfo->setDirectory((*itr2)->getDirectory());
                pVersionInfo->setSize(List.st_size);

                pVersionInfo->setVersion(version);

                
                list<VersionInfo*>::iterator itr =
                    find_if(VersionInfoList.begin(), VersionInfoList.end(), isSameName(pVersionInfo->getName()));

                
                if (itr != VersionInfoList.end()) {
                    

                    
                    
                    

                    if ((*itr)->getVersion() < pVersionInfo->getVersion()) {
                        VersionInfo* pTempVersionInfo = (*itr);

                        VersionInfoList.erase(itr);
                        

                        delete pTempVersionInfo;

                        VersionInfoList.push_back(pVersionInfo);
                        

                    } else {
                    }

                    
                } else {
                    VersionInfoList.push_back(pVersionInfo);
                }
            }
        }
    }
    closedir(pDIR);
}

void create_Index() {
    list<VersionInfo*>::const_iterator itr = VersionInfoList.begin();

    int beforeVersion = 0;
    int version = 0;

    
    for (; itr != VersionInfoList.end(); itr++) {
        StringStream msg1;
        StringStream msg2;
        StringStream msg3;
        StringStream msg4;

        UpdateManager updatemanager;

        Update* pUpdate = new Update();

        msg1 << (*itr)->getName() << " " << (*itr)->getSize();
        msg2 << (*itr)->getDirectory() << (*itr)->getName();

        version = (*itr)->getVersion();

        char filename[256];
        sprintf(filename, "%s/v%05d/index.dat", g_pConfig->getProperty("PatchDirectory").c_str(), version);

        ofstream ofile(filename, ios::out | ios::app);

        
        if (beforeVersion != version) {
            int Count = 0;
            list<VersionInfo*>::const_iterator itr2 = VersionInfoList.begin();

            for (; itr2 != VersionInfoList.end(); itr2++) {
                if ((*itr2)->getVersion() == version) {
                    Count++;
                }
            }
            // cout << "Version : " << version << ", Count: " << Count << endl;

            ofile.write((const char*)&Count, szWORD);
            beforeVersion = version;
        }

        // cout << "create Index msg1 : " << msg1.toString() << endl;
        // cout << "create Index msg2 : " << msg2.toString() << endl;

        pUpdate = new Update();
        pUpdate->setVersion(version);
        pUpdate->setUpdateType(UPDATETYPE_CREATE_FILE);
        pUpdate->setParam(0, msg1.toString());
        pUpdate->setParam(1, msg2.toString());

        pUpdate->save(ofile);
        ofile.close();
    }

    
    for (int i = 0; i <= version; i++) {
        char filename[256];
        sprintf(filename, "%s/v%05d/index.dat", g_pConfig->getProperty("PatchDirectory").c_str(), i);
        ifstream ifile(filename, ios::in | ios::binary);
        
        if (!ifile) {
            int Count = 0;

            char filename[256];
            sprintf(filename, "%s/v%05d/index.dat", g_pConfig->getProperty("PatchDirectory").c_str(), i);
            ofstream ofile(filename, ios::out | ios::app);
            ofile.write((const char*)&Count, szWORD);
        }
        ifile.close();
    }
}

//--------------------------------------------------------------------------------
//
// main()
//

//
//--------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    if (argc < 3) {
        
        exit(1);
    }

    
    string* Argv;

    Argv = new string[argc];
    for (int i = 0; i < argc; i++)
        Argv[i] = argv[i];

    
    
    

    try {
        if (Argv[1] != "-f") {
            throw Error("Usage : updateserver -f ");
        }

        
        g_pConfig = new Properties();
        g_pConfig->load(Argv[2]);

        // cout << g_pConfig->toString() << endl;

    } catch (Error& e) {
        // cout << e.toString() << endl;
    }

    
    //	scan_Source( g_pConfig->getProperty( "SourceDirectory") );

    /*
    //cout << "============================================================================" << endl;
    for( list<SourceInfo*>::const_iterator itr = SourceInfoList.begin(); itr != SourceInfoList.end(); itr++ ) {
        //cout << "Name : " << (*itr)->getName() << ", Directory : " << (*itr)->getDirectory() << endl;
    }
    //cout << "============================================================================" << endl;
    */

    //	scan_Version( g_pConfig->getProperty("PatchDirectory" ), 0 );

     

    
    //	create_Index();


    //
    
    //
    try {
        struct rlimit rl;
        rl.rlim_cur = RLIM_INFINITY;
        rl.rlim_max = RLIM_INFINITY;
        setrlimit(RLIMIT_CORE, &rl);

        
        g_pUpdateServer = new UpdateServer();

        
        g_pUpdateServer->init();

        
        g_pUpdateServer->start();

    } catch (Throwable& e) {
        
        ofstream ofile("../log/instant.log", ios::out);
        ofile << e.toString() << endl;
        ofile.close();

        
        // cout << e.toString() << endl;

        
        
        g_pUpdateServer->stop();
    }
}
