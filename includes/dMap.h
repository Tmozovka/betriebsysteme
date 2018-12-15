#pragma once
#include "blockdevice.h"

class dMap {

private:

	int dmap[BLOCK_NUMBER];
	int firstFreeBlock;

public:

	~dMap();
	dMap();

	//returns -1 if no more blocks available
	int getFreeBlocks(int neededBlocks, int** returnArray);
	int setUsed(int blockNumber);
	int setUnused(int blockNumber);

	void showDmap();

};
