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

#include <string.h>
#include <stdlib.h>

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

	printf("WORK WITH CONTAINER INIT : %s \n",nameCont );
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
	LOG("start writeBlockDevice() \n");
	fat->writeBlockDevice(blocks, FAT_START);
	LOG("fat writeBlockDevice sucess \n");
	root->writeBlockDevice(blocks, ROOT_START);
	LOG("root writeBlockDevice sucess \n");
	dmap->init(DMAP_START, blocks);
	LOG("dmap writeBlockDevice sucess \n");
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
int MyFS::readFile(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {

	// TODO: Implement this!
	//printf("readFile start \n"); //funktioniert nicht
	LOG("********************************************************************************************** ");
	LOGF("readFile start , size: %i, offset: %i \n", (int)size, (int)offset);

	//todo hier vielleicht noch irgendwas in fileInfo prüfen?

	//Informationen zu file abrufen
	MyFile * file = new MyFile();
	int returnValue;
	returnValue=root->getFile(path, file);
	if ( returnValue == -1) {
		//printf("can't get file from root root.getFile(path, &fcopy) \n");
		LOG("can't get file from root root.getFile(path, &fcopy) \n");
		RETURN(-ENOENT);
	}


	int fileSize=file->getSize(); ;//= ft->getSize();
	LOGF("readFile fileSize= ft->getSize() %i  \n", fileSize);
	//int blocksNumber = ceil(fcopy.getSize() / BD_BLOCK_SIZE);
	/*if (fileSize % BD_BLOCK_SIZE != 0) {
		printf("File's size is false  fcopy.getSize() mod D_BLOCK_SIZE!=0 \n");
		RETURN(-ENOENT);
	}*/
	//TODO : blocksNumber falsch
	int blocksNumber = fileSize / BD_BLOCK_SIZE;
	LOGF("blocksnumber in readFile : %i \n", blocksNumber);
	//buf = new char [fcopy->getSize()];


	//grenzüberschreitung überprüfen
	if((int)offset+(int)size>fileSize){
		LOG("Requested Postion not within filesize");
		RETURN(-EINVAL);	//TODO ist das der korrekte fehlercode: (Invalid argument)?
	}

	//Auszulesende stelle berechen
	int blockNumber = offset/BD_BLOCK_SIZE;
	int positionInBlock = offset%BD_BLOCK_SIZE;


	//blocknummer des blockNummer-ten Block suchen
	int currentBlock = file->getFirstBlock();
	for(int i = 0; i<blockNumber; i++){ // blockNumber-mal häufig den nächsten Block suchen TODO: ist das so richtig? +-1 block?
		int next = 0;
		fat->getNext(currentBlock,&next);
		currentBlock = next;
	}

	char * readBuf = new char[BD_BLOCK_SIZE];
	int bytesRead = 0;
	int testcount=0;
	//Auslesen bis size blöcke gelesen
	while(bytesRead<=(int)size){
		testcount++;
		blocks->read(currentBlock,readBuf);
		//if(testcount==1)
		//{
			string temp(readBuf);
			printf("size from readBuf: %i readBuf: %s \n",temp.length() , readBuf);

		//}
		//Ausgelesene Bytes in buffer schreiben, bis readbuffer-ende oder genug gelesen
		while(positionInBlock<BD_BLOCK_SIZE && bytesRead<=(int)size){
			buf[bytesRead++]=readBuf[positionInBlock++];
		//	if(testcount==1)
				printf("bytesRead: %i, positionInBlock: %i , buf[bytesRead]: %c,readBuf[positionInBlock]: %c\n ",bytesRead-1,positionInBlock-1, buf[bytesRead-1], readBuf[positionInBlock-1]);

		}
		positionInBlock=0;

		//Nächste Blocknummer holen
		int next = 0;
		fat->getNext(currentBlock,&next);
		currentBlock = next;
	}

	printf("buf: %s \n", buf);
	delete[] readBuf;
	delete file;

	RETURN(0);
/*
>>>>>>> bcd816e55f499fb5569a0bef1c98158ec4d91eec
	int sizeWrite = 0;

	int currentBlock = ft->getFirstBlock();
	int countBuf = 0;
	int temp = 0;
<<<<<<< HEAD
////////////////////////////////////////////////////////////////////////
	LOG("****************************************************************\n");


	LOG("FAT: \n");
	for (int i = 900; i != 950; i++) {

		int next;
		fat->getNext(i, &next);
		//if((i+1)!=next)
		LOGF("%i -> %i \n", i, next);
	}

	//////////////////////////////////////////////////////////////////////
=======



>>>>>>> bcd816e55f499fb5569a0bef1c98158ec4d91eec

		char * buffer1 = new char[BD_BLOCK_SIZE];
		//buffer1="";
		//printf("buffer1: %s \n", buffer1);
	while (currentBlock != -1 && blocksNumber != 0) {
		if (blocks->read(currentBlock, buffer1) == 0) {

		//	LOGF("buffer in currentBlock %i is : %s \n",currentBlock, buffer1);
			string tmp(buffer1);
		printf("size buffer: %i, buffer in currentBlock %i is : %s \n",(int)tmp.length(),currentBlock, buffer1);

			LOGF("buffer in currentBlock %i is : %s \n", currentBlock, buffer1);


			temp = 0;
			while (*(buffer1) != '\0') {
				*(buf++) = *(buffer1++);
				countBuf++;
				sizeWrite++;
				temp++;
				/*if(temp==BD_BLOCK_SIZE-1)
				 break;
			}
			*buf = '\0';
			buffer1 -= temp;

		} else {
			LOG("error in fuseREAD blocks.read(currentBlock, buffer) \n");
			RETURN(-EPERM);
		}

		int neu = 0;
		if (fat->getNext(currentBlock, &neu) == -1) {
			LOG("error in fuseREAD fat.getNext(currentBlock,&currentBlock) \n");
			RETURN(-ENOENT);
		}
		//if(currentBlock+1!=neu)
		LOGF(" verweis currentBlock : %i , neuBlock: %i", currentBlock, neu);
		currentBlock = neu;

		blocksNumber--;
	}

	LOGF("sizewrite: %i \n", sizeWrite);
	LOGF("fcopy->getSize() %i \n", fileSize);
	if (sizeWrite == fileSize) {
		*(buf) = char(0);
		buf++;
		countBuf++;
	} else {
		LOG("sizeWrite==fileSize false \n");
		for (int i = sizeWrite; i < fileSize; i++) {
			*buf = char(0);
			buf++;
			countBuf++;
		}
		LOG("end schleife \n");
	}

	buf -= countBuf;
	//buf+=offset;
	//LOGF("all buf is  : %s \n",buf);

	delete[] buffer1;
	LOG("delete[] buffer1 sucess \n");
	delete ft;
	LOG("delete[] ft sucess \n");

	LOG("readFile success \n");
	*/

}
// int fuseCreate(const char *, mode_t, struct fuse_file_info *);
int MyFS::addFile(const char * name, mode_t mode, time_t mtime, off_t size, char * text) {

	LOG("********************************************************************************************** ");
	LOG("addFile start");
	LOGF("add file with name : %s \n", name);

	int numberNeededBlocks = size / BD_BLOCK_SIZE;
	int bytesInLastBlock = size%BD_BLOCK_SIZE;
	//file passt nicht vollständig in blöcke
	if(bytesInLastBlock!=0){
		numberNeededBlocks++;
	}

	LOGF("numberNeededBlocks : %i \n", numberNeededBlocks);
	LOGF("BytesInLastBlock: %i \n",bytesInLastBlock);

	//array zum merken der blocknummern
	int * blocksUse = new int[numberNeededBlocks+1];
	blocksUse[numberNeededBlocks] = -1; //keine Verlinkung vom letzten Block aus

	int returnCode;

	//Freie blöcke finden
	returnCode= dmap->getFreeBlocks(numberNeededBlocks, &blocksUse);
	if(returnCode ==-1){
		printf("error in addFile no free blocks in dmap \n");
		RETURN(-EPERM);
	}

	//file ins root schreiben
	returnCode = root->addFile(name, size, mode, mtime, blocksUse[0]);
	if(returnCode == -1){
		printf("error in addFile in root->addFile(name, size, mode,st_mtime,blocks[0] \n");
		return -1;
	}


	int i = 0;
	while(numberNeededBlocks!=0){
		//1. Als belegt markieren
		dmap->setUsed(blocksUse[i]);

		//2. Verlinken in Fat
		returnCode = fat->link(blocksUse[i], &blocksUse[i + 1]);
		if(returnCode ==-1){
			printf("Error: Problem with linking blocks in fat");
			RETURN(-1);
		}
		// 3. Inhalt schreiben
		//Ausnahme: letzter Block ist nicht ganz voll
		if(numberNeededBlocks==1 && bytesInLastBlock!=0){
			char * buffer = new char[BD_BLOCK_SIZE];

			for(int index = 0; index<bytesInLastBlock;index++){ // Gültigen Inhalt schreiben
				buffer[index]= *text++;
			}
			for(int index = bytesInLastBlock;index<BD_BLOCK_SIZE; index++){ // Rest mit 0 füllen
				buffer[index]= char(0);
			}
			//printf("Puffer %s \n",buffer);
			returnCode = this->blocks->write(blocksUse[i], buffer);
			delete[] buffer;
		}else{
			returnCode = this->blocks->write(blocksUse[i], text);
			text += BD_BLOCK_SIZE; // pointer weiter schieben
		}
		if(returnCode ==-1){
					printf("Error: Problem with writing to blockdevice");
					RETURN(-1);
				}
		numberNeededBlocks--;
	}


/*
		for (int i = 0; i < numberNeededBlocks; i++) {
			dmap->setUsed(blocksUse[i]);

			if (i + 1 != numberNeededBlocks) {
				if (fat->link(blocksUse[i], &blocksUse[i + 1]) == -1) {
					RETURN(-1);
					printf("error in addFile in fat.link(blocks[i], &blocks[i+1] \n");
				}
			}

			string tmp(text);
			printf("text size:%i \n", tmp.length());

			if(tmp.length()<BLOCK_SIZE)
			{
				for(int i=tmp.length();i<BLOCK_SIZE;i++)
					text[i]=char(0);
			}


			if (this->blocks->write(blocksUse[i], text) == -1) {
				printf("error in addFile in this->blocks.write(i, \"try\") \n");
			}
			text += BD_BLOCK_SIZE;
		}
*/

	delete[] blocksUse;
	LOG("addFile succes");
	//printf("addFile succes \n");
	RETURN(0);
}

//int fuseUnlink(const char *path);
int MyFS::deleteFile(const char *name) {
	//TODO: veraendern fuer richtige size
	MyFile fcopy;
	if (root->getFile(name, &fcopy) == -1 || root->deleteFile(name) == -1) {
		printf("error in deleteFeil in root.getFile(name, &fcopy)==-1||root.deleteFile(name)==-1");
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
			printf("error in deleteFeil in fat.getNext(currentBlock, &currentBlock");
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
	LOGM()
	;

	//Ist es hier genau so wie in Aufgabestellung?
	//Dateinamen werden in path mit „/“ am Anfang übergeben – ggf. beachten
	//beim Durchsuchen des Verzeichnisses!

	LOGF("Requested path = %s ", path);

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

			LOGF("Cant find a file with path: %s", path);
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
		LOGF("size from %s is %i", path, f->getSize());
		st->st_size = f->getSize();

	}

	delete f;
	LOG("fuseGetattr erfolgreich beendet");
	RETURN(0);

}

int MyFS::fuseMknod(const char *path, mode_t mode, dev_t dev) {
	LOGM();

	LOGF("fuseMKnod start , path : %s \n ", path);

	if (root->getSize() >= NUM_DIR_ENTRIES) {
		LOG("es gibt schon zu viel Dateien \n");
		RETURN(-EPERM);
	}

	if (root->existName(path + 1)) {
		LOG("DATEINAME EXISTIERT SCHON \n");
		RETURN(-EEXIST);
	}
	string t(path + 1);
	if (t.length() > 255) {
		LOG("DATEINAME ZU GROSS \n");
		printf("DATEINAME ZU GROSS \n");
		RETURN(-EPERM);
	}

	int * blocks = new int[1];

	LOGF("dmap->getFreeBlocks(1, blocks) : %i \n", dmap->getFreeBlocks(1, &blocks));
	int returnCode;

	//1. Freien Block suchen
	returnCode = dmap->getFreeBlocks(1, &blocks);
	if (returnCode == -1) {
		LOG("can't add file in root dmap is full. Error in dmap.getFreeBlocks(1,blocks) \n");
		RETURN(-EPERM);
	}

	// 2. File ins root schreiben
	returnCode =root->addFile(path + 1, 0, mode, time(NULL), blocks[0]);
	if (returnCode == -1) {
		LOG("can't add file in root. Error in root.addFile(path, 0, S_IFREG | 0444) \n");
		RETURN(-EPERM);
	}
	LOGF("Added File : %s to root \n ", path+1);

	//3. Block als belegt markieren
	returnCode = dmap->setUsed(blocks[0]);
	if (returnCode == -1) {
		LOG("can't add file in root. Error in dmap->setUsed(blocks[0]) \n");
		RETURN(-EPERM);
	}

	writeBlockDevice();
	RETURN(0);
}

int MyFS::fuseUnlink(const char *path) {
	LOGM()
	;
	int ret = deleteFile(path + 1);
	writeBlockDevice();
	RETURN(ret);

}

int MyFS::fuseOpen(const char *path, struct fuse_file_info *fileInfo) { // How to open file from hier?
	LOGM()
	;

	//TODO funktioniert falsh
	LOGF("Requested path in Fuse Open = %s ", path);

	//wir brauchen open nicht ? weil es nur 64 dateien existieren kann
	/*if (sp->getOpen() > NUM_OPEN_FILES) {
	 LOG("too many files are opened");
	 RETURN(-EPERM);
	 }
	 //sp
	 sp->addOpen();*/
	LOG("1");
	LOGF("root->existName(%s) == %i ", path + 1, root->existName(path + 1));
	if (root->existName(path + 1)) {
		/*	MyFile fcopy;
		 if (root->getFile(path + 1, &fcopy) == 0)
		 {
		 fileInfo->attribute angeben...
		 }*/
		LOGF("name %s exist \n", path);
		fileInfo->fh = 1;
		RETURN(0);
	}
	LOG("2");
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

int MyFS::fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
	LOGF("fuseRead start path: %s size: %i offset: %i \n",path,size,offset);

	//Überprüfen, dass file existiert
	int returnValue = root->existName(path + 1);
	LOGF("root->existName(%s) == %i \n", path + 1, returnValue);
	if (!returnValue) {
		RETURN(-ENOENT);LOG("root->existName(path+1)==0 \n");
	}

	readFile(path + 1, buf, size, offset, fileInfo);

	//printf("%s \n",buf);
	//LOGF("buf in fuseRead: %s", buf);

	return size;
}


/*int MyFS::fuseWrite
 - mit fuseWrite kann man den Inhalt einer Datei veraendern
 - mit size kann ich festlegen, wie viel ich veraendern moechte, mit offset, wo in der Datei ich
 etwas aendern moechte und buf ist mein Inhalt, den ich in die schon vorhandene Datei einfuegen
 moechte*/
int MyFS::fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
	LOGM()
	;
	//TODO MODE prueffen
	// TODO: Implement this!
//long endSizeBuf=size;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LOGF("fusewrite %s \n", path);
	LOGF("fusewrite buf %s \n", buf);
	LOGF("in fuseWrite size : %i offset: %i \n", size, offset);

	string* s = root->getArray();

	for (int i = 0; i < root->getSize(); i++)
		LOGF("s:  %s \n", s[i].c_str());

	MyFile *flink = new MyFile();
	LOGF("try to get %s \n", path + 1);

	string tempStr(path + 1);
	if (root->getFile(tempStr, flink) == -1) {
		LOG("error in fuseWrite in root.getFile( path, &fcopy) \n");
		RETURN(-EPERM);
	}
	LOG("2 \n");

	int blocksNumber;
	size_t newSizeFile;
	char * newBuf;
	int newBufCount = 0;
	int ret = size;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (offset != 0) {
		LOG("offset ist ungleich 0 \n");
		int temp = ceil((double) flink->getSize() / BD_BLOCK_SIZE) * BD_BLOCK_SIZE;
		LOGF("anzahl der Bytes in alte Datei: %i \n", temp);
		if (offset == temp) {
			LOGF("offset ist gleich anzahl an Bloecke %i == %i \n", offset, temp);

			///anzahlBytes in Datei/////////////////////////////////
			char * tempBuf = new char[flink->getSize()];
			readFile(path + 1, tempBuf, flink->getSize(), 0, fileInfo);

			int bytesFile = 0;
			while (*tempBuf != char(0)) {
				bytesFile++;
				tempBuf++;
			}

			tempBuf -= bytesFile;
			LOGF("Bytes in File: %i \n", bytesFile);
			LOGF("Bytes in tempBuf: %s \n", tempBuf);
			delete[] tempBuf;
			LOG("delete tempBuf sucess \n");
			////////////////////////////////////////////////////////
			LOG("count bytes sucess");

			double allSize = ((int) size + bytesFile);
			LOGF("allSize=(size+ (bytesFile - offset)): %f \n", allSize);
			LOGF("ceil(allSize/ BD_BLOCK_SIZE) : %f \n", ceil((double)allSize/ BD_BLOCK_SIZE));
			newSizeFile = ceil((double) allSize / BD_BLOCK_SIZE) * BD_BLOCK_SIZE;
			LOGF("newSizeFile is %i \n", newSizeFile);

			newBuf = new char[newSizeFile];

			LOG("try to readFile \n");
			readFile(path + 1, newBuf, size, 0, fileInfo);
			LOGF("end readFile, newBuf: %s \n", newBuf);

			int newCount = 0;
			for (int i = 0; *newBuf != char(0); i++) {
				newBuf++;
				newCount++;
			}
			//(newBuf++)='\0';
			//size += newCount;
			/**(newBuf++)='\0';
			 newCount++;*/
			//int t=(int) size+newCount;
			while (*buf != char(0)) {
				*(newBuf++) = *(buf++);
				newCount++;
			}

			newBuf -= newCount;
			ret = newCount;
		} else {
			LOGF("offset ist ungleich anzahl an Bloecke %i == %i \n", offset, temp);
			int newAddSize = ((int) size + (int) offset);
			LOGF("so viel Bytes muessen hinzuegefuegt werden: size(%i) + offset(%i) = %i \n", size, offset, newAddSize);
			if (newAddSize > temp) {

				LOGF("es muss mehr Bytes(%) hinzuegefugt werden, als die Groesse der Datei (%i) \n", newAddSize, temp);
				//die Datei wird groesse nach der ueberschreibung

				//groeese die hinzuegefugt wird finden
				int countBufForSize = 0;
				//countBufForSize - groesse von buf
				while (*buf != char(0)) {
					buf++;
					countBufForSize++;
				}
				buf -= countBufForSize;
				/*int countFileSize = 0;
				 //countFileSize - groesse von den text in der datei
				 char * readBufTemp = new char[temp];
				 readFile(path + 1, readBufTemp, size, 0, fileInfo);
				 while (readBufTemp != char(0)) {
				 readBufTemp++;
				 countFileSize++;
				 }*/

				newSizeFile = ceil((double) newAddSize / BD_BLOCK_SIZE) * BD_BLOCK_SIZE;

				int newCount = 0;
				newBuf = new char[newSizeFile];
				readFile(path + 1, newBuf, size, 0, fileInfo);
				//buf bis offset auslesen
				while (newCount < offset) {
					newBuf++;
					newCount++;
				}

				while (*buf != char(0)) {
					*(newBuf++) = *(buf++);
					newCount++;
				}

				newBuf -= newCount;
				ret = newCount;

			} else {
				LOGF("es muss weniger Bytes(%i) hinzuegefugt werden, als die Groesse der Datei (%i) \n", newAddSize,
						temp);
				int newCount = 0;
				//die Groesse bleibt gleich
				newSizeFile = temp;
				newBuf = new char[newSizeFile];
				readFile(path + 1, newBuf, size, 0, fileInfo);
				//buf bis offset auslesen
				while (newCount < offset) {
					newBuf++;
					newCount++;
				}

				while (*buf != char(0)) {
					*(newBuf++) = *(buf++);
					newCount++;
				}

				while (*newBuf != char(0)) {
					newBuf++;
					newCount++;
				}

				newBuf -= newCount;
				ret = newCount;
			}
		}
	} else {
		LOG("offset ist gleich 0 \n");

		///////////////////////////////version : den ganzen Text in alte Datei muss geloescht werden
		newSizeFile = ceil((double) size / BD_BLOCK_SIZE) * BD_BLOCK_SIZE;

		newBuf = new char[newSizeFile];
		int newCount = 0;
		for (int i = 0; i < (int) size; i++) {
			*(newBuf++) = *(buf++);
			newCount++;
		}

		newBuf -= newCount;
		ret = newCount;

	}

	LOGF("newBuf: %s \n", newBuf);
	LOGF("ret: %i \n", ret);

///////////////////////////////////////alte parameter loeschen////////////////////////////////////////////////////////
	//if (offset == 0) {
	char * text = new char[ BD_BLOCK_SIZE];
	for (int i = 0; i < BD_BLOCK_SIZE; i++)
		*(text++) = char(0);
	text -= BD_BLOCK_SIZE;
///////////////////////////////////////////////////
	blocksNumber = (flink->getSize() / BD_BLOCK_SIZE);
	int currentBlock = flink->getFirstBlock();
	LOGF("erste Block am Anfang : %i \n", currentBlock);
	LOGF("blocksNumber alt: %i \n", blocksNumber);

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
			printf("error in deleteFeil in fat.getNext(currentBlock, &currentBlock");
			RETURN(-EPERM);
		}

		if (fat->unLink(blockBefore) == -1) {
			printf("error in deleteFeil in fat.unLink(blockBefore)");
			RETURN(-EPERM);
		}

		blocksNumber--;
	}

	//delete[] text;
	//newSizeFile = ceil((double) size / BD_BLOCK_SIZE) * BD_BLOCK_SIZE;
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

	LOGF("newSizeFile: %i \n", newSizeFile);
	flink->setSize(newSizeFile);
	LOG("flink->setSize(newSizeFile) sucess \n");
	//root->copyFile(flink->getName(), flink);
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
				LOGF("%i != blocksNumber \n", i + 1);
				if (fat->link(blocksUse[i], &blocksUse[i + 1]) == -1) {
					LOG("error in addFile in fat.link(blocks[i], &blocks[i+1] \n");
					RETURN(-1);
				}
				LOGF("Bloecke %i und %i sind gelinkt \n", blocksUse[i], blocksUse[i + 1]);
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
				if (tempCount == (int) ret)
					break;
			}
			LOG("end while\n");
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

	/////////////////////
	MyFile *ftr = new MyFile();
	//LOGF("try to get %s \n", path+1);

	string temp2(path + 1);
	LOGF("temp2 : %s \n", temp2.c_str());
	int err = root->copyFile(temp2, flink);
	LOGF("err = root->copyFile(temp2, flink) : %i \n", err);
	if (root->getFile(temp2, ftr) == -1)
		;
	LOGF("flink->getSize() : %i \n", flink->getSize());
	LOGF("ftr->getSize() : %i \n", ftr->getSize());

	/////////////////////

	LOG("root->copyFile(path, flink); ready \n");
	writeBlockDevice();
	LOG("writeBlockDevice sucess \n");
	delete[] blocksUse;
	LOG("delete[] blocksUse sucess \n");
	newBuf -= newBufCount;
	delete[] newBuf;
	LOG("delete[] newBuf sucess \n");
	delete[] text;
	LOG("delete text sucess \n");

	delete ftr;
	LOG("delete ftr sucess \n");
	delete flink;
	LOG("delete flink sucess \n");

	//LOGF("currentBlock: %i \n", currentBlock);
	LOG("6");

	RETURN(ret); //!!???
}

int MyFS::fuseRelease(const char *path, struct fuse_file_info *fileInfo) {
	LOGM()
	;

	// TODO: Implement this!
	//temporeres Zeug loeschen
	fileInfo->fh = NULL;
	//sp->closeOpen();

	//sonst noch was?

	RETURN(0);
}

int MyFS::fuseOpendir(const char *path, struct fuse_file_info *fileInfo) { // Is it not the same as fuseReaddir?
	LOGM()
	;

	LOGF("Requested path in Fuse Open dir = %s ", path);

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

int MyFS::fuseReaddir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset,
		struct fuse_file_info *fileInfo) {
	LOG("start fuseReaddir \n");

	int count = 0;
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
		count++;
		filler(buffer, name, NULL, 0);
		LOG("filler success \n");
		LOGF("count : %i \n", count);

		delete[] name;

	}
	LOG("readDir success\n");
	RETURN(0);
	// <<< My new code
}

int MyFS::fuseReleasedir(const char *path, struct fuse_file_info *fileInfo) {
	LOGM()
	;

	// TODO: Implement this!
	//temporeres Zeug loeschen

	fileInfo->fh = NULL; // Julia: Sonst noch was loeschen?

	RETURN(0);
}

int MyFS::fuseCreate(const char *path, mode_t mode, struct fuse_file_info *fileInfo) {
	LOGM()
	;

	LOGF("fuseMKnod start , path : %s \n ", path);

	if (root->getSize() >= NUM_DIR_ENTRIES) {
		LOG("es gibt schon zu viel Dateien \n");
		RETURN(-EPERM);
	}

	if (root->existName(path + 1)) {
		LOG("DATEINAME EXISTIERT SCHON \n");
		RETURN(-EEXIST);
	}
	string t(path + 1);
	if (t.length() > 255) {
		LOG("DATEINAME ZU GROSS \n");
		printf("DATEINAME ZU GROSS \n");
		RETURN(-EPERM);
	}

	int * blocks = new int[1];

	LOG("0\n");
	LOGF("dmap->getFreeBlocks(1, blocks) : %i \n", dmap->getFreeBlocks(1, &blocks));
	if (dmap->getFreeBlocks(1, &blocks) == -1) {
		LOG("can't add file in root dmap is full. Error in dmap.getFreeBlocks(1,blocks) \n");
		RETURN(-EPERM);
	}

	LOG("1\n");

	if (root->addFile(path + 1, 0, mode, time(NULL), blocks[0]) == -1) {
		LOG("can't add file in root. Error in root.addFile(path, 0, S_IFREG | 0444) \n");
		RETURN(-EPERM);

		LOG("2\n");
	}

	if (dmap->setUsed(blocks[0]) == -1) {
		LOG("can't add file in root. Error in dmap->setUsed(blocks[0]) \n");
		RETURN(-EPERM);
	}

	writeBlockDevice();
	RETURN(0);

}

void* MyFS::fuseInit(struct fuse_conn_info *conn) {
	// Open logfile
	this->logFile = fopen(((MyFsInfo *) fuse_get_context()->private_data)->logFile, "w+");
	if (this->logFile == NULL) {
		fprintf(stderr, "ERROR: Cannot open logfile %s\n", ((MyFsInfo *) fuse_get_context()->private_data)->logFile);
	} else {
		//    this->logFile= ((MyFsInfo *) fuse_get_context()->private_data)->logFile;

		// turn of logfile buffering
		setvbuf(this->logFile, NULL, _IOLBF, 0);

		LOG("Starting logging...\n");
		LOGM()
		;

		// you can get the containfer file name here:
		LOGF("Container file name: %s", ((MyFsInfo * ) fuse_get_context()->private_data)->contFile);

		// TODO: Implement your initialization methods here!
		// Konstruktor hier (vermuetlich) schreiben . Es wird schon in mkfs.myfs gemacht
		//Wieso muessen wir Konstruktor schreiben? Er wird automatisch aufgerufen
		// Falls wir das von Terminal aufrufen dann? was passiert dann? Muessen wir so was schreiben MyFs mf = new MyFs()?;
		//MyFS();
		char * nameCont = ((MyFsInfo *) fuse_get_context()->private_data)->contFile;

		//if(*nameCont!=char(0))
		//{
		LOG("try to open container \n");
		blocks->open(nameCont);
		LOG(" open container sucess \n");

		root->read(ROOT_START, blocks);
		LOG(" open root sucess \n");
		dmap->read(DMAP_START, blocks);
		LOG(" open dmap sucess \n");
		fat->read(FAT_START, blocks);
		LOG(" open fat sucess \n");
		//}
		//else
		//{
		//	blocks->create("contFuse.bin");
		//}
		/*LOG("try blocks->open(nameCont) \n");
		 BlockDevice * blocks = new BlockDevice();
		 blocks->open(nameCont);*/

		LOG("sucess blocks->open(nameCont) \n");
	}

	RETURN(0);
}

//////////////////////////Todos end////////////////////////////////////////////////////////////////////

int MyFS::fuseMkdir(const char *path, mode_t mode) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseRmdir(const char *path) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseSymlink(const char *path, const char *link) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseRename(const char *path, const char *newpath) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseLink(const char *path, const char *newpath) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseChmod(const char *path, mode_t mode) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseChown(const char *path, uid_t uid, gid_t gid) {
	LOGM()
	;
	return 0;
}
int MyFS::fuseReadlink(const char *path, char *link, size_t size) {
	LOGM()
	;
	return 0;
}
int MyFS::fuseTruncate(const char *path, off_t newSize) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseUtime(const char *path, struct utimbuf *ubuf) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseStatfs(const char *path, struct statvfs *statInfo) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseFlush(const char *path, struct fuse_file_info *fileInfo) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseFsync(const char *path, int datasync, struct fuse_file_info *fi) {
	LOGM()
	;
	return 0;
}

int MyFS::fuseListxattr(const char *path, char *list, size_t size) {
	LOGM()
	;
	RETURN(0);
}

int MyFS::fuseRemovexattr(const char *path, const char *name) {
	LOGM()
	;
	RETURN(0);
}

int MyFS::fuseFsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
	LOGM()
	;
	RETURN(0);
}

int MyFS::fuseTruncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
	LOGM()
	;
	RETURN(0);
}

void MyFS::fuseDestroy() {
	LOGM()
	;
}

#ifdef __APPLE__
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x) {
#else
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
#endif
	LOGM()
	;
	RETURN(0);
}

#ifdef __APPLE__
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x) {
#else
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size) {
#endif
	LOGM()
	;
	RETURN(0);
}

