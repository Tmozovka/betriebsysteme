#pragma once
#include <string>
#include <unistd.h>
#include <stdexcept>
#include <stdio.h>
#include "myfs-structs.h"
#include "blockdevice.h"
using namespace std;

class MyFile {
private:
	/*
off_t is a long int: format = "%ld"
size_t is an unsigned int: format = "%u"*/

	string name; //Dateiname 255
	uid_t user; //Benutzer  unsigned int 10
	gid_t group; //Gruppen-ID unsigned int 10
	off_t size; //Dateigroesse  10  off_t is a long int: format = "%ld"
	mode_t mode; //Zugriffsberechtigung 1nt 10
	time_t lastAccess; //Zeitpunkt letzter Zugriff (atime) 15
	time_t lastMod; //letzte Ver�nderung (mtime) 15
	time_t lastStatusChange; //letzter Status�nderung (ctime) 15
	char * puffer=new char [BLOCK_SIZE];
	int firstBlock; //Zeiger auf ersten Block 5
public:
	MyFile(string cname, uid_t cuser, gid_t cgroup, off_t csize, mode_t cmode,
			time_t clastAccess, time_t clastMod, time_t clastStatusChange,
			int cfirstBlock);

	MyFile();

	MyFile(const MyFile &f);
	MyFile(MyFile * f);

	~MyFile();
	MyFile(char *);
	//get
	mode_t getMode();
	time_t getLastMod();
	off_t getSize();
	int getFirstBlock();
	string getName();

	//set
	void setLastMod(time_t t);
	void setSize(off_t s);
	void setName(string n);
	void setLastAccess(time_t t);
	void setFirstBlock(int i);
	void writeFromPuffer(char * buf);

	void showFile();

	friend bool operator ==(MyFile const &f, MyFile const& f2);
	MyFile& operator =(const MyFile &f);

	void writeFileChar(char * b);
	char * writeBlock();
	void readBlock(char ** c, int blockNo, BlockDevice  blocks);

	void writeVar(char *,  char * buf, int size, int & count );
	//void writeVar(int * var, char * buf, int size, int & count );
	void convertS(string & var, char * varT);
	template<class T>
	void convertI(T & var, char * varT);
	int init(MyFile * f);

	void resize(char * text, int oldSize, int newSize);
	char getPuffer( int i){return puffer[i];};
	void setPuffer(int i, char c){puffer[i]=c;};
};

