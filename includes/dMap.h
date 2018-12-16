﻿#pragma once
#include "blockdevice.h"
#include <bitset>
#include "blockdevice.h"

class dMap {

private:


	std::bitset<BLOCK_NUMBER> dmap;
	int firstFreeBlock;



public:

	~dMap();
	dMap();

	//returns -1 if no more blocks available
	int getFreeBlocks(int neededBlocks, int** returnArray);
	int setUsed(int blockNumber);
	int setUnused(int blockNumber);
	int getBlock(int blockNumber);
	void showDmap();


	//returns next free block to continue to initialize
	//returns -1 if error occurred
	int init(int startingBlock, BlockDevice* blocks);

	//return next Block to read
	int read(int startingBlock, BlockDevice* blocks);


};
