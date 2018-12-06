#include "dMap.h"
//#include "blockdevice.h"
dMap::dMap() {


	for (int i = 0; i < BLOCK_NUMBER; i++)
			dmap[i]=0;
	firstFreeBlock = 0;
	printf("Konstruktor von dMap ist beendet \n");
}

dMap::~dMap() {

	//Blockno 0 is not used
	for (int i = 0; i < BLOCK_NUMBER; i++)
		dmap[i]=-1;
	printf("Destruktor von dMap ist beendet \n");
}

int dMap::getFreeBlocks(int neededBlocks,  int** returnArray) {

	int * array = *returnArray;
	for (int i = firstFreeBlock; i < BLOCK_NUMBER; i++) {

		if (dmap[i] == 0) {
			array[neededBlocks - 1] = i; //Fehler hier
			neededBlocks--;
		}
		//Still more blocks needed?
		if (neededBlocks == 0) {
			return 0;
		}
	}

	// Blockdevice full, no free blocks found
	return -1;
}


int dMap::setUsed(int blockNumber) {

	if(blockNumber>=BLOCK_NUMBER||blockNumber<0)
		return -1;

	dmap[blockNumber] = 1;
	return 0;

}

int dMap::setUnused(int blockNumber) {
if(blockNumber>=BLOCK_NUMBER||blockNumber<0)
			return -1;

	dmap[blockNumber] = 0;

	// Update first free block
	if (blockNumber < firstFreeBlock) {
		firstFreeBlock = blockNumber;
	}
return 0;
}
