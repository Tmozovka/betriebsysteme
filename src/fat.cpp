#include "fat.h"
#include <string.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
using namespace std;

void MyFAT::showFat(int start, int end) {
	printf(
			"****************************************************************\n");
	printf("FAT: \n");
	for (int i = start; i != end; i++) {

		if ((i + 1) != table[i])
			printf("%i -> %i \n", i, table[i]);
	}
}

int MyFAT::unLink(int current) { //todo fehle behandeln + header bearbeiten
	if (current >= size || current < 0)
		return -1;
	else
		table[current] = -1;
	return 0;

}

int MyFAT::link(int current, int* next) {

	if (*next >= size || *next < 0 || current >= size || current < 0)
		return -1;
	else
		table[current] = *next;
	return 0;
}

int MyFAT::getNext(int current, int*next) {
	//printf(" start *next: %i, size: %i, current: %i \n", *next, size, current);
	if (current >= size || current < 0)
		return -1;
	else {
		*next = table[current];
		//	printf(" end *next: %i, size: %i, current: %i \n", *next, size,	current);
	}
	return 0;
}

MyFAT::MyFAT() {

	for (int i = 0; i < BLOCKS_START; i++)
		table[i] = -2;

	for (int i = BLOCKS_START; i < size; i++)
		table[i] = -1;

	//printf("Konstruktor von MyFat ist beendet \n");

}

MyFAT::~MyFAT() {
	for (int i = 0; i < size; i++)
		table[i] = -1;

	//printf("Destruktor von MyFat ist beendet \n");
}

char * MyFAT::writeBlock() {
	char * result = new char[size * 6];

	char * temp = new char[1];
	temp[0] = '_';

	strcpy(result, to_string(this->table[0]).c_str());
	strcat(result, "_");

	for (int i = 1; i < size; i++) {

		char * next = new char[6]; //char * next = new char[5];
		strcpy(next, to_string(this->table[i]).c_str());
		strcat(result, next);
		strcat(result, "_");
		delete[] next;

	}

	temp[0] = char(0);
	strcat(result, temp);

	delete[] temp;

	return result;
}

void MyFAT::read(int start, BlockDevice * blocks) {
	//printf("start MyFAT(BlockDevice * blocks, int start) start : %i \n", start);
	printf("start of reading fat from blockdevice \n");
	start++;
	char * buffer = new char[BD_BLOCK_SIZE];

	blocks->read(start++, buffer);
	int bufferIndex = 0;
	//printf("buffer:%s \n", buffer);

	for (int fatIndex = 0; fatIndex < BLOCK_NUMBER; fatIndex++) { // durch alle fat einträge durchzählen
		char* fatEntry = new char[6];
		for (int i = 0; i < 6; i++) {
			fatEntry[i] = char(0);
		}

		int fatEntryIndex = 0;
		//printf("start while \n");

		while (buffer[bufferIndex] != '_') { // ein Zeichen füllen bis _ erreicht

			//printf("before fatEntry[fatEntryIndex]: ???, buffer[bufferIndex]: %c\n", buffer[bufferIndex]);
			char temp = buffer[bufferIndex];
			char *tempAdr = &buffer[bufferIndex];
			fatEntry[fatEntryIndex++] = buffer[bufferIndex++];
			//printf("after fatEntry[fatEntryIndex]: %c, buffer[bufferIndex]: %c\n",fatEntry[fatEntryIndex], buffer[bufferIndex]);

			if (bufferIndex == BD_BLOCK_SIZE
					|| buffer[bufferIndex] == char(0)) { // wenn buffer komplett gelesen, dann neuen buffer holen
				if (fatIndex < BLOCK_NUMBER-1) {
					blocks->read(start++, buffer);
					//printf("buffer:%s \n", buffer);
					bufferIndex = 0;
				}else
					break;

			}

		}

		bufferIndex++;
		if (bufferIndex == BD_BLOCK_SIZE || buffer[bufferIndex] == char(0)) { // wenn buffer komplett gelesen, dann neuen buffer holen
			if (fatIndex < BLOCK_NUMBER-1) {
				blocks->read(start++, buffer);
				//printf("buffer:%s \n", buffer);
				bufferIndex = 0;
			}
		}

	/*	if (fatIndex > 9930 && fatIndex < 10112)
			printf("Fat: %i -> %s \n", fatIndex, fatEntry);*/

		//if(fatIndex<50)
		//printf("Fat: %i -> %s \n", fatIndex, fatEntry);

		char * trash;
		table[fatIndex] = (int) strtol(fatEntry, &trash, 10);
	/*	if (fatIndex > 9930 && fatIndex < 10112)
			printf("fatIndex:%i, trash:%s \n", fatIndex, trash);*/

		delete[] fatEntry;
	}

	delete[] buffer;

	printf("end of reading fat from blockdevice \n");
	/*
	 printf("start MyFAT(BlockDevice * blocks, int start) start : %i \n", start);
	 char * buf = new char[BLOCK_NUMBER * 6];
	 this->readBlockDevice(blocks, start, buf);

	 //printf("readBlockDevice fat : %s \n", buf);
	 //printf("readBlockDevice fat end of first Block: %s \n", buf + 510);
	 //printf("readBlockDevice fat end of second Block: %s \n", buf + 510 * 2);

	 //MyFAT::MyFAT(char * buf)
	 for (int i = 0; i < size; i++) {

	 char * next = new char[5];
	 for (int j = 0; *buf != '_'; j++, buf++) {
	 next[j] = *buf;
	 }
	 buf++;
	 char * trash;
	 table[i] = (int) strtol(next, &trash, 10);
	 delete[] next;
	 }


	 */
}

MyFAT::MyFAT(BlockDevice * blocks, int start) {
	this->read(start, blocks);

	/*
	 char * buf = new char[BLOCK_NUMBER * 6];
	 this->readBlockDevice(blocks, start, buf);
	 printf("buf: %s", buf);

	 //printf("readBlockDevice fat : %s \n", buf);
	 //printf("readBlockDevice fat end of first Block: %s \n", buf + 510);
	 //printf("readBlockDevice fat end of second Block: %s \n", buf + 510 * 2);

	 //MyFAT::MyFAT(char * buf)
	 int bufCount = 0;
	 for (int i = 0; i < size; i++) {

	 char * next = new char[5];
	 for (int j = 0; buf[bufCount] != '_'; j++, buf++) {
	 next[j] = buf[bufCount];
	 }
	 bufCount++;
	 printf("bufCount %i \n", bufCount);
	 char * trash;
	 table[i] = (int) strtol(next, &trash, 10);
	 //table[i] = atoi(next);
	 delete[] next;
	 }

	 */
}
/*
 void MyFAT::readBlockDevice(BlockDevice * blocks, int start, char * newBuf) {
 char * readBuf = new char[512];

 blocks->read(start, readBuf);
 int j = atoi(readBuf);
 //printf("nrBlocks read from %i : %i \n", start, j);
 start++;

 int i = start;
 while (j != 0) {
 //printf("blocks->read from block %i  \n", i);
 blocks->read(i++, readBuf);
 //printf("bloks read success \n");
 strcat(newBuf, readBuf);
 //printf("strcat sucess \n");
 j--;

 }
 //printf("sucess, readBuf: %s \n", readBuf);
 delete[] readBuf;
 //printf("sucess end \n");
 string tmpstr(newBuf);
 //printf("size: %i \n", tmpstr.length());

 }*/

/*MyFAT::MyFAT(char * buf) {

 for (int i = 0; i < size; i++) {

 char * next = new char[5];
 for(int j=0;*buf!='_'; j++, buf++)
 {
 next[j]=*buf;
 }
 buf++;
 table[i] = atoi(next);
 delete[] next;
 }

 }*/
bool operator !=(MyFAT const &f1, MyFAT const& f2) {
	return compare(f1, f2) == -1;
}
bool operator ==(MyFAT const &f1, MyFAT const& f2) {
	return compare(f1, f2) == 0;
}

int compare(MyFAT f1, MyFAT f2) {
	for (int i = 0; i < f1.getSize(); i++) {
		if (f1.table[i] != f2.table[i]) {
			printf(
					"Fehler auf der Stelle %i , f1.table[i]= %i, f2.table[i]=%i\n",
					i, f1.table[i], f2.table[i]);
			return -1;
		}
	}

	return 0;
}

void MyFAT::writeBlockDevice(BlockDevice * blocks, int start) {
	printf("start for fat writeBlockDevice  start : %i \n", start);
	int nrBlocks = 0;
	char * buf = new char[size * 6];
	buf = this->writeBlock();
	//printf("fat: %s \n", buf);

	int firstBlock = start;
	start++;

	int countBuf = 0;
	while (buf[countBuf] != '\0') {
		char * writeBuf = new char[BLOCK_SIZE];
		//	char * startWriteBuf = writeBuf;

		int i = 0;

		for (; i < BLOCK_SIZE - 1; i++) {
			writeBuf[i] = buf[countBuf++];
			if (buf[countBuf] == '\0') {
				printf("break countBuf : %i \n", countBuf);
				break;
			}
		}

		//	printf("i: %i \n", i);
		//int restSize = BLOCK_SIZE - countBuf % BLOCK_SIZE;
		//printf("restSize: %i \n", restSize);
		for (int j = i; j < BLOCK_SIZE; j++)
			writeBuf[j] = char(0);

		//writeBuf=buf;
		//printf("writeBuf: %s \n", writeBuf);
		blocks->write(start++, writeBuf);
		(nrBlocks)++;
		//buf+=BLOCK_SIZE;

		//writeBuf = startWriteBuf;

		delete[] writeBuf;
	}

	char * temp = new char[BLOCK_SIZE];

	strcpy(temp, to_string(nrBlocks).c_str());
	temp[to_string(nrBlocks).length()] = char(0);
	//resize(buf, to_string(this->sizeRoot).length(), BLOCK_SIZE);

	for (int i = to_string(nrBlocks).length(); i < BLOCK_SIZE; i++)
		temp[i] = char(0);

	printf("nrBlocks write in %i : %s \n", firstBlock, temp);
	//resize(temp,to_string(nrBlocks).length(),BLOCK_SIZE);
	blocks->write(firstBlock, temp);

	delete[] buf;
	delete[] temp;
}


/*
 void MyFAT::readBlockDevice(BlockDevice * blocks, int start, char * newBuf) {

 char * readBuf = new char[BLOCK_SIZE];
 printf("start read BlockDevice \n");
 blocks->read(start, readBuf);
 //string tmp( readBuf);
 //printf("size ReadBuf : %i readBuf: %s \n",tmp.length(), readBuf);
 char * trash;
 int j = (int) strtol(readBuf, &trash, 10);
 //int j = atoi(readBuf);
 //printf("nrBlocks read from %i : %i \n", start, j);
 start++;

 int i = start;
 while (j != 0) {
 //printf("blocks->read from block %i  \n", i);
 blocks->read(i++, readBuf);
 //	printf("bloks read success readBuf:%s \n", readBuf);
 strcat(newBuf, readBuf);
 //	printf("strcat sucess ");
 j--;

 }
 //printf("sucess, readBuf: %s \n", readBuf);
 delete[] readBuf;
 //printf("sucess end \n");
 string tmpstr(newBuf);
 //printf("size: %i newBuf: %s \n", tmpstr.length(), newBuf);

 }*/


void MyFAT::resize(char * text, int oldSize, int newSize) {

	int i = newSize - oldSize;
	text += oldSize - 1;
	while (i != 0) {
		*(text++) = char(0);
		i--;
	}
	//*(text-1)='\0';
	text -= newSize;

}
/*char * MyFAT::BlockIntoBuffer(BlockDevice fatBlocks) {

 for (int i = 0; i < size; i++) {
 for (int j = 0; j < size; j++) {
 * buf[i]= * *fatBlocks[i];
 i++;
 j++;
 }
 }
 return * buf;
 }

 MyFAT::pufferIntoBlock(char * buf) {

 BlockDevice fatBlocks = new BlockDevice;

 for (int i = 0; i < size; i++) {
 for (int j = 0; j < size; j++) {
 *fatBlocks[i]= * buf[j];
 i++;
 j++;
 }
 }
 return fatBlocks;
 }*/
