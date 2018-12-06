#pragma once
#include <string>
#include <stdlib.h>
using namespace std;

class MyFile{
private:
	string name;//Dateiname
	uid_t user;//Benutzer
	gid_t group;//Gruppen-ID
	off_t size;//Dateigroesse
	mode_t mode;//Zugriffsberechtigung
	time_t lastAccess;//Zeitpunkt letzter Zugriff (atime)
	time_t lastMod;//letzte Ver�nderung (mtime)
	time_t lastStatusChange;//letzter Status�nderung (ctime)
	int firstBlock;//Zeiger auf ersten Block
public:
	MyFile(string cname, uid_t cuser, gid_t cgroup, off_t csize, mode_t cmode, time_t clastAccess,
			time_t clastMod, time_t clastStatusChange, int cfirstBlock);

	MyFile();

	~MyFile();
	//get
	time_t getLastMod();
	off_t getSize();
	int getFirstBlock();
	string getName();

	//set
	void setLastMod(time_t t);
	void setSize(off_t s);
	void  setName(string n);
	void setLastAccess(time_t t);

	void showFile();
	//operatoren
	bool operator==(MyFile f);

};
