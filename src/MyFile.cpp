#include "MyFile.h"
#include <string>
#include <stdlib.h>
using namespace std;
//Define the constructor
//TODO FEHLE FINDEN
MyFile::MyFile(string cname, uid_t cuser, gid_t cgroup, off_t csize, mode_t cmode, time_t clastAccess,
		time_t clastMod, time_t clastStatusChange, int cfirstBlock) {
	name = cname;//Dateiname
	user = cuser;//Benutzer
	group = cgroup;//Gruppen-ID
	size = csize;//Dateigroesse
	mode = cmode;//Zugriffsberechtigung
	lastAccess = clastAccess;//Zeitpunkt letzter Zugriff (atime)
	lastMod = clastMod;//letzte Ver�nderung (mtime)
	lastStatusChange = clastStatusChange;//letzter Status�nderung (ctime)
	firstBlock = cfirstBlock;//Zeiger auf ersten Block (u_int32_t BlockNo)
}
MyFile::~MyFile() {//
	/*delete[] name;//Dateiname
	delete[] user;//Benutzer
	delete[] group;//Gruppen-ID
	delete[] size;//Dateigroesse
	delete[] mode;//Zugriffsberechtigung
	delete[] lastAccess;//Zeitpunkt letzter Zugriff (atime)
	delete[] lastMod;//letzte Ver�nderung (mtime)
	delete[] lastStatusChange;//letzter Status�nderung (ctime)
	delete[] firstBlock;//Zeiger auf ersten Block*/
}

//Define the constructor
//TODO FEHLE FINDEN
MyFile::MyFile() {
	name = "";//Dateiname
	user = NULL;//Benutzer
	group = NULL;//Gruppen-ID
	size = NULL;//Dateigroesse
	mode = NULL;//Zugriffsberechtigung
	lastAccess = time(NULL);//Zeitpunkt letzter Zugriff (atime)
	lastMod = time(NULL);//letzte Ver�nderung (mtime)
	lastStatusChange = time(NULL);//letzter Status�nderung (ctime)
	firstBlock = -1;//Zeiger auf ersten Block (u_int32_t BlockNo)
}

void MyFile::showFile()
{
	printf("File's name: %s, user id: %i, group id: %i, size: %i , firstBlock: %i \n \n", name.c_str(), user, group, (int)size, firstBlock);
}

bool MyFile::operator==(MyFile f)
	{

	    return (name==f.name && firstBlock==f.firstBlock);
	}

//get
time_t  MyFile::getLastMod(){return lastMod;}
off_t  MyFile::getSize(){return size;}
int  MyFile::getFirstBlock(){return firstBlock;}
string  MyFile::getName(){return name;}

//set
void  MyFile::setLastMod(time_t t){ lastMod=t;}
void  MyFile::setSize(off_t s){ size=s;}
void   MyFile::setName(string n){ name=n;}
void  MyFile::setLastAccess(time_t t){lastAccess=t;}
