//
//  myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 02.08.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

// For documentation of FUSE methods see https://libfuse.github.io/doxygen/structfuse__operations.html

#undef DEBUG

// TODO: Comment this to reduce debug messages
#define DEBUG
#define DEBUG_METHODS
#define DEBUG_RETURN_VALUES

#include "macros.h"
#include "myfs.h"
#include "myfs-info.h"
#include "fat.h"
#include "superBlock.h"
#include "myfs.h"

// fragen zur Folie 31
//TODO Wie kann ich die anzahl geoefnete Dateien abrufen?
// wann soll ich die Bloecke schliessen? Nach dem oeffnen oder nach dem Lesen auch
// wie kann ich pruefen was in gepuffertem Block liegt?
//in folie steht read (n, p), aber wir haben nur read()
using namespace std;

MyFS* MyFS::_instance = NULL;

MyFS* MyFS::Instance() {
	if (_instance == NULL) {
		_instance = new MyFS();
	}
	return _instance;
}

MyFS::MyFS() {
	this->logFile = stderr;

	sp = new SuperBlock();
	dmap = new dMap();
	fat = new MyFAT();
	root = new MyRoot();
	blocks = new BlockDevice();
	// printf("Konstruktor von MyFS ist beendet \n");
	//LOG("Konstruktor von MyFS ist beendet \n");
}

MyFS::MyFS(char * nameCont) {
	this->logFile = stderr;

	blocks = new BlockDevice();
	sp = new SuperBlock();
	dmap = new dMap();
	fat = new MyFAT();
	root = new MyRoot();

	blocks->open(nameCont);
	dmap->read(DMAP_START, blocks);
	fat->read(FAT_START, blocks);
	root->read(ROOT_START, blocks);

	// printf("Konstruktor von MyFS ist beendet \n");
	//LOG("Konstruktor von MyFS ist beendet \n");
}

MyFS::~MyFS() {

	delete sp;
	delete dmap;
	delete fat;
	delete root;
	delete blocks;
	//printf("Destruktor von MyFS ist beendet \n");
	//LOG("Destruktor von MyFS ist beendet \n");

}

bool operator ==(MyFS const &f1, MyFS const& f2) {

	if (*(f1.fat) != *(f2.fat))
		return false;

	if (*(f1.root) != *(f2.root))
		return false;

	/*if((*(f1.dmap)!=*(f2.dmap)))
	 return false;*/

	return true;

}

void MyFS::writeBlockDevice() {
	printf("start writeBlockDevice() \n");
	fat->writeBlockDevice(blocks, FAT_START);
	root->writeBlockDevice(blocks, ROOT_START);
	dmap->init(DMAP_START, blocks);
}

void MyFS::resize(char * text, int oldSize, int newSize) {

	int i = newSize - oldSize;
	text += oldSize - 1;
	while (i != 0) {
		*(text++) = char(0);
		i--;
	}
	//*(text-1)='\0';
	text -= newSize;

}
//int fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
int MyFS::readFile(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fileInfo) {

	// TODO: Implement this!
	//printf("readFile start \n"); //funktioniert nicht
	LOG("********************************************************************************************** ");LOG("readFile start ");LOGF("offset: %i, size: %i", offset, size);
	/*if (offset > size) // not possible
	 RETURN(-1);*/

	/*if(fuseOpen(path,fileInfo)==-1)
	 {
	 printf("error in fuseOpen \n");
	 RETURN(-EPERM);
	 }*/

	char * buffer1 = new char[BD_BLOCK_SIZE];

	MyFile * ft = new MyFile();
	if (root->getFile(path, ft) == -1) {

		//printf("can't get file from root root.getFile(path, &fcopy) \n");
		LOG("can't get file from root root.getFile(path, &fcopy) \n");
		RETURN(-ENOENT);
	}
	int fileSize = ft->getSize();
	//int blocksNumber = ceil(fcopy.getSize() / BD_BLOCK_SIZE);
	if (fileSize % BD_BLOCK_SIZE != 0) {
		printf("File's size is false  fcopy.getSize() mod D_BLOCK_SIZE!=0 \n");
		RETURN(-ENOENT);
	}
	int blocksNumber = fileSize / BD_BLOCK_SIZE;
	//buf = new char [fcopy->getSize()];
	int sizeWrite = 0;
	int currentBlock = ft->getFirstBlock();
	int count = 0;
	int temp = 0;
	while (currentBlock != -1 && blocksNumber != 0) {
		if (blocks->read(currentBlock, buffer1) == 0) {
			LOGF("buffer in currentBlock %i is : %s \n",currentBlock, buffer1);

			temp = 0;
			while (*(buffer1) != '\0') {
				*(buf++) = *(buffer1++);
				count++;
				sizeWrite++;
				temp++;
			}
			*buf = '\0';
			buffer1 -= temp;

		} else {
			printf("error in fuseREAD blocks.read(currentBlock, buffer) \n");
			RETURN(-EPERM);
		}

		if (fat->getNext(currentBlock, &currentBlock) == -1) {
			printf(
					"error in fuseREAD fat.getNext(currentBlock,&currentBlock) \n");
			RETURN(-ENOENT);
		}

		blocksNumber--;
	}

	LOGF("sizewrite: %i \n", sizeWrite);LOGF("fcopy->getSize() %i \n",fileSize);
	if (sizeWrite == fileSize) {
		*(buf++) = char(0);
		count++;
	} else {
		LOG("sizeWrite==fileSize false \n");
		for (int i = sizeWrite; i < fileSize; i++) {
			*buf = char(0);
			buf++;
			count++;
		}
	}

	buf -= count;
	//buf+=offset;
	LOGF("all buf is  : %s \n",buf);

	delete[] buffer1;
	delete ft;

	LOG("readFile success \n");
	RETURN(0);
}
// int fuseCreate(const char *, mode_t, struct fuse_file_info *);
int MyFS::addFile(const char * name, mode_t mode, time_t mtime, off_t size,
		char * text) {
	LOG("********************************************************************************************** ");LOG("addFile start");
	if (size % BD_BLOCK_SIZE != 0) {
		LOGF("Die Datei %s wurde falsh hinzugefuegt \n", name);
		RETURN(-1);

	}

	LOGF("add file with name : %s \n", name);

	int blocksNumber = size / BD_BLOCK_SIZE;
	LOGF("blocksNumber : %i \n", blocksNumber);

	int * blocksUse = new int[blocksNumber + 1];

	blocksUse[blocksNumber] = 0;
	if (dmap->getFreeBlocks(blocksNumber, &blocksUse) == 0) {
		if (root->addFile(name, size, mode, mtime, blocksUse[0]) == -1) {
			printf(
					"error in addFile in root->addFile(name, size, mode,st_mtime,blocks[0] \n");
			return -1;
		}

		for (int i = 0; i < blocksNumber; i++) {
			dmap->setUsed(blocksUse[i]);

			if (i + 1 != blocksNumber) {
				if (fat->link(blocksUse[i], &blocksUse[i + 1]) == -1) {
					RETURN(-1);
					printf(
							"error in addFile in fat.link(blocks[i], &blocks[i+1] \n");
				}
			}

			if (this->blocks->write(blocksUse[i], text) == -1) {
				printf("error in addFile in this->blocks.write(i, \"try\") \n");
			}
			text += BD_BLOCK_SIZE;
		}

	} else {
		printf("error in addFile no free blocks in dmap \n");
		RETURN(-EPERM);
		//no more place
	}
	delete[] blocksUse;
	LOG("addFile succes");
	//printf("addFile succes \n");
	RETURN(0);
}

//int fuseUnlink(const char *path);
int MyFS::deleteFile(const char *name) {

	// TODO: Implement this!
	// Tanja's comments:
	//How to unlink file?
	//1 delete file from root
	//2 set bloks unused (change FAT, this changes are executed in Dmap.setUnused funktion)

	//TODO MODE prueffen
	MyFile fcopy;
	if (root->getFile(name, &fcopy) == -1 || root->deleteFile(name) == -1) {
		printf(
				"error in deleteFeil in root.getFile(name, &fcopy)==-1||root.deleteFile(name)==-1");
		RETURN(-ENOENT);
	}

	int blocksNumber = fcopy.getSize() / BD_BLOCK_SIZE;
	int currentBlock = fcopy.getFirstBlock();
	//////////////////////////////////////
	char * text = new char[fcopy.getSize()];
	for (int i = 0; i < fcopy.getSize(); i++)
		*(text++) = char(0);
	text -= fcopy.getSize();
	//////////////////////////////////////
	while (blocksNumber != 0 && currentBlock != -1) {
		if (this->blocks->write(currentBlock, text) == -1) {
			printf("error in addFile in this->blocks.write(i,    ) \n");
		}

		if (dmap->setUnused(currentBlock) == -1) {
			printf("error in deleteFile in dmap.setUnused(currentBlock)");
			RETURN(-EPERM);
		}

		int blockBefore = currentBlock;
		if (fat->getNext(currentBlock, &currentBlock) == -1) {
			printf(
					"error in deleteFeil in fat.getNext(currentBlock, &currentBlock");
			RETURN(-EPERM);
		}

		if (fat->unLink(blockBefore) == -1) {
			printf("error in deleteFeil in fat.unLink(blockBefore)");
			RETURN(-EPERM);
		}

		blocksNumber--;
	}
	delete[] text;
	RETURN(0);
}

int MyFS::fuseGetattr(const char *path, struct stat *st) {
	LOGM();
	//TODO
	//printf("[getattr] Called\n");
	//printf("\tAttributes of %s requested\n", path);

	//Ist es hier genau so wie in Aufgabestellung?
	//Dateinamen werden in path mit „/“ am Anfang übergeben – ggf. beachten
	//beim Durchsuchen des Verzeichnisses!

	LOGF("Requested path = %s ",path);

	MyFile * f = new MyFile();
	LOG("1");

	/*const char *temp = new char[1];
	 temp=path;
	 if (strcmp(temp, "/")) {
	 //Path ist ungleich "/"
	 LOG("can't get file from root. File's should start with /");
	 RETURN(-ENOENT);
	 }*/
	LOG("2");

	if (strcmp(path, "/") != 0)
		if (root->getFile(path + 1, f) == -1) {

			LOGF("Cant find a file with path: %s",path);
			RETURN(-ENOENT);
		}

	LOG("Now starting to set attributes");

	/*struct stat {
	 dev_t     st_dev;
	 ino_t     st_ino;     /* inode number
	 mode_t    st_mode;    /* protection
	 nlink_t   st_nlink;   /* number of hard links
	 uid_t     st_uid;     /* user ID of owner /
	 gid_t     st_gid;     /* group ID of owner
	 dev_t     st_rdev;    / device ID (if special file)
	 off_t     st_size;    / total size, inytes
	 blksize_t st_blksize; / blocksize for fi system I/O
	 blkcnt_t  st_blocks;  / number of 512B blos allocated
	 time_t    st_atime;   / time of last access
	 time_t    st_mtime;   / time of last modification
	 time_t    st_ctime;   / time of last status change
	 };*/

	st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	st->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
	//st->st_mtime = fcopy.getLastMod(); ??? Should be here the current time?
	st->st_mtime = time(NULL); // The last "m"odification of the file/directory is right now
	st->st_ctime = time(NULL);

	if (strcmp(path, "/") == 0) {
		LOG("Path == /");

		st->st_mode = S_IFDIR | 0555;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
		//size = sum size all files in dir
	} else {
		LOG("Anderere Path als \"/\" wurde abgefragt");
		st->st_mode = S_IFREG | 0444;
		st->st_nlink = 1;
		LOGF("size from %s is %i", path,f->getSize() );
		st->st_size = f->getSize();

	}

	delete f;
	LOG("fuseGetattr erfolgreich beendet");
	RETURN(0);

}

int MyFS::fuseMknod(const char *path, mode_t mode, dev_t dev) { //??? wir brauchen das nicht
	LOGM();

	LOGF("fuseMKnod start , path : %s \n ", path);

	// TODO: Implement this!

	// How to create Inode?
	// add new file to the Root
	// Information about the file is saved in the objekt File 
	// dev_t dev muss ID von unsere file system sein

	int * blocks = new int[1];

	LOG("0\n");LOGF("dmap->getFreeBlocks(1, blocks) : %i \n", dmap->getFreeBlocks(1, &blocks));
	if (dmap->getFreeBlocks(1, &blocks) == -1) {
		LOG("can't add file in root dmap is full. Error in dmap.getFreeBlocks(1,blocks)");
		RETURN(-1);
	}

	LOG("1\n");

	if (root->addFile(path + 1, 512, mode, time(NULL), blocks[0])) {
		LOG("can't add file in root. Error in root.addFile(path, 512, S_IFREG | 0444)");
		RETURN(-EPERM);

		LOG("2\n");
	}
//dmap? fat?
	writeBlockDevice();
	RETURN(0);
}

int MyFS::fuseUnlink(const char *path) {
	LOGM();
	int ret = deleteFile(path + 1);
	writeBlockDevice();
	RETURN(ret);

}

int MyFS::fuseOpen(const char *path, struct fuse_file_info *fileInfo) { // How to open file from hier?
	LOGM();

	//TODO funktioniert falsh
	LOGF("Requested path in Fuse Open = %s ",path);

	if (sp->getOpen() > NUM_OPEN_FILES) {
		LOG("too many files are opened");
		RETURN(-EPERM);
	}
//sp
	sp->addOpen();
	LOG("1");LOGF("root->existName(%s) == %i ", path+1, root->existName(path+1));
	if (root->existName(path + 1)) {
		/*	MyFile fcopy;
		 if (root->getFile(path + 1, &fcopy) == 0)
		 {
		 fileInfo->attribute angeben...
		 }*/
		LOGF("name %s exist \n", path);
		fileInfo->fh = 1;
		RETURN(0);
	}LOG("2");
	//TOdo etwas tun, wenn path existiert
	//vermerken, dass datei geöffnet
	//Julia: als was merken? als int, ähnlich zu filedescriptor in liunx?
	//falls zu viele datein schon geöffnet-> fehler

//fuseInit()
	/*if(blocks->open(path)==-1) //wenn ich das mache, kann ich danach nich read machen
	 {
	 fileInfo->fh=1;
	 printf("error in fuseOpen in blocks.open(path)");
	 RETURN(-EPERM);

	 }*/

	RETURN(-1);
}

int MyFS::fuseRead(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fileInfo) {
	LOGF("fuseRead start path: %s \n",path);

	LOGF("root->existName(%s) == %i \n", path+1, root->existName(path+1));

	if (root->existName(path + 1) == 0) {
		RETURN(-1);LOG("root->existName(path+1)==0 \n");
	}LOG("1");

	MyFile * fcopy = new MyFile();
	if (root->getFile(path + 1, fcopy) == -1) {
		LOG("can't get file from root root.getFile(path, &fcopy) \n");
		RETURN(-ENOENT);
	}

	/*	if(offset>size)
	 RETURN(-1);*/

	//buf = new char[size];
	LOG("2");

	// TODO: Implement this!

	int i = readFile(path + 1, buf, size, offset, fileInfo);
	//LOGF("%s \n",buf);
	//printf("%s \n",buf);
	//RETURN(size);
	LOGF("buf in fuseRead: %s", buf);

	int count = 0;
	while (*(buf++) != '\0')
		count++;
	buf -= count;

	//size = fcopy->getSize();
	delete fcopy;
	return count;

}
/*int MyFS::fuseWrite
 - mit fuseWrite kann man den Inhalt einer Datei veraendern
 - mit size kann ich festlegen, wie viel ich veraendern moechte, mit offset, wo in der Datei ich
 etwas aendern moechte und buf ist mein Inhalt, den ich in die schon vorhandene Datei einfuegen
 moechte*/
int MyFS::fuseWrite(const char *path, const char *buf, size_t size,
		off_t offset, struct fuse_file_info *fileInfo) {
	LOGM();
	//TODO MODE prueffen
	// TODO: Implement this!

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LOGF("fusewrite %s \n", path);LOGF("fusewrite buf %s \n", buf);LOGF("in fuseWrite size : %i offset: %i \n", size, offset);

	string* s = root->getArray();

	for (int i = 0; i < root->getSize(); i++)
		LOGF("s:  %s \n", s[i].c_str());

	MyFile *flink = new MyFile();
	LOGF("try to get %s \n", path+1);

	string temp(path + 1);
	if (root->getFile(temp, flink) == -1) {
		printf("error in fuseWrite in root.getFile( path, &fcopy)");
		RETURN(-EPERM);
	}LOG("2");
	int blocksNumber;
	size_t newSizeFile;
	char * newBuf;
	int newBufCount=0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (offset != 0) {
		newSizeFile =
				ceil(
						(double) size + flink->getSize() / BD_BLOCK_SIZE) * BD_BLOCK_SIZE;

		newBuf = new char[newSizeFile];
		readFile(path + 1, newBuf, size, offset, fileInfo);

		int newCount = 0;
		for (int i = 0; *newBuf!=char(0); i++) {
			newBuf++;
			newCount++;
		}

		*(newBuf++)='\0';
		newCount++;


		for (int i = 0; i < (int)size; i++) {
			*(newBuf++) = *(buf++);
			newCount++;
		}

		newBuf -= newCount;
	} else {
		newSizeFile =
				ceil(
						(double) size / BD_BLOCK_SIZE) * BD_BLOCK_SIZE;

		newBuf = new char[newSizeFile];
		int newCount = 0;
		for (int i = 0; i < (int)size; i++) {
			*(newBuf++) = *(buf++);
			newCount++;
		}

		newBuf -= newCount;
	}

	LOGF("newBuf: %s \n", newBuf);
///////////////////////////////////////alte parameter loeschen////////////////////////////////////////////////////////
	//if (offset == 0) {
	char * text = new char[ BD_BLOCK_SIZE];
	for (int i = 0; i < BD_BLOCK_SIZE; i++)
		*(text++) = char(0);
	text -= BD_BLOCK_SIZE;
///////////////////////////////////////////////////
	blocksNumber = (flink->getSize() / BD_BLOCK_SIZE);
	int currentBlock = flink->getFirstBlock();
	LOGF("erste Block am Anfang : %i \n",currentBlock);LOGF("blocksNumber alt: %i \n", blocksNumber);

	while (blocksNumber != 0 && currentBlock != -1) {
		if (this->blocks->write(currentBlock, text) == -1) {
			printf("error in addFile in this->blocks.write(i,    ) \n");
		}

		if (dmap->setUnused(currentBlock) == -1) {
			printf("error in deleteFile in dmap.setUnused(currentBlock)");
			RETURN(-EPERM);
		}

		int blockBefore = currentBlock;
		if (fat->getNext(currentBlock, &currentBlock) == -1) {
			printf(
					"error in deleteFeil in fat.getNext(currentBlock, &currentBlock");
			RETURN(-EPERM);
		}

		if (fat->unLink(blockBefore) == -1) {
			printf("error in deleteFeil in fat.unLink(blockBefore)");
			RETURN(-EPERM);
		}

		blocksNumber--;
	}

	delete[] text;
	newSizeFile = ceil((double) size / BD_BLOCK_SIZE) * BD_BLOCK_SIZE;
//	}
//////////////////////////////neu parameter angeben////////////////////////////////////////////
	/*	else {
	 newSizeFile =ceil((double) size + flink->getSize() / BD_BLOCK_SIZE) * BD_BLOCK_SIZE;

	 char * newBuf = new char[newSizeFile];
	 readFile(path+1, newBuf, size, offset,fileInfo);

	 int newCount=0;
	 for(int i=0; i<flink->getSize();i++)
	 {
	 newBuf++;
	 newCount++;
	 }

	 for(int i=0;i<size;i++)
	 {
	 *(newBuf++)=*(buf++);
	 newCount++;
	 }

	 newBuf-=newCount;


	 }*/

	LOGF("newSize: %i \n", newSizeFile);
	flink->setSize(newSizeFile);
	LOGF("flink->getSize() : %i \n", flink->getSize());
	flink->setLastMod(time(NULL));
	flink->setLastAccess(time(NULL));
	LOG("5");

	blocksNumber = (newSizeFile / BD_BLOCK_SIZE);
	LOGF("blocksNumber new: %i \n", blocksNumber);
	int * blocksUse = new int[blocksNumber];

	//blocksUse[blocksNumber] = 0;
	if (dmap->getFreeBlocks(blocksNumber, &blocksUse) == 0) {
		/*if (root->addFile(name, size, mode, mtime, blocksUse[0]) == -1) {
		 printf(
		 "error in addFile in root->addFile(name, size, mode,st_mtime,blocks[0] \n");
		 return -1;
		 }*/
		flink->setFirstBlock(blocksUse[0]);
		LOGF("flink->getFirstBlock() : %i \n", flink->getFirstBlock());

		//currentBlock = flink->getFirstBlock();
		int tempCount = 0;
		for (int i = 0; i < blocksNumber; i++) {
			LOGF("bearbeite Block nr %i \n", blocksUse[i]);
			dmap->setUsed(blocksUse[i]);

			if (i + 1 != blocksNumber) {
				LOGF("%i != blocksNumber \n", i+1);
				if (fat->link(blocksUse[i], &blocksUse[i + 1]) == -1) {
					RETURN(-1);
					printf(
							"error in addFile in fat.link(blocks[i], &blocks[i+1] \n");
				}
			}
			char * buffer = new char[BD_BLOCK_SIZE];
			int count = 0;
			while (*newBuf != '\0') {
				//LOG("start while\n");
				*buffer = *newBuf;
				newBuf++;
				buffer++;
				count++;
				tempCount++;
				newBufCount++;
				if (count == BD_BLOCK_SIZE - 2)
					break;
				if (tempCount == (int) size)
					break;
			}
			*(buffer++) = char(0);
			count++;
			buffer -= count;

			LOGF("write buffer %s in  block %i \n", buffer, blocksUse[i]);
			if (this->blocks->write(blocksUse[i], buffer) == -1) {
				printf("error in addFile in this->blocks.write(i, \"try\") \n");
			}

			//buf += BD_BLOCK_SIZE;
			delete[] buffer;
		}

	} else {
		printf("error in addFile no free blocks in dmap \n");
		RETURN(-EPERM);
		//no more place
	}
	delete[] blocksUse;
	newBuf-=newBufCount;
	delete [] newBuf;
	//LOGF("currentBlock: %i \n", currentBlock);
	LOG("6");
////////////////beschreiben/////////////////////////////////////////////////////////////

	RETURN(0);
}

int MyFS::fuseRelease(const char *path, struct fuse_file_info *fileInfo) {
	LOGM();

	// TODO: Implement this!
	//temporeres Zeug loeschen
	fileInfo->fh = NULL;
	sp->closeOpen();

	//sonst noch was?

	RETURN(0);
}

int MyFS::fuseOpendir(const char *path, struct fuse_file_info *fileInfo) { // Is it not the same as fuseReaddir?
	LOGM();

	LOGF("Requested path in Fuse Open dir = %s ",path);

	int templog = (strcmp(path, "/"));

	LOGF("strcmp(path, \"/\") returns: %d", templog);

	// TODO: Implement this!
	if (strcmp(path, "/") == 0) // Root oeffnen
			{
		printf("Opening root directory");
		fileInfo->fh = 1; //Julia: gibt es etwas sinnvolleres als sich "1" zu merken?
		//return 0;
	}

	/*  if (strcmp(path, "/") != 0){ // es existieren keine anderen Directories, daher fehler
	 printf("This directory doesnt exist, try opening the root directory");
	 return -ENOENT ;//Datei oder Verzeichnis existiert nicht
	 }*/

	RETURN(0);
}

int MyFS::fuseReaddir(const char *path, void *buffer, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fileInfo) {
	LOG("start fuseReaddir \n");

	// TODO: Implement this!

	/* if(fileInfo->fh == NULL){ //fh durch fuseopendir gesetzt-> bestaetigt existenz
	 return -ENOENT ;//Datei oder Verzeichnis existiert nicht
	 }*/

	LOGF("--> Getting The List of Files of %s\n", path);

	// filler(void *buf, const char *name,const struct stat *stbuf, off_t off);
	// struct stat <- can contain the file type

	filler(buffer, ".", NULL, 0); // Current Directory
	filler(buffer, "..", NULL, 0); // Parent Directory

	LOG(" ready with filler"); // Parent Directory\n");

	string * listNames;
	offset = 0;

	listNames = root->getArray();

	for (int unsigned i = 0; i < (root->getSize()); i++) {
		//convert from string to char. Do we need string?
		char *name = new char[listNames[i].length() + 1];
		strcpy(name, listNames[i].c_str());
		LOGF("name %i : %s \n", i, name);
		//////////////////////////////////////////////////////////////////////
		filler(buffer, name, NULL, 0);
		LOG("filler success \n");
		delete[] name;

	}LOG("readDir success\n");
	RETURN(0);
	// <<< My new code
}

int MyFS::fuseReleasedir(const char *path, struct fuse_file_info *fileInfo) {
	LOGM();

	// TODO: Implement this!
	//temporeres Zeug loeschen

	fileInfo->fh = NULL; // Julia: Sonst noch was loeschen?

	RETURN(0);
}

int MyFS::fuseCreate(const char *path, mode_t mode,
		struct fuse_file_info *fileInfo) { // Was wird es hier gemacht? Sien hier fat dmap superblock und root erstellt?
	LOGM();

	// TODO: Implement this!
	//add empty File
	char * c;
	RETURN(addFile(path,mode,time(NULL),0, c));

}

void* MyFS::fuseInit(struct fuse_conn_info *conn) {
	// Open logfile
	this->logFile = fopen(
			((MyFsInfo *) fuse_get_context()->private_data)->logFile, "w+");
	if (this->logFile == NULL) {
		fprintf(stderr, "ERROR: Cannot open logfile %s\n",
				((MyFsInfo *) fuse_get_context()->private_data)->logFile);
	} else {
		//    this->logFile= ((MyFsInfo *) fuse_get_context()->private_data)->logFile;

		// turn of logfile buffering
		setvbuf(this->logFile, NULL, _IOLBF, 0);

		LOG("Starting logging...\n");LOGM();

		// you can get the containfer file name here:
		LOGF("Container file name: %s", ((MyFsInfo *) fuse_get_context()->private_data)->contFile);

		// TODO: Implement your initialization methods here!
		// Konstruktor hier (vermuetlich) schreiben . Es wird schon in mkfs.myfs gemacht
		//Wieso muessen wir Konstruktor schreiben? Er wird automatisch aufgerufen
		// Falls wir das von Terminal aufrufen dann? was passiert dann? Muessen wir so was schreiben MyFs mf = new MyFs()?;
		//MyFS();
		char * nameCont =
				((MyFsInfo *) fuse_get_context()->private_data)->contFile;

		blocks->open(nameCont);
		fat->read(FAT_START, blocks);
		root->read(ROOT_START, blocks);
		dmap->read(DMAP_START, blocks);
		/*LOG("try blocks->open(nameCont) \n");
		 BlockDevice * blocks = new BlockDevice();
		 blocks->open(nameCont);*/

		LOG("sucess blocks->open(nameCont) \n");
	}

	RETURN(0);
}

//////////////////////////Todos end////////////////////////////////////////////////////////////////////

int MyFS::fuseMkdir(const char *path, mode_t mode) {
	LOGM();
	return 0;
}

int MyFS::fuseRmdir(const char *path) {
	LOGM();
	return 0;
}

int MyFS::fuseSymlink(const char *path, const char *link) {
	LOGM();
	return 0;
}

int MyFS::fuseRename(const char *path, const char *newpath) {
	LOGM();
	return 0;
}

int MyFS::fuseLink(const char *path, const char *newpath) {
	LOGM();
	return 0;
}

int MyFS::fuseChmod(const char *path, mode_t mode) {
	LOGM();
	return 0;
}

int MyFS::fuseChown(const char *path, uid_t uid, gid_t gid) {
	LOGM();
	return 0;
}
int MyFS::fuseReadlink(const char *path, char *link, size_t size) {
	LOGM();
	return 0;
}
int MyFS::fuseTruncate(const char *path, off_t newSize) {
	LOGM();
	return 0;
}

int MyFS::fuseUtime(const char *path, struct utimbuf *ubuf) {
	LOGM();
	return 0;
}

int MyFS::fuseStatfs(const char *path, struct statvfs *statInfo) {
	LOGM();
	return 0;
}

int MyFS::fuseFlush(const char *path, struct fuse_file_info *fileInfo) {
	LOGM();
	return 0;
}

int MyFS::fuseFsync(const char *path, int datasync, struct fuse_file_info *fi) {
	LOGM();
	return 0;
}

int MyFS::fuseListxattr(const char *path, char *list, size_t size) {
	LOGM();
	RETURN(0);
}

int MyFS::fuseRemovexattr(const char *path, const char *name) {
	LOGM();
	RETURN(0);
}

int MyFS::fuseFsyncdir(const char *path, int datasync,
		struct fuse_file_info *fileInfo) {
	LOGM();
	RETURN(0);
}

int MyFS::fuseTruncate(const char *path, off_t offset,
		struct fuse_file_info *fileInfo) {
	LOGM();
	RETURN(0);
}

void MyFS::fuseDestroy() {
	LOGM();
}

#ifdef __APPLE__
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x) {
#else
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value,
		size_t size, int flags) {
#endif
	LOGM();
	RETURN(0);
}

#ifdef __APPLE__
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x) {
#else
int MyFS::fuseGetxattr(const char *path, const char *name, char *value,
		size_t size) {
#endif
	LOGM();
	RETURN(0);
}

