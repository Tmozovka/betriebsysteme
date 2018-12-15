#pragma once
#include "blockdevice.h"
class MyFAT {

private:
	int table[BLOCK_NUMBER];
	int size = BLOCK_NUMBER;
public:
	MyFAT();
	~MyFAT();

	int link(int currentNo, int* nextNo);
	int unLink(int currentNo);
	int getNext(int current, int* next);
	int getSize() {
		return size;
	}
	;

	void showFat();
	/*int firstBlock:
	 int nextBlock;
	 int currentBlock;
	 int fileLength;*/

};
