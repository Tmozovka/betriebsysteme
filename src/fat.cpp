#include "fat.h"
#include <string.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
using namespace std;

void MyFAT::showFat() {
	printf(
			"****************************************************************\n");
	printf("FAT: \n");
	for (int i = 0; i != 50; i++) {

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
	if (*next >= size || *next < 0 || current >= size || current < 0)
		return -1;
	else
		*next = table[current];
	return 0;
}

MyFAT::MyFAT() {

	for (int i = 0; i < size; i++)
		table[i] = -1;

	printf("Konstruktor von MyFat ist beendet \n");

}

MyFAT::~MyFAT() {
	for (int i = 0; i < size; i++)
		table[i] = -1;

	printf("Destruktor von MyFat ist beendet \n");
}

char * MyFAT::writeBlock() {
	char * result = new char[size * 2];
	char * next = new char[1];

	strcpy(result, to_string(this->table[0]).c_str());

	for (int i = 1; i < size; i++) {

		strcpy(next, to_string(this->table[i ]).c_str());
		strcat(result, next);

	}
	/*for (int i = nr*BLOCK_SIZE+BLOCK_SIZE; i < size; i++)
	 //table[i] = -1;
	 {

	 }*/
	//delete[] next;
	return result;
}

MyFAT::MyFAT(char * buf) {
	char * next = new char[1];
	for (int i = 0; i < size; i++) {

		if (*buf == '-' && *(buf + 1) == '1') {
			table[i] = -1;
			buf += 2;
		} else {
			next = (buf++);
			table[i] = atoi(next);
		}

	}

//	delete[] next;
}

int compare(MyFAT f1, MyFAT f2) {
	for (int i = 0; i < f1.getSize(); i++) {
		if (f1.table[i] != f2.table[i])
			return -1;
	}

	return 0;
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
