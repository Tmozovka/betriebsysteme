#include "MyFile.h"
#include <string.h>
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

	printf("Konstruktor von MyFile ist beendet \n");
}
MyFile::~MyFile() {//
	printf("Destruktor von MyFile ist beendet \n");
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

	printf("Konstruktor von MyFile ist beendet \n");
}

void MyFile::showFile()
{
	printf("File's name: %s, user id: %i, group id: %i, size: %i , firstBlock: %i \n \n", name.c_str(), user, group, (int)size, firstBlock);
}

	bool operator == ( MyFile const &f, MyFile const &f2)
			{
		return ((f2.name==f.name) && f2.firstBlock==f.firstBlock);

	}


	MyFile& MyFile::operator = (const MyFile &f)
		{

		name = f.name;//Dateiname
			user = f.user;//Benutzer
			group = f.group;//Gruppen-ID
			size = f.size;//Dateigroesse
			mode = f.mode;//Zugriffsberechtigung
			lastAccess = f.lastAccess;//Zeitpunkt letzter Zugriff (atime)
			lastMod = f.lastMod;//letzte Ver�nderung (mtime)
			lastStatusChange = f.lastStatusChange;//letzter Status�nderung (ctime)
			firstBlock = f.firstBlock;//Zeiger auf ersten Block (u_int32_t BlockNo)
			return *this;
		}

//get
//Als Parameter fuer addFile verwendbar
mode_t MyFile::getMode() {return mode;}

time_t  MyFile::getLastMod(){return lastMod;}
off_t  MyFile::getSize(){return size;}
int  MyFile::getFirstBlock(){return firstBlock;}
string  MyFile::getName(){return name;}


//set
void  MyFile::setLastMod(time_t t){ lastMod=t;}
void  MyFile::setSize(off_t s){ size=s;}
void   MyFile::setName(string n){ name=n;}
void  MyFile::setLastAccess(time_t t){lastAccess=t;}
