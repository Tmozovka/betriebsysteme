#include "dMap.h"
//#include "blockdevice.h"

void dMap::showDmap() {
	printf(
			"****************************************************************\n");
	printf("DMAP: \n");
	for (int i = 0; i != 50; i++) {

		printf("%i -> %c \n", i, dmap.test(i));
	}
}

dMap::dMap() {

	//is not needed because biset is initalised with 0;
	//for (int i = 0; i < BLOCK_NUMBER; i++)
		//dmap[i] = 0;

	firstFreeBlock = 0;
	printf("Konstruktor von dMap ist beendet \n");
}

dMap::~dMap() {

	for (int i = 0; i < BLOCK_NUMBER; i++)
		dmap[i] = -1;
	printf("Destruktor von dMap ist beendet \n");
}

int dMap::getFreeBlocks(int neededBlocks, int** returnArray) {

	int * array = *returnArray;
	int j = 0;
	for (int i = firstFreeBlock; i < BLOCK_NUMBER; i++) {

		if (dmap[i] == 0) {
			array[j++] = i;
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

	if (blockNumber >= BLOCK_NUMBER || blockNumber < 0)
		return -1;

	dmap[blockNumber] = 1;
	return 0;

}

int dMap::setUnused(int blockNumber) {
	if (blockNumber >= BLOCK_NUMBER || blockNumber < 0)
		return -1;

	dmap[blockNumber] = 0;

	// Update first free block
	if (blockNumber < firstFreeBlock) {
		firstFreeBlock = blockNumber;
	}
	return 0;
}

int dMap::init(int startingBlock,BlockDevice *blocks){


	//Da bitset verwendet entspricht BLOCK_SIZE der Anzahl an Benötigter BITS
	// BLOCK_NUMBER/8 == Anzahl an Benötigten Bytes
	//Anzahl an Benötigten Bytes / 512 = Anzahl an Benötigten Blöcken

	//TODO:
	//Puffer erstellen und dmap in 512 Byte große stücke unterteilen
	// char * puffer = new char[8*BD_BLOCK_SIZE];
	 //blocks->write(startingBlock,puffer);



	//while(noch teile übrig)
	//	blockdevice.write(startingBlock++, puffer);
	//return startingBlock;

	return -1;
}



