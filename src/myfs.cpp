//
//  myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 02.08.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

// For documentation of FUSE methods see https://libfuse.github.io/doxygen/structfuse__operations.html

#undef DEBUG

//  Comment this to reduce debug messages
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

}

MyFS::~MyFS() {

	delete sp;
	delete dmap;
	delete fat;
	delete root;
	delete blocks;

}

bool operator ==(MyFS const &f1, MyFS const& f2) {

	if (*(f1.fat) != *(f2.fat))
		return false;

	if (*(f1.root) != *(f2.root))
		return false;

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
	text -= newSize;

}

int MyFS::readFile(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fileInfo) {

	LOG("********************************************************************************************** ");
	LOGF("readFile start , size: %i, offset: %i \n", (int )size, (int )offset);

	//todo hier vielleicht noch irgendwas in fileInfo prüfen?

	//Informationen zu file abrufen
	MyFile * file = new MyFile();
	int returnValue;
	returnValue = root->getFile(path, file);
	if (returnValue == -1) {
		LOG("can't get file from root root.getFile(path, &fcopy) \n");
		RETURN(-ENOENT);
	}


	int fileSize = file->getSize();

	LOGF("readFile fileSize= ft->getSize() %i  \n", fileSize);

	int usedBlocks = fileSize / BD_BLOCK_SIZE;
	if (fileSize % BD_BLOCK_SIZE != 0)
		usedBlocks++;
	LOGF("usedBlocks in readFile : %i \n", usedBlocks);

	//count place to read
	int blockNumber = offset / BD_BLOCK_SIZE;
	LOGF("blockNumber: %i \n", blockNumber);
	int positionInBlock = offset % BD_BLOCK_SIZE;
	LOGF("positionInBlock: %i \n", positionInBlock);



	//blocknummer des blockNummer-ten Block suchen
	int currentBlock = file->getFirstBlock();
	int currentBlockNumber = 0;
	LOGF("currentBlockNumber: %i ->", currentBlockNumber);
	for (int i = 0; i < blockNumber; i++) { // blockNumber-mal häufig den nächsten Block suchen TODO: ist das so richtig? +-1 block?
		int next = 0;
		fat->getNext(currentBlock, &next);
		currentBlock = next;
		currentBlockNumber++;
		//LOGF("currentBlock: %i \n", currentBlock);
	}

	char * readBuf = new char[BD_BLOCK_SIZE];
	int bytesRead = 0;

	//Size kürzen, damit nicht über Blockgrenze gelesen wird
	if ((int) size > fileSize)
		size = fileSize;

	//Auslesen bis size blöcke gelesen
	while (bytesRead < (int) size) {

		//Prüfen, ob aktueller block in filebuffer steht
		if (fileInfo->writepage == currentBlockNumber+1) {

			LOGF("fileInfo->writepage: %i\n",fileInfo->writepage);
			LOGF("currentBlockNumber: %i steht in filebuffer-> lesen aus buffer\n",currentBlockNumber);
			root->writeFromPuffer(path,readBuf);
			LOGF("buffer aus file buffer: %s\n",readBuf);
		} else {

			blocks->read(currentBlock, readBuf);
		}

		//Ausgelesene Bytes in buffer schreiben, bis readbuffer-ende oder genug gelesen
		while (positionInBlock < BD_BLOCK_SIZE && bytesRead < (int) size) {
			buf[bytesRead++] = readBuf[positionInBlock++];
			//	if(testcount==1)
			//	printf("bytesRead: %i, positionInBlock: %i , buf[bytesRead]: %i,readBuf[positionInBlock]: %i\n ",bytesRead-1,positionInBlock-1, buf[bytesRead-1], readBuf[positionInBlock-1]);

		}
		positionInBlock = 0;

		//Nächste Blocknummer holen
		int next = 0;
		fat->getNext(currentBlock, &next);
		currentBlock = next;
		currentBlockNumber++;
		//LOGF("currentBlock: %i \n", currentBlock);
	}

	//buf[bytesRead]=char(0);
	//string strBuf(buf);
	//printf("sizeBuf: %i , buf: %s \n", strBuf.length(), buf);
	file->setAccessTime(time(NULL));
	root->copyFile(path, file);

	//buffer neu setzen

	if (fileInfo->writepage != currentBlockNumber) {
		LOGF("neue filebuffer von currentBlockNumber: %i wird gesetzt",currentBlockNumber);
		root->writeToPuffer(path,readBuf);
		int tmp=currentBlockNumber;
		fileInfo->writepage = tmp;
		LOGF("fileInfo->writepage: %i\n",fileInfo->writepage);
	}


	delete[] readBuf;
	delete file;
	//LOGF("readBuf : %s \n", buf);
	LOG("END readFile \n");
	RETURN(size);

}
// int fuseCreate(const char *, mode_t, struct fuse_file_info *);
int MyFS::addFile(const char * name, mode_t mode, time_t mtime, off_t size,
		char * text) {

	LOG("********************************************************************************************** ");
	LOG("addFile start");
	LOGF("add file with name : %s \n", name);

	int numberNeededBlocks = size / BD_BLOCK_SIZE;
	int bytesInLastBlock = size % BD_BLOCK_SIZE;
	//file passt nicht vollständig in blöcke
	if (bytesInLastBlock != 0) {
		numberNeededBlocks++;
	}

	//LOGF("numberNeededBlocks : %i \n", numberNeededBlocks);
	//LOGF("BytesInLastBlock: %i \n", bytesInLastBlock);

	//array zum merken der blocknummern
	int * blocksUse = new int[numberNeededBlocks + 1];
	blocksUse[numberNeededBlocks] = -1; //keine Verlinkung vom letzten Block aus

	int returnCode;

	//Freie blöcke finden
	returnCode = dmap->getFreeBlocks(numberNeededBlocks, &blocksUse);
	if (returnCode == -1) {
		printf("error in addFile no free blocks in dmap \n");
		RETURN(-ENOSPC);
	}

	//file ins root schreiben
	returnCode = root->addFile(name, size, mode, mtime, blocksUse[0]);
	if (returnCode != 0) {
		printf(
				"error in addFile in root->addFile(name, size, mode,st_mtime,blocks[0] \n");
		RETURN(returnCode);
	}

	int i = 0;
	while (numberNeededBlocks != 0) {
		//1. Als belegt markieren
		dmap->setUsed(blocksUse[i]);

		//2. Verlinken in Fat
		//printf("blocksUse[i]:%i , blocksUse[i + 1]:%i \n", blocksUse[i], blocksUse[i + 1]);
		returnCode = fat->link(blocksUse[i], &blocksUse[i + 1]);
		if (returnCode == -1) {
			printf("Error: Problem with linking blocks in fat");
			RETURN(-ENOMSG);
		}
		// 3. Inhalt schreiben
		//Ausnahme: letzter Block ist nicht ganz voll
		if (numberNeededBlocks == 1 && bytesInLastBlock != 0) {
			char * buffer = new char[BD_BLOCK_SIZE];

			for (int index = 0; index < bytesInLastBlock; index++) { // Gültigen Inhalt schreiben
				buffer[index] = *text++;
			}
			for (int index = bytesInLastBlock; index < BD_BLOCK_SIZE; index++) { // Rest mit 0 füllen
				buffer[index] = char(0);
			}
			//printf("Puffer %s \n",buffer);

			//if(size<5000)
			//printf("write in BlockNr %i , buffer : %s \n",blocksUse[i],buffer );

			//if (size < 5000)
			//printf("write in BlockNr %i , buffer : %s \n", blocksUse[i], buffer);
			returnCode = this->blocks->write(blocksUse[i], buffer);
			delete[] buffer;
		} else {
			/*char * temp = new char[BD_BLOCK_SIZE];
			 temp=text;
			 if(size<5000)
			 printf("write in BlockNr %i , buffer : %s \n",blocksUse[i],temp );*/
			returnCode = this->blocks->write(blocksUse[i], text);
			text += BD_BLOCK_SIZE; // pointer weiter schieben

		}
		if (returnCode == -1) {
			printf("Error: Problem with writing to blockdevice");
			RETURN(-ENOMSG);
		}
		numberNeededBlocks--;
		i++;
	}

	delete[] blocksUse;
	LOG("addFile succes");
	//printf("addFile succes \n");
	RETURN(0);
}

//int fuseUnlink(const char *path);
int MyFS::deleteFile(const char *name) {

	MyFile * fcopy = new MyFile();
	if (root->getFile(name, fcopy) == -1) {
		printf("ERROE: File doesn't exist \n");
		RETURN(-ENOENT);
	}

	if (root->deleteFile(name) == -1) {
		printf("ERROR in Delete File");
		RETURN(-ENOMSG);
	}

	int blocksNumber = fcopy->getSize() / BD_BLOCK_SIZE;
	if (fcopy->getSize() % BD_BLOCK_SIZE != 0)
		blocksNumber++;
	int currentBlock = fcopy->getFirstBlock();

	//////////////////////////////////////
	char * text = new char[BD_BLOCK_SIZE];
	for (int i = 0; i < BD_BLOCK_SIZE; i++)
		text[i] = char(0);
	//text -= fcopy.getSize();
	//////////////////////////////////////
	while (blocksNumber != 0 && currentBlock != -1) {
		if (this->blocks->write(currentBlock, text) == -1) {
			LOG("error in addFile in this->blocks.write(i,    ) \n");
		}

		if (dmap->setUnused(currentBlock) == -1) {
			LOG("error in deleteFile in dmap.setUnused(currentBlock)");
			RETURN(-ENOMSG);
		}

		int blockBefore = currentBlock;
		if (fat->getNext(currentBlock, &currentBlock) == -1) {
			LOG("error in deleteFeil in fat.getNext(currentBlock, &currentBlock");
			RETURN(-ENOMSG);
		}

		if (fat->unLink(blockBefore) == -1) {
			LOG("error in deleteFeil in fat.unLink(blockBefore)");
			RETURN(-ENOMSG);
		}

		blocksNumber--;
	}
	delete fcopy;
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
	LOG("1");LOG("2");

	if (strcmp(path, "/") != 0)
		if (root->getFile(path + 1, f) == -1) {

			LOGF("Cant find a file with path: %s", path);
			RETURN(-ENOENT);
		}

	LOG("Now starting to set attributes");

	if (strcmp(path, "/") == 0) {
		LOG("Path == /");

		st->st_mode = S_IFDIR | 0555;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
		//size = sum size all files in dir
		st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
		st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
		st->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
		//st->st_mtime = fcopy.getLastMod(); ??? Should be here the current time?
		st->st_mtime = time(NULL); // The last "m"odification of the file/directory is right now
		st->st_ctime = time(NULL);
	} else {
		LOG("Anderere Path als \"/\" wurde abgefragt");
		st->st_atime = f->getAccessTime();
		st->st_ctime = f->getChangeTime();
		st->st_mtime = f->getModifyTime();
		st->st_mode = S_IFREG | 0444;
		st->st_nlink = 1;
		LOGF("size from %s is %i", path, f->getSize());
		st->st_size = f->getSize();
		st->st_blocks =
				(f->getSize() % BLOCK_SIZE == 0) ?
						f->getSize() / BLOCK_NUMBER :
						(f->getSize() / BLOCK_NUMBER + 1);

	}

	delete f;
	LOG("fuseGetattr erfolgreich beendet");
	RETURN(0);

}

int MyFS::fuseMknod(const char *path, mode_t mode, dev_t dev) {
	LOGM()
	;

	LOGF("fuseMKnod start , path : %s \n ", path);

	if (root->getSize() >= NUM_DIR_ENTRIES) {
		LOG("ERROR: es gibt schon zu viel Dateien \n");
		RETURN(-ENOMSG);
	}

	if (root->existName(path + 1)) {
		LOG("ERROE: DATEINAME EXISTIERT SCHON \n");
		RETURN(-EEXIST);
	}
	string t(path + 1);
	if (t.length() > 255) {
		LOG("DATEINAME ZU GROSS \n");
		printf("DATEINAME ZU GROSS \n");
		RETURN(-ENAMETOOLONG);
	}

	int * blocks = new int[1];

	LOGF("dmap->getFreeBlocks(1, blocks) : %i \n", dmap->getFreeBlocks(1, &blocks));
	int returnCode;

	//1. Freien Block suchen
	returnCode = dmap->getFreeBlocks(1, &blocks);
	if (returnCode == -1) {
		LOG("ERROR: not enough space on device \n");
		RETURN(-ENOSPC);
	}

	// 2. File ins root schreiben
	returnCode = root->addFile(path + 1, 0, mode, time(NULL), blocks[0]);
	if (returnCode < 0) {
		LOG("can't add file in root. Error in root.addFile(path, 0, S_IFREG | 0444) \n");
		RETURN(returnCode);
	}LOGF("Added File : %s to root \n ", path + 1);

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

	LOGF("Requested path in Fuse Open = %s ", path);

	LOG("1");LOGF("root->existName(%s) == %i ", path + 1, root->existName(path + 1));
	if (root->existName(path + 1)) {
		LOGF("name %s exist \n", path);
		fileInfo->fh = 1;
		RETURN(0);

	}LOG("2");

	fileInfo->fh = 0;
	RETURN(-ENOENT);
}

int MyFS::fuseRead(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fileInfo) {
	LOGF("fuseRead start path: %s size: %i offset: %i \n", path, size, offset);

	//Überprüfen, dass file existiert
	/*int returnValue = root->existName(path + 1);
	 LOGF("root->existName(%s) == %i \n", path + 1, returnValue);
	 if (!returnValue) {
	 RETURN(-ENOENT);LOG("root->existName(path+1)==0 \n");
	 }*/

	//printf("%s \n",buf);
	//LOGF("buf in fuseRead: %s", buf);
	if (fileInfo->fh == 1) {
		RETURN(readFile(path + 1, buf, size, offset, fileInfo));
	}

	RETURN(-ENOENT);
}

/*int MyFS::fuseWrite
 - mit fuseWrite kann man den Inhalt einer Datei veraendern
 - mit size kann ich festlegen, wie viel ich veraendern moechte, mit offset, wo in der Datei ich
 etwas aendern moechte und buf ist mein Inhalt, den ich in die schon vorhandene Datei einfuegen
 moechte*/
int MyFS::fuseWrite(const char *path, const char *buf, size_t size,
		off_t offset, struct fuse_file_info *fileInfo) {
	LOGM()
	;

	LOGF("fusewrite start %s \n", path);
	//LOGF("fusewrite buf %s \n", buf);
	LOGF("in fuseWrite size : %i offset: %i \n", size, offset);

	//fileinfos holen
	MyFile *file = new MyFile();
	int returnValue;
	returnValue = root->getFile(path + 1, file);
	if (returnValue == -1) {
		LOGF("cant find file with name %s in root", path + 1);
		RETURN(-ENOENT);
	}

	//Aktuelle file eigenschaften
	int fileSize = file->getSize();
	int usedBlocks = fileSize / BD_BLOCK_SIZE;
	int bytesLastBlock = fileSize % BD_BLOCK_SIZE;
	if (bytesLastBlock != 0) {
		usedBlocks++;
	}LOGF("fileSize: %i usedBlocks: %i bytesLAstBlock: %i\n", fileSize, usedBlocks, bytesLastBlock);

	//Weitere Blöcke allokieren wenn benötigt
	int bytesToAdd = ((int) offset + (int) size) - (usedBlocks * BD_BLOCK_SIZE);
	LOGF("bytesToAdd: %i\n", bytesToAdd);

	if (bytesToAdd > 0 && (bytesLastBlock != 0 || fileSize == 0)) {
		LOGF("In last Block is space for %i Blocks", BD_BLOCK_SIZE - bytesLastBlock);
		bytesToAdd -= BD_BLOCK_SIZE - bytesLastBlock;
	}

	if (bytesToAdd > 0) {
		LOG("byteyToAdd > 0 -> new blocks are going to be added\n");
		// Wenn im letzen Block noch platz, dann diesem Platz abziehen

		// Benötigte Blöcke berechen
		int blocksToAdd = bytesToAdd / BD_BLOCK_SIZE;
		if (bytesToAdd % BD_BLOCK_SIZE != 0) { //weitern block hinzu, wenn es nicht aufgeht
			blocksToAdd++;
		}LOGF("blocksToAdd: %i\n", blocksToAdd);

		//Aus dmap freie blöcke suchen
		int * blocksToAddArray = new int[blocksToAdd + 1];
		blocksToAddArray[blocksToAdd] = -1;
		returnValue=dmap->getFreeBlocks(blocksToAdd, &blocksToAddArray);
		if(returnValue==-1){
			LOG("keine blöcke mehr frei in dmap\n");
			RETURN(-ENOSPC);
		}

		//Letzten allokierten block bekommen
		int currentBlock = file->getFirstBlock();
		int next = 0;

		while (1) {
			fat->getNext(currentBlock, &next);
			if (next != -1)
				currentBlock = next;
			else
				break;
		}

		//Blöcke linken
		returnValue = fat->link(currentBlock, &blocksToAddArray[0]);
		if (returnValue == -1) {
			LOGF("Error occured: Cant link  fat entry at Fatindex %i to block %i\n", currentBlock, blocksToAddArray[0]);
			RETURN(-ENOMSG);  // No message of desired type
		}LOGF("position: %i  -> %i\n ", currentBlock, blocksToAddArray[0]);

		for (int i = 0; i < blocksToAdd; i++) {
			dmap->setUsed(blocksToAddArray[i]);
			returnValue = fat->link(blocksToAddArray[i],
					&blocksToAddArray[i + 1]);

			if (returnValue == -1) {
				LOGF("Error occured: Cant link fat entry at Fatindex %i to block %i\n", i, blocksToAddArray[i]);
				RETURN(-ENOMSG);  // No message of desired type
			}LOGF("position: %i  -> %i\n ", blocksToAddArray[i], blocksToAddArray[i + 1]);
		}

	}

	//Startposition zum schreiben berechen
	int blockToWrite = offset / BD_BLOCK_SIZE;
	int positionInBlock = offset % BD_BLOCK_SIZE;
	LOGF("blockToWrite: %i positionInBlock: %i\n", blockToWrite, positionInBlock);

	//Block zum schreiben suchen
	int currentBlock = file->getFirstBlock();
	int next = 0;
	for (int i = 0; i < blockToWrite; i++) {
		fat->getNext(currentBlock, &next);
		currentBlock = next;
	}



	int bytesToWrite = size;
	char * bufWrite = new char[BD_BLOCK_SIZE];
	while (bytesToWrite != 0) {
		LOG("start while_________________________________________________\n");
		//schreiben eines nicht kompletten Blocks
		if (positionInBlock != 0 || bytesToWrite < BD_BLOCK_SIZE) {

			if (blockToWrite > usedBlocks || fileSize == 0) { // neu allokierter block //muss mit char(0) gefüllt werden
				LOGF("blockToWrite:%i usedBlocks:%i\n", blockToWrite, usedBlocks);
				for (int i = 0; i < BD_BLOCK_SIZE; i++) {
					bufWrite[i] = char(0);
				}
			} else {  //bereits beschriebener block

				/*if (fileInfo->keep_cache == blockToWrite+1){
					LOGF("fileInfo->keep_cache: %i\n",fileInfo->keep_cache);
					root->writeFromPuffer(path+1,bufWrite);
				} else {*/
					blocks->read(currentBlock, bufWrite);
				//}

				LOGF("Aus %i gelesen bufWrite %s\n", currentBlock, bufWrite);

			}

			// zu schreibende Zeichen mit alten Zeichen kombinieren
			LOGF("In currentBlock: %i wird an positionToWrite: %i geschrieben. bytesToWrite: %i\n", currentBlock,
					positionInBlock, bytesToWrite);
			while (bytesToWrite != 0 && positionInBlock < BD_BLOCK_SIZE) {
				bufWrite[positionInBlock++] = *buf;
				buf++;
				bytesToWrite--;
			}

			LOGF("in Block %i schreiben bufWrite %s \n", currentBlock, bufWrite);
			blocks->write(currentBlock, bufWrite);
			positionInBlock = 0;
			LOG("1\n");

		} else {  //ganzen block schreiben

			for (int i = 0; i < BD_BLOCK_SIZE; i++) {
				bufWrite[i] = buf[i];
			}LOGF("buf:%s\n", buf);
			blocks->write(currentBlock, bufWrite);
			LOGF("Ganzer Block currentBLock: %i beschrieben. bytesToWrite: %i\n", currentBlock, bytesToWrite);LOGF("in Block %i schreiben bufWrite %s \n", currentBlock, bufWrite);
			buf += BD_BLOCK_SIZE;
			*bufWrite += *buf;
			bytesToWrite -= BD_BLOCK_SIZE;
		}

		//Nächsten block aus fat holen
		fat->getNext(currentBlock, &next);
		currentBlock = next;
		blockToWrite++;
	}

	delete[] bufWrite;

	if (((int) size + (int) offset) > fileSize) {
		fileSize = (int)size + (int)offset;
		file->setSize(fileSize);
	}

	file->setModifyTime(time(NULL));
	file->setChangeTime(time(NULL));

	root->copyFile(path+1, file);
	LOGF("fileSize:%i\n", fileSize);


	root->showFile(path+1);

	return size;

}

int MyFS::fuseRelease(const char *path, struct fuse_file_info *fileInfo) {
	LOGM()
	;

	fileInfo->fh = 0;

	RETURN(0);
}

int MyFS::fuseOpendir(const char *path, struct fuse_file_info *fileInfo) { 
	LOGM()
	;

	LOGF("Requested path in Fuse Open dir = %s ", path);

	int templog = (strcmp(path, "/"));

	LOGF("strcmp(path, \"/\") returns: %d", templog);

	if (strcmp(path, "/") == 0) {
		printf("Opening root directory");
		fileInfo->fh = 1;
		RETURN(0);
	}

	RETURN(-ENOENT);
}

int MyFS::fuseReaddir(const char *path, void *buffer, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fileInfo) {
	LOG("start fuseReaddir \n");

	int count = 0;
	LOGF("--> Getting The List of Files of %s\n", path);

	filler(buffer, ".", NULL, 0);	// Current Directory
	filler(buffer, "..", NULL, 0);	// Parent Directory

	LOG(" ready with filler");	// Parent Directory\n");

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
		LOG("filler success \n");LOGF("count : %i \n", count);

		delete[] name;

	}LOG("readDir success\n");
	RETURN(0);
	// <<< My new code
}

int MyFS::fuseReleasedir(const char *path, struct fuse_file_info *fileInfo) {
	LOGM()
	;

	fileInfo->fh = 0;	// Julia: Sonst noch was loeschen?

	RETURN(0);
}

int MyFS::fuseCreate(const char *path, mode_t mode,
		struct fuse_file_info *fileInfo) {
	LOGM()
	;

	LOGF("fuseMKnod start , path : %s \n ", path);

	if (root->getSize() >= NUM_DIR_ENTRIES) {
		LOG("ERROR: too many Files in root\n");
		RETURN(-ENOMSG);
	}

	if (root->existName(path + 1)) {
		LOG("ERROR: DATEINAME EXISTIERT SCHON \n");
		RETURN(-EEXIST);
	}
	string t(path + 1);
	if (t.length() > 255) {
		LOG("ERROR: DATEINAME ZU GROSS \n");
		printf("DATEINAME ZU GROSS \n");
		RETURN(-EFBIG);
	}

	int * blocks = new int[1];

	LOG("0\n");LOGF("dmap->getFreeBlocks(1, blocks) : %i \n", dmap->getFreeBlocks(1, &blocks));
	if (dmap->getFreeBlocks(1, &blocks) == -1) {
		LOG("can't add file in root dmap is full. Error in dmap.getFreeBlocks(1,blocks) \n");
		RETURN(-ENOSPC);
	}

	LOG("1\n");

	int retCode = root->addFile(path + 1, 0, mode, time(NULL), blocks[0]);
	if (retCode < 0) {
		LOG("can't add file in root. Error in root.addFile(path, 0, S_IFREG | 0444) \n");
		RETURN(retCode);

		LOG("2\n");
	}

	if (dmap->setUsed(blocks[0]) == -1) {
		LOG("can't add file in root. Error in dmap->setUsed(blocks[0]) \n");
		RETURN(-ENOMSG);
	}

	writeBlockDevice();
	RETURN(0);

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

		LOG("Starting logging...\n");LOGM()
		;

		// you can get the containfer file name here:
		LOGF("Container file name: %s", ((MyFsInfo * ) fuse_get_context()->private_data)->contFile);

		char * nameCont =
				((MyFsInfo *) fuse_get_context()->private_data)->contFile;

		LOG("try to open container \n");
		blocks->open(nameCont);
		LOG(" open container sucess \n");

		root->read(ROOT_START, blocks);
		LOG(" open root sucess \n");
		dmap->read(DMAP_START, blocks);
		LOG(" open dmap sucess \n");
		fat->read(FAT_START, blocks);
		LOG(" open fat sucess \n");

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
	LOGM();
	LOGF("newSize: %i\n",newSize);

	if (newSize == 0) {
		MyFile *file = new MyFile();
		int returnCode;
		returnCode = root->getFile(path + 1, file);
		if (returnCode == -1) {
			LOGF("cant find file with name %s in root", path + 1);
			RETURN(-ENOENT);
		}

		int currentBlock = file->getFirstBlock();
		int next = 0;
		while (1) {
			fat->getNext(currentBlock, &next);
			if (next == -1) {
				break;
			} else {
				currentBlock = next;
				returnCode = fat->unLink(currentBlock);
				if (returnCode == -1) {
					LOGF("cant unlink in fat currentBlock: %i ",currentBlock);
					RETURN(-ENOMSG);
				}
				dmap->setUnused(currentBlock);
				if (returnCode == -1) {
					LOGF("cant free in dma p currentBlock: %i ",currentBlock);
					RETURN(-ENOMSG);
				}
			}
		}
		root->setSize(path + 1, 0);
		delete file;

	} else {
		RETURN(-ENOSYS); /* Function not implemented */
	}

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

int MyFS::fuseFsyncdir(const char *path, int datasync,
		struct fuse_file_info *fileInfo) {
	LOGM()
	;
	RETURN(0);
}

int MyFS::fuseTruncate(const char *path, off_t offset,
		struct fuse_file_info *fileInfo) {
	LOGM();
	RETURN(0);
}

void MyFS::fuseDestroy() {
	LOGM()
	;
}

#ifdef __APPLE__
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x) {
#else
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value,
		size_t size, int flags) {
#endif
	LOGM()
	;
	RETURN(0);
}

#ifdef __APPLE__
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x) {
#else
int MyFS::fuseGetxattr(const char *path, const char *name, char *value,
		size_t size) {
#endif
	LOGM()
	;
	RETURN(0);
}

