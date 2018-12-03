﻿
#include "blockdevice.h"

class dMap{


private:

	static int dmap[BLOCK_NUMBER];
	static int firstFreeBlock;

public:

	~dMap();
	dMap();


	//returns -1 if no more blocks available
	int getFreeBlocks(int neededBlocks,  int* returnArray[]);
	int setUsed(int blockNumber);
	int setUnused(int blockNumber);

};
