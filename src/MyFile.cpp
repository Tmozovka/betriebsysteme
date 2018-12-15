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

	printf("Konstruktor von MyFile ist beendet \n");
}
MyFile::~MyFile() { //
	printf("Destruktor von MyFile ist beendet \n");
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

	printf("Konstruktor von MyFile ist beendet \n");
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

char * MyFile::writeBlock(int i)
{

	const char * name = new char [FILE_NAME_SIZE];
	name = this->name.c_str();

	const char * user = new char [FILE_USER_SIZE];
	//itoa((int)this->user,user);
	user = to_string(this->user).c_str();

	const char * group = new char [FILE_GROUP_SIZE];
	//itoa((int)this->group,group);
	group = to_string(this->group).c_str();

	const char * size = new char [FILE_SIZE_SIZE];
	size = to_string(this->size).c_str();

	const char * mode = new char [FILE_MODE_SIZE];
	mode = to_string(this->mode).c_str();

	const char * lastAccess = new char [FILE_ACCESS_SIZE];
	lastAccess = to_string(this->lastAccess).c_str();

	const char * lastMod = new char [FILE_MOD_SIZE];
	lastMod = to_string(this->lastMod).c_str();

	const char * lastStatusChange = new char [FILE_STATUS_SIZE];
	lastStatusChange = to_string(this->lastStatusChange).c_str();

	const char * firstBlock = new char [FILE_BLOCK_SIZE];
	firstBlock = to_string(this->firstBlock).c_str();


	/*char* name_with_extension;
	name_with_extension = malloc(strlen(name)+1+4); // make space for the new string (should check the return value ...)
	strcpy(name_with_extension, name); // copy name into the new var
	strcat(name_with_extension, extension);*/

	char * block = new char [BLOCK_SIZE];
	strcpy(block, name);
	strcat(block, user);
	strcat(block, group);
	strcat(block, size);
	strcat(block, mode);
	strcat(block, lastAccess);
	strcat(block, lastMod);
	strcat(block, lastStatusChange);
	strcat(block, firstBlock);

	printf ("write to block : %s \n ", block);

	resize(block, MAX_FILE_SIZE ,BLOCK_SIZE);


	return block;

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
	text += oldSize - 1;
	while (i != 0) {
		*(text++) = char(0);
		i--;
	}
	//*(text-1)='\0';
	text -= newSize;

}
