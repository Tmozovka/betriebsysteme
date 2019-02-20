#include "MyFile.h"
#include <string.h>
#include <stdlib.h>
using namespace std;
//Define the constructor

MyFile::MyFile(string cname, uid_t cuser, gid_t cgroup, off_t csize,
		mode_t cmode, time_t clastAccess, time_t clastMod,
		time_t clastStatusChange, int cfirstBlock) {
	name = cname; //Dateiname
	user = cuser; //Benutzer
	group = cgroup; //Gruppen-ID
	size = csize; //Dateigroesse
	mode = cmode; //Zugriffsberechtigung
	lastAccess = clastAccess; //Zeitpunkt letzter Zugriff (atime)
	lastMod = clastMod; //letzte Ver�nderung (mtime)
	lastStatusChange = clastStatusChange; //letzter Status�nderung (ctime)
	firstBlock = cfirstBlock; //Zeiger auf ersten Block (u_int32_t BlockNo)
	puffer=new char[size+1];
	for(int i =0;i<size+1;i++)
		puffer[i]=char(0);

	//printf("Konstruktor von MyFile ist beendet \n");
}
MyFile::~MyFile() { //
	//printf("Destruktor von MyFile ist beendet \n");
	delete [] puffer;
}

//Define the constructor

MyFile::MyFile() {
	name = ""; //Dateiname
	user = NULL; //Benutzer
	group = NULL; //Gruppen-ID
	size = NULL; //Dateigroesse
	mode = NULL; //Zugriffsberechtigung
	lastAccess = time(NULL); //Zeitpunkt letzter Zugriff (atime)
	lastMod = time(NULL); //letzte Ver�nderung (mtime)
	lastStatusChange = time(NULL); //letzter Status�nderung (ctime)
	firstBlock = -1; //Zeiger auf ersten Block (u_int32_t BlockNo)
	puffer=new char[size+1];
	for(int i =0;i<size+1;i++)
			puffer[i]=char(0);

	//printf("Konstruktor von MyFile ist beendet \n");
}

int MyFile::init(MyFile * f) {

	name=f->name;
	user = f->user;
	group = f->group;
	size = f->size;
	mode = f->mode;
	lastAccess = f->lastAccess;
	lastMod = f->lastMod; //letzte Ver�nderung (mtime)
	lastStatusChange = f->lastStatusChange; //letzter Status�nderung (ctime)
	firstBlock = f->firstBlock; //Zeiger auf ersten Block (u_int32_t BlockNo)
	puffer=new char[size+1];
	for(int i =0;i<size+1;i++)
			puffer[i]=char(0);

	return 1;

}

void MyFile::showFile() {
	printf(
			"File's name: %s, user id: %i, group id: %i, size: %i , firstBlock: %i \n \n",
			name.c_str(), user, group, (int) size, firstBlock);
}

//operators
bool operator ==(MyFile const &f, MyFile const &f2) {
	return ((f2.name == f.name) && f2.firstBlock == f.firstBlock && f2.size==f.size);

}

MyFile& MyFile::operator =(const MyFile &f) {

	name = f.name; //Dateiname
	user = f.user; //Benutzer
	group = f.group; //Gruppen-ID
	size = f.size; //Dateigroesse
	mode = f.mode; //Zugriffsberechtigung
	lastAccess = f.lastAccess; //Zeitpunkt letzter Zugriff (atime)
	lastMod = f.lastMod; //letzte Ver�nderung (mtime)
	lastStatusChange = f.lastStatusChange; //letzter Status�nderung (ctime)
	firstBlock = f.firstBlock; //Zeiger auf ersten Block (u_int32_t BlockNo)
	return *this;
}

//to Blocks
char * MyFile::writeBlock() {
	char * buf = new char[BLOCK_SIZE];
	char * readBuf = new char[BLOCK_SIZE];
	writeFileChar(buf);

	//printf("buf mit tryFile: %s \n", buf);
	//printf("write to block \n");

	BlockDevice blocks;

	blocks.create("containerFileTest.bin");
	blocks.write(0, buf);
	blocks.read(0, readBuf);
	if (strcmp(buf, readBuf) != 0) {
		throw std::invalid_argument(
				"Differences between written and read Blocks");
	}
	remove("containerFileTest.bin");
	delete [] buf;
	return readBuf;
}

void MyFile::writeFileChar(char * block) {

	char * name = new char[FILE_NAME_SIZE];
	strcpy(name, this->name.c_str());
	resize(name, this->name.length(), FILE_NAME_SIZE);

	char * user = new char[FILE_USER_SIZE];
	//itoa((int)this->user,user);
	strcpy(user, to_string(this->user).c_str());
	resize(user, to_string(this->user).length(), FILE_USER_SIZE);

	char * group = new char[FILE_GROUP_SIZE];
	//itoa((int)this->group,group);
	strcpy(group, to_string(this->group).c_str());
	resize(group, to_string(this->group).length(), FILE_GROUP_SIZE);

	char * size = new char[FILE_SIZE_SIZE];
	strcpy(size, to_string(this->size).c_str());
	resize(size, to_string(this->size).length(), FILE_SIZE_SIZE);

	char * mode = new char[FILE_MODE_SIZE];
	strcpy(mode, to_string(this->mode).c_str());
	resize(mode, to_string(this->mode).length(), FILE_MODE_SIZE);

	char * lastAccess = new char[FILE_ACCESS_SIZE];
	strcpy(lastAccess, to_string(this->lastAccess).c_str());
	resize(lastAccess, to_string(this->lastAccess).length(), FILE_ACCESS_SIZE);

	char * lastMod = new char[FILE_MOD_SIZE];
	strcpy(lastMod, to_string(this->lastMod).c_str());
	resize(lastMod, to_string(this->lastMod).length(), FILE_MOD_SIZE);

	char * lastStatusChange = new char[FILE_STATUS_SIZE];
	strcpy(lastStatusChange, to_string(this->lastStatusChange).c_str());
	int size1 = to_string(this->lastStatusChange).length();
	resize(lastStatusChange, size1, FILE_STATUS_SIZE);

	char * firstBlock = new char[FILE_BLOCK_SIZE];
	strcpy(firstBlock, to_string(this->firstBlock).c_str());
	resize(firstBlock, to_string(this->firstBlock).length(), FILE_BLOCK_SIZE);

	//char * block = new char[BLOCK_SIZE];
	//printf("pointer %hd \n", ()block);
	strcpy(block, name);
	strcat(block, user);
	strcat(block, group);
	strcat(block, size);
	strcat(block, mode);
	strcat(block, lastAccess);
	strcat(block, lastMod);
	strcat(block, lastStatusChange);
	strcat(block, firstBlock);

	//resize(block, MAX_FILE_SIZE, BLOCK_SIZE);
	string tstr(block);
	for(int i=tstr.length();i<BLOCK_SIZE;i++)
		block[i]=char(0);
	printf("write to block : %s \n ", block);

	printf("MAX SIZE: %i, block size: %i",MAX_FILE_SIZE,tstr.length());

	delete [] name;
	delete [] user;
	delete [] group;
	delete [] size;
	delete [] mode;
	delete [] lastAccess;
	delete [] lastMod;
	delete [] lastStatusChange;
	delete [] firstBlock;
	//return block;

}
void MyFile::readBlock(char ** readBuf,int blockNo, BlockDevice blocks) {
	char * buf = *readBuf;
	blocks.read(blockNo, buf);
	//return buf;
}

void MyFile::writeVar(char * varT, char * buf, int size, int &count) {

	//char * varT = new char[size];
	int sizeVar = 0;
	while (sizeVar != size - 1) {

		while (*(buf) != '_') {
			*(varT++) = *(buf++);
			sizeVar++;
			(count)++;

			if (sizeVar == size)
				break;

		}

		if (*(buf) == '_') {
			buf++;
			(count)++;
		}

		if (*(buf) != '_')
			break;

	}

	*varT = char(0);
	varT -= sizeVar;

	//return varT;

}

void MyFile::convertS(string & var, char * varT) {
	var = varT;
}

template<class T>
void MyFile::convertI(T & var, char * varT) {
	var = atoi(varT);
}

MyFile::MyFile(char * buf) {

	int count = 0;
printf("start name \n");
	//name
	char * varT = new char[FILE_NAME_SIZE];
	writeVar(varT,buf, FILE_NAME_SIZE, count);
	convertS(this->name, varT);
	buf += count;
	count = 0;
	delete [] varT;

	//user
	varT = new char[FILE_USER_SIZE];
	writeVar(varT,buf, FILE_USER_SIZE, count);
	convertI(this->user,varT);
	buf += count;
	count = 0;
	delete [] varT;

	//group
	varT = new char[FILE_GROUP_SIZE];
	writeVar(varT,buf, FILE_GROUP_SIZE, count);
	convertI(this->group, varT);
	buf += count;
	count = 0;
	delete [] varT;

	//size
	varT = new char[FILE_SIZE_SIZE];
	writeVar(varT, buf, FILE_SIZE_SIZE, count);
	convertI(this->size, varT);
	buf += count;
	count = 0;
	delete [] varT;

	//mode
	varT = new char[FILE_MODE_SIZE];
	writeVar(varT,buf, FILE_MODE_SIZE, count);
	convertI(this->mode, varT);
	buf += count;
	count = 0;
	delete [] varT;

	//lastAccess
	varT = new char[FILE_ACCESS_SIZE];
	writeVar(varT,buf, FILE_ACCESS_SIZE, count);
	convertI(this->lastAccess, varT);
	buf += count;
	count = 0;
	delete [] varT;

	//lastMod
	varT = new char[FILE_MOD_SIZE];
	writeVar(varT,buf, FILE_MOD_SIZE, count);
	convertI(this->lastMod, varT);
	buf += count;
	count = 0;
	delete [] varT;

	//lastStatusChange
	varT = new char[FILE_STATUS_SIZE];
	writeVar(varT,buf, FILE_STATUS_SIZE, count);
	convertI(this->lastStatusChange, varT);
	buf += count;
	count = 0;
	delete [] varT;

	//firstBlock
	varT = new char[FILE_BLOCK_SIZE];
	writeVar(varT,buf, FILE_BLOCK_SIZE, count);
	convertI(this->firstBlock, varT);
	buf += count;
	count = 0;
	delete [] varT;

	//printf("Konstruktor von MyFile ist beendet \n");

}
//get
//Als Parameter fuer addFile verwendbar
mode_t MyFile::getMode() {
	return mode;
}

time_t MyFile::getLastMod() {
	return lastMod;
}
off_t MyFile::getSize() {
	return size;
}
int MyFile::getFirstBlock() {
	return firstBlock;
}
string MyFile::getName() {
	return name;
}

//set
void MyFile::setLastMod(time_t t) {
	lastMod = t;
}
void MyFile::setSize(off_t s) {
	size = s;
}
void MyFile::setName(string n) {
	name = n;
}
void MyFile::setLastAccess(time_t t) {
	lastAccess = t;
}

void MyFile::setFirstBlock(int i) {
	firstBlock = i;
}

void MyFile::resize(char * text, int oldSize, int newSize) {

	int i = newSize - oldSize;
	if (i == 0)
		return;

	text += oldSize;
	while (i != 0) {
		*(text++) = '_';
		i--;
	}
	*(text - 1) = '\0';
	text -= newSize;

}

void  MyFile::writeFromPuffer(char * buf)
{
	for(int i=0;i<size+1; i++)
	{
		buf[i]=puffer[i];
	}
}
