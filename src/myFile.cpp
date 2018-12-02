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
	user = "";//Benutzer
	group = NULL;//Gruppen-ID
	size = NULL;//Dateigroesse
	mode = NULL;//Zugriffsberechtigung
	lastAccess = time(NULL);//Zeitpunkt letzter Zugriff (atime)
	lastMod = time(NULL);//letzte Ver�nderung (mtime)
	lastStatusChange = time(NULL);//letzter Status�nderung (ctime)
	firstBlock = -1;//Zeiger auf ersten Block (u_int32_t BlockNo)
}
