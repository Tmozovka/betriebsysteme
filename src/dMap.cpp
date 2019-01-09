#include "dMap.h"
//#include "blockdevice.h"

void dMap::showDmap() {
	printf(
			"****************************************************************\n");
	printf("DMAP: \n");

	for (int i = 0; i != 30; i++) {

		printf("%i -> %c \n", i, dmap.test(i));
	}

	for (int i = 59049 - 5; i != 59049 + 5; i++) {

		printf("%i -> %c \n", i, dmap.test(i));
	}

	for (int i = BLOCK_NUMBER - 20; i != BLOCK_NUMBER; i++) {

		printf("%i -> %c \n", i, dmap.test(i));
	}
}

dMap::dMap() {

	firstFreeBlock = 0;
	//Erste BLOCKS_START Blöcke besetzt markieren
	for (int i = 0; i < BLOCKS_START; i++) {
		setUsed(i);
	}



	//erste Bloecke braucht man fuer Datenstrukturen
	/*for (int i = 0; i < BLOCKS_START; i++)
			dmap[i] = -1;

	for (int i = BLOCKS_START; i < BLOCK_NUMBER; i++)
		dmap[i] = 1;*/

	//printf("Konstruktor von dMap ist beendet \n");
}

dMap::~dMap() {

	//for (int i = 0; i < BLOCK_NUMBER; i++)
	//dmap[i] = -1;
	//printf("Destruktor von dMap ist beendet \n");
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

	if(blockNumber==firstFreeBlock){
		firstFreeBlock++;
	}

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

int dMap::isSet(int blockNumber) {
	return dmap.test(blockNumber);
}

int dMap::init(int startingBlock, BlockDevice *blocks) {

	//Da bitset verwendet, entspricht BLOCK_SIZE der Anzahl an Benötigter BITS
	// BLOCK_NUMBER/8 == Anzahl an Benötigten Bytes, entspricht Anzahl benötigter Chars

	//Anzahl an Benötigten Bytes / 512 = Anzahl an Benötigten Blöcken

	int pufferLength = BD_BLOCK_SIZE; //512
	char * puffer = new char[pufferLength];

	printf("Dmap->firstFreeBlock lautet vor dem Schreiben %d\n",firstFreeBlock);
	//firstFreeBlock in ersten Block schreiben

	for (int byteNr = 0; byteNr < 4; byteNr++) {
		char c = 'a';
		for (int charBit = 0; charBit < 8; charBit++) {
			if ((firstFreeBlock >> (byteNr * 8 + charBit)) & 1) { //Ist Bit von firstFreeBlock gesetzt?
				c |= 1 << charBit; //set charbit

			} else {
				c &= ~(1 << charBit); //unset charbit
			}

		}
		puffer[byteNr] = c;
	}

	blocks->write(startingBlock++, puffer);
	//***********************************************************************************
	//Nun dmap inhalt in Blockdevice schreiben
	int bitsLeft = BLOCK_NUMBER;
	int CurrentStartOfPufferInDmap = 0;

	while (bitsLeft >= pufferLength * 8) { // Puffer voller Länge auffüllen

		for (int charNumber = 0; charNumber < pufferLength; charNumber++) { //Puffergröße in Bytes zerteilt durchlaufen
			char c = 'a';

			for (int charBit = 0; charBit < 8; charBit++) { //Char mit 8 Bits befüllen
				if (dmap.test(
						CurrentStartOfPufferInDmap + charNumber * 8
								+ charBit)) {
					c |= 1 << charBit;
				} else {
					c &= ~(1 << charBit);
				}
				//if(charNumber ==pufferLength-1)
				//printf("eintragnummer in Char geschrieben: %d\n",CurrentStartOfPufferInDmap+charNumber*8+charBit);
			}
			puffer[charNumber] = c; // Char in Puffer schreiben

		}
		CurrentStartOfPufferInDmap += 4096;
		blocks->write(startingBlock++, puffer); //Puffer schreiben
		bitsLeft -= pufferLength * 8;
		//printf("In Block %d geschrieben. Letzter geschriebener Dmapeintrag: %d \n",startingBlock-1,BLOCK_NUMBER-bitsLeft-1);

	}

	printf("Alle ganzen Blöcke geschrieben. Jetzt noch %d Bits Übrig\n",
			bitsLeft);

	if (bitsLeft != 0) { //Puffer mit Rest auffüllen

		for (int charNumber = 0; charNumber < pufferLength; charNumber++) {
			char c = 'a';

			if (bitsLeft == 0) { //Rest noch auffüllen
				c = char(0);
			} else {
				for (int charBit = 0; charBit < 8; charBit++) { //Char mit 8 Bits befüllen

					if (bitsLeft == 0) { //Restliche Bits von Char ignorieren
						break;
					}

					if (dmap.test(
							CurrentStartOfPufferInDmap + charNumber * 8
									+ charBit)) {
						c |= 1 << charBit;
					} else {
						c &= ~(1 << charBit);
					}
					bitsLeft--;
				}
			}
			puffer[charNumber] = c; // Char in Puffer schreiben

		}
		blocks->write(startingBlock++, puffer);
		printf(
				"In Block %d geschrieben. Letzter geschriebener Dmapeintrag: %d \n",
				startingBlock - 1, BLOCK_NUMBER - bitsLeft - 1);
	}

	delete[] puffer;
	return startingBlock;
}

int dMap::read(int startingBlock, BlockDevice* blocks) {

	int currentBlock = startingBlock;
	int currentDmapIndex = 0;
	char * puffer = new char[BD_BLOCK_SIZE];

	// aus erstem block firstFreeBlock auslesen
	blocks->read(currentBlock++, puffer);
	firstFreeBlock= 0;


	//todo das noch testen
	for (int byteNr = 0; byteNr < 4; byteNr++) {
		char c = puffer[byteNr];
		for(int charBit= 0; charBit<8; charBit++){
			if((c>>(8*byteNr+charBit))&1){
				firstFreeBlock |= 1 << (8* byteNr +charBit);
			}

		}
	}
	printf("Dmap->firstFreeBlock lautet nach dem Lesen %d\n",firstFreeBlock);

	//dmap Blöcke auslesen
	while (currentDmapIndex < BLOCK_NUMBER) {

		blocks->read(currentBlock++, puffer); //currentBlock auslesen
		//printf("Aus Block %d gelesen \n",currentBlock-1);
		for (int charNumber = 0; charNumber < BD_BLOCK_SIZE; charNumber++) { //charbit auslesen
			char c = puffer[charNumber];

			for (int charBit = 0; charBit < 8; charBit++) {
				if (currentDmapIndex == BLOCK_NUMBER) {
					delete[] puffer;
					printf("Letzter für dmap ausgelesener Block: %d \n",
							currentBlock - 1);
					return currentBlock;
				}
				if ((c >> charBit) & 1) {
					dmap[currentDmapIndex++] = 1;
				} else {
					dmap[currentDmapIndex++] = 0;
				}
			}
		}
	}

	return -1;
}

