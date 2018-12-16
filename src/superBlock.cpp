#include "superBlock.h"

int SuperBlock::getOpen() {
	return open;
}

void SuperBlock::setOpen(int open) {
	this->open = open;
}

void SuperBlock::addOpen() {
	open++;
}

void SuperBlock::closeOpen() {
	open--;
}

int SuperBlock::init(int startingBlock,BlockDevice* blocks){

	//todo
	//open in blockdevice speichern

	return -1;
}

