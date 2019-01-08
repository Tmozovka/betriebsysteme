#pragma once
#include "blockdevice.h"
#include "myfs-structs.h"

class MyFAT {

private:
	int table[BLOCK_NUMBER];
	int size = BLOCK_NUMBER;
public:
	MyFAT();
	MyFAT(char * b);
	MyFAT(BlockDevice * blocks, int start);
	~MyFAT();

	int link(int currentNo, int* nextNo);
	int unLink(int currentNo);
	int getNext(int current, int* next);
	int getSize() {
		return size;
	}
	;

	void showFat();

	void resize(char * text, int oldSize, int newSize);
	char * writeBlock();
	friend int compare(MyFAT f1, MyFAT f2);
	void writeBlockDevice(BlockDevice * b,int start);
	void readBlockDevice( BlockDevice * blocks,int start,char * newBuf);
	/*int firstBlock:
	 int nextBlock;
	 int currentBlock;
	 int fileLength;*/

};
