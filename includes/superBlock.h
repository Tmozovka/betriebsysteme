#pragma once
#include "blockdevice.h"

class SuperBlock {
private:
	int open;
public:
	int getOpen();
	void setOpen(int o);
	void addOpen();
	void closeOpen();


	// return next Block to write on, return -1 if error occurred
	int init(int startingBlock, BlockDevice* blocks);
};
