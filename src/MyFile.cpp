#include "MyFile.h"
#include <string.h>
#include <stdlib.h>
using namespace std;
//Define the constructor
//TODO FEHLE FINDEN
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

	//printf("Konstruktor von MyFile ist beendet \n");
}
MyFile::~MyFile() { //
	//printf("Destruktor von MyFile ist beendet \n");
}

//Define the constructor
//TODO FEHLE FINDEN
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

	//printf("Konstruktor von MyFile ist beendet \n");
}

void MyFile::showFile() {
	printf(
			"File's name: %s, user id: %i, group id: %i, size: %i , firstBlock: %i \n \n",
			name.c_str(), user, group, (int) size, firstBlock);
}

//operators
bool operator ==(MyFile const &f, MyFile const &f2) {
	return ((f2.name == f.name) && f2.firstBlock == f.firstBlock);

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
char * MyFile::writeBlock(){
	char * buf = new char [512];
			char * readBuf = new char [512];
			buf = this->writeFileChar();

			//printf("buf mit tryFile: %s \n", buf);
			//printf("write to block \n");

			BlockDevice  blocks ;

			blocks.create("containerFileTest.bin");
			blocks.write(0,buf);
			blocks.read(0,readBuf);
			if(strcmp(buf, readBuf)!=0){
				throw std::invalid_argument( "Differences between written and read Blocks" );
			}
			return readBuf;
}

char * MyFile::writeFileChar() {

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

	char * block = new char[BLOCK_SIZE];
	strcpy(block, name);
	strcat(block, user);
	strcat(block, group);
	strcat(block, size);
	strcat(block, mode);
	strcat(block, lastAccess);
	strcat(block, lastMod);
	strcat(block, lastStatusChange);
	strcat(block, firstBlock);

	resize(block, MAX_FILE_SIZE, BLOCK_SIZE);
	printf("write to block : %s \n ", block);

	return block;

}
char * MyFile::readBlock(int blockNo, BlockDevice blocks){
	char * buf = new char [512];
	blocks.read(blockNo,buf);
	return buf;
}

char * MyFile::writeVar(char * buf, int size, int &count) {

	char * varT = new char[size];
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

	return varT;

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

	//name
	convertS(this->name, writeVar(buf, FILE_NAME_SIZE, count));
	buf += count;
	count = 0;

	//user
	convertI(this->user, writeVar(buf, FILE_USER_SIZE, count));
	buf += count;
	count = 0;

	//group
	convertI(this->group, writeVar(buf, FILE_GROUP_SIZE, count));
	buf += count;
	count = 0;

	//size
	convertI(this->size, writeVar(buf, FILE_SIZE_SIZE, count));
	buf += count;
	count = 0;

	//mode
	convertI(this->mode, writeVar(buf, FILE_MODE_SIZE, count));
	buf += count;
	count = 0;

	//lastAccess
	convertI(this->lastAccess, writeVar(buf, FILE_ACCESS_SIZE, count));
	buf += count;
	count = 0;

	//lastMod
	convertI(this->lastMod, writeVar(buf, FILE_MOD_SIZE, count));
	buf += count;
	count = 0;

	//lastStatusChange
	convertI(this->lastStatusChange, writeVar(buf, FILE_STATUS_SIZE, count));
	buf += count;
	count = 0;

	//firstBlock
	convertI(this->firstBlock, writeVar(buf, FILE_BLOCK_SIZE, count));
	buf += count;
	count = 0;

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
