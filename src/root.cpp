#include <iostream>
#include "root.h"
#include "myfs-structs.h"
#include <string>
#include "macros.h"

MyRoot::MyRoot(MyFile firstfile) {
	sizeRoot = 1;
	addressRoot = &firstfile;
	files.push_front(firstfile);

	//printf("Konstruktor von MyRoot ist beendet \n");
}

MyRoot::MyRoot() {
	sizeRoot = 0;
	addressRoot = NULL;

	//printf("Konstruktor von MyRoot ist beendet \n");
}

//MyFile(string cname, uid_t cuser, gid_t cgroup, off_t csize, mode_t cmode, time_t clastAccess,
//time_t clastMod, time_t clastStatusChange, int cfirstBlock);

MyRoot::MyRoot(string name, off_t size, mode_t mode, int firstBlock) {
	//firstfile -> first block
	sizeRoot = 1;
	MyFile * firstfile = new MyFile(name, getuid(), getgid(), size, mode,
			time(NULL), time(NULL), time(NULL), firstBlock);
	addressRoot = firstfile;
	files.push_front(*firstfile);
	//printf("Konstruktor von MyRoot ist beendet \n");
}

MyRoot::MyRoot(char **array) {
	sizeRoot = sizeof(array);
	for (int i = 0; i <= sizeRoot; i++, array++) {

		MyFile *f = new MyFile(*array);
		this->addFile(f->getName(), f->getSize(), f->getMode(), f->getLastMod(),
				f->getFirstBlock());
		delete f;
	}
	std::list<MyFile>::iterator it = files.begin();
	addressRoot = &(*it);

}

MyRoot::~MyRoot() {
	sizeRoot = 0;
	files.clear();
	addressRoot = NULL;
	std::list<MyFile>::iterator it = files.begin();
	/*
	 while (it != files.end())
	 {
	 delete &(*(it++));
	 }*/

//printf("Destruktor von MyRoot ist beendet \n");
}

int MyRoot::addFile(string name, off_t size, mode_t mode, time_t st_mtime,
		int firstBlock) {
//Wird das uebergebene Size abgefragt oder das des Roots?
//Es sollte ja das des roots angesprochen werden
//Und muesste es nicht >= heissen?
	if (sizeRoot + 1 > NUM_DIR_ENTRIES) {
		printf("too many files in Root \n");
		return -ENOMSG;
	}

	if (existName(name)) {
		printf("File's %s name is already exist \n", name.c_str());
		return -EEXIST;
	}
	if (name.length() > NAME_LENGTH) {
		printf("File's %s name is too big \n", name.c_str());
		return -EFBIG;
	}

//Speichern von Name, Dateigroesse und Zugriffsrechte pro Datei
	/*MyFile * firstfile = new MyFile(name, getuid(), getgid(), size, mode,
	 time(NULL), time(NULL), time(NULL), firstBlock);*/
	const MyFile * f = new MyFile(name, getuid(), getgid(), size, mode,
			time(NULL), st_mtime, time(NULL), firstBlock);

//Wenn erstes File
	if (sizeRoot == 1) {
		addressRoot = new MyFile(name, getuid(), getgid(), size, mode,
				time(NULL), st_mtime, time(NULL), firstBlock);
	}

	sizeRoot++;
	files.push_back(*f);
	return 0;
}

bool MyRoot::existName(string name) {
	std::list<MyFile>::iterator it = files.begin();

	while (it != files.end()) {
		if ((it++)->getName() == name) {
			//printf("Dateiname existiert bereits\n");
			return true;
		}
	}

	return false;
}

bool MyRoot::compareRoots(MyRoot * root) {
	bool ret = true;
	string* thisRoot = this->getArray();
	string* similarRoot = root->getArray();

	MyFile *f1 = new MyFile();
	MyFile *f2 = new MyFile();

	if (this->sizeRoot != root->sizeRoot)
		return false;

	for (int k = 0; k <= this->sizeRoot; k++) {
		if ((thisRoot[k] == similarRoot[k]) == false) {
			return false;
		}
		this->getFile(thisRoot[k], f1);
		this->getFile(similarRoot[k], f2);

		if (f1 != f2) {
			ret = false;
			break;
		}

		/*if((f1->user==f2->user)==false){
		 return false;
		 } else if((f1->size==f2->size)==false){
		 return false;
		 }else if((f1->mode==f2->mode)==false){
		 return false;
		 }else if((f1->lastMod==f2->lastMod)==false){
		 return false;
		 }else if((f1->firstBlock==f2->firstBlock)==false){
		 return false;
		 }*/

	}
	delete f1;
	delete f2;
	return ret;

}

void MyRoot::showRoot() {
	printf(
			"************************************************************ \nRoot: \n");
	std::list<MyFile>::iterator it = files.begin();
	int count = 1;
	while (it != files.end()) {
		printf("%i: \n", count++);
		(it)->showFile();
		it++;
	}
	printf("************************************************************ \n");
}

void MyRoot::showFile(string path) {
	printf(
			"************************************************************ \nRoot: \n");
	std::list<MyFile>::iterator it = files.begin();
	while (it != files.end()) {
		if(it->getName()==path)
		{
			(it)->showFile();
			break;
		}
		it++;
	}
	printf("************************************************************ \n");
}
int MyRoot::getFile(string name, MyFile * f) {

//LOG("Starting to look for file in root");
	std::list<MyFile>::iterator it = files.begin();

	while (it->getName() != name) {
		//LOGF("Found this file:%s",it->getName());
		it++;
		if (it == files.end()) {
			//printf("no such file in root \n");
			return -1;
		}
	}

	*f = *it;
	return 0;

}

int MyRoot::setSize(string name, off_t size) {
	std::list<MyFile>::iterator it = files.begin();

	while (it->getName() != name) {
		it++;
		if (it == files.end()) {
			return -1;
		}
	}
	it->setSize(size);

	return 0;
}

int MyRoot::copyFile(string name, MyFile* file) {

	std::list<MyFile>::iterator it = files.begin();

	while (it->getName() != name) {
		//LOGF("Found this file:%s",it->getName());
		it++;
		if (it == files.end()) {
			//printf("no such file in root \n");
			return -1;
		}
	}

	return (it->init(file));

}

int MyRoot::deleteFile(string name) {
//In Root Verzeichnis Datei loeschen
	if (files.size() != 0) {
		MyFile filetodelete;

		if (getFile(name, &filetodelete) == -1) {
			// no such file
			return -1;
		}
		files.remove(filetodelete);
		sizeRoot--;
		return 0;
	}
	return -1;
}

/*void MyRoot::getArray(string ** array)
 {
 string *arr = *array;
 arr = new string[files.size()];
 int k = 0;
 for (MyFile i: files) {
 arr[k++] = i.getName();
 }


 }*/

string * MyRoot::getArray() {
	string *arr = new string[sizeRoot];
	std::list<MyFile>::iterator it = files.begin();

	int k = 0;
	while (it != files.end()) {
		arr[k++] = it->getName();
		it++;
	}

	return arr;
}
/*
 char** MyRoot::writeRootChar() {
 char ** block;		//[sizeRoot][BLOCK_SIZE];
 block = new char *[sizeRoot];

 std::list<MyFile>::iterator it = files.begin();

 while (it != files.end()) {
 *block = new char[BLOCK_SIZE];
 *block = it->writeFileChar();
 block++;
 it++;
 }

 return block;

 }
 char ** MyRoot::writeBlocks(BlockDevice blocks) {
 char ** writeBlockChar = new char *[this->getSize()];

 writeBlockChar = this->writeRootChar();

 char * buf = new char[512];
 char * readBuf = new char[512];

 string *nameArray = this->getArray();

 for (u_int32_t k = 0; k <= this->getSize(); k++) {
 //this>getFile(nameArray[k],new MyFile());
 buf = *writeBlockChar;

 //blocks.create("containerRootTest.bin"); //Ist der alte Fehler beseitigt?

 blocks.write(k, buf);
 blocks.read(k, readBuf);
 if (strcmp(buf, readBuf) != 0) {
 throw std::invalid_argument(
 "Differences between written and read Blocks");
 }

 writeBlockChar++;
 }

 return writeBlockChar;
 }

 char** MyRoot::readBlocks(BlockDevice blocks) {
 //Sollen mehrere ausgelesen werden?
 //Wenn ja, welche?

 char ** block;
 block = new char *[sizeRoot];

 std::list<MyFile>::iterator it = files.begin();

 for (int i = 0; it != files.end(); it++, i++) {
 *block = new char[BLOCK_SIZE];
 *block = it->readBlock(i, blocks);
 block++;
 it++;
 }

 return block;

 }*/
/////////////////////////////////Tanja's funktionen , um Root in BlockDevice zu schreiben/////////////////////////////////////////////////////
void MyRoot::writeBlockDevice(BlockDevice * blocks, int start) {

	printf("start for root writeBlockDevice \n");

	char * buf = new char[BLOCK_SIZE];

	printf("sizeRoot %i \n ", sizeRoot);

	strcpy(buf, to_string(this->sizeRoot).c_str());
//resize(buf, to_string(this->sizeRoot).length(), BLOCK_SIZE);

//printf("buf %i : %s \n", start, buf);

	for (int i = to_string(this->sizeRoot).length(); i < BLOCK_SIZE; i++)
		buf[i] = char(0);

	blocks->write(start++, buf);

	std::list<MyFile>::iterator it = files.begin();

	while (it != files.end()) {
		it->writeFileChar(buf);
		//printf("buf %i : %s \n", start, buf);
		blocks->write(start++, buf);
		it++;
	}

	delete[] buf;

}

MyRoot::MyRoot(BlockDevice * blocks, int start) {
	printf("start MyRoot(BlockDevice * blocks, int start) \n");
	char * buf = new char[BLOCK_SIZE];
	blocks->read(start++, buf);
	sizeRoot = atoi(buf);
	printf("size root: %i \n", sizeRoot);

	if (sizeRoot != 0) {

		for (int i = 0; i < sizeRoot; i++) {
			blocks->read(start++, buf);
			printf("blocks.read buf %i : %s \n", start - 1, buf);
			MyFile * f = new MyFile(buf);
			printf("create myfile sucess \n");
			files.push_back(*f);

		}

		std::list<MyFile>::iterator it = files.begin();
		addressRoot = &(*it);

	} else
		addressRoot = NULL;

	delete[] buf;

}

void MyRoot::read(int start, BlockDevice * blocks) {
	printf("start MyRoot(BlockDevice * blocks, int start) \n");
	char * buf = new char[BLOCK_SIZE];
	blocks->read(start++, buf);
	sizeRoot = atoi(buf);
	delete[] buf;

	if (sizeRoot != 0) {

		for (int i = 0; i < sizeRoot; i++) {
			buf = new char[BLOCK_SIZE];
			blocks->read(start++, buf);
			printf("blocks.read buf %i : %s \n", start - 1, buf);
			printf("HALLOOO %s \n", buf);
			MyFile * file = new MyFile(buf);
			files.push_back(*file);
			delete file;
			delete[] buf;

		}

		std::list<MyFile>::iterator it = files.begin();
		addressRoot = &(*it);

	} else
		addressRoot = NULL;
}

bool operator ==(MyRoot const &r1, MyRoot const& r2) {
	if (r1.sizeRoot != r2.sizeRoot)
		return false;

	std::list<MyFile>::const_iterator it1 = r1.files.begin();
	std::list<MyFile>::const_iterator it2 = r2.files.begin();

	for (int i = 0; it1 != r1.files.end(); it1++, i++, it2++) {
		if (!(*it1 == *it2))
			return false;
	}

	return true;
}

bool operator !=(MyRoot const &r1, MyRoot const& r2) {
	return !(r1 == r2);
}

void MyRoot::resize(char * text, int oldSize, int newSize) {

	int i = newSize - oldSize;
	text += oldSize - 1;
	while (i != 0) {
		*(text++) = char(0);
		i--;
	}
//*(text-1)='\0';
	text -= newSize;

}

void MyRoot::writeFromPuffer(const char * name, char * buf) {
	if (existName(name)) {
		std::list<MyFile>::iterator it = files.begin();

		while (it != files.end()) {
			if ((it)->getName() == name) {
				for (int i = 0; i < BLOCK_SIZE; i++)
					buf[i] = it->puffer[i];
				break;
			}
			it++;

		}
	}
}

void MyRoot::writeToPuffer(const char * name, char * buf) {
	if (existName(name)) {
		std::list<MyFile>::iterator it = files.begin();

		while (it != files.end()) {
			if ((it)->getName() == name) {
				for (int i = 0; i < BLOCK_SIZE; i++)
					it->puffer[i]=buf[i];
				break;
			}
			it++;

		}
	}
}

