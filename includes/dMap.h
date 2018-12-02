
#include "blockdevice.h"

class dMap{


private:
	//+1 because Blockno 0 is not used
	static int dmap[BLOCK_NUMBER+1];
	static int firstFreeBlock;

public:

	~dMap();
	dMap();


	//returns -1 if no more blocks available
	int getFreeBlocks(int neededBlocks,  int* returnArray[]);
	int setUsed(int blockNumber);
	int setUnused(int blockNumber);

};
