#include "catch.hpp"
#include "helper.hpp"
#include "fat.h"
#include "blockdevice.h"
#include <string.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
using namespace std;
TEST_CASE( "Set/get/delete Allocated Blocks", "[fat]" ) {

	MyFAT* fat = new MyFAT();

	// Was passiert wenn, alle Bloecke gelinkt sind? Und fat voll ist?

	SECTION("unLink"){

	int next=4;
	REQUIRE(fat->link(5,&next)==0);

	REQUIRE(fat->getNext(5,&next)==0);
	REQUIRE(next == 4);

	fat->unLink(5);
	REQUIRE(fat->getNext(5,&next)==0);
	REQUIRE(next == -1);

}

	SECTION("link"){

	int next=4;
	REQUIRE(fat->link(5, &next)==0);
	REQUIRE(fat->getNext(5,&next) == 0);
	REQUIRE(next == 4);

}

	SECTION("writeBlocks"){

	for(int i=0;i<60000;i++)
	{
		int next=i+1;
		fat->link(i, &next);
	}

	printf("START FAT TEST \n ");

	BlockDevice blocks;
	blocks.create("containerFatTest.bin");

	//write in blocks
	int nrBlocks=0;
	fat->writeBlockDevice(&blocks,200,&nrBlocks);
	printf("IN TEST FAT NUMBER WROTEN BLOCKS : %i \n", nrBlocks);
	//read from blocks
	MyFAT* newFat = new MyFAT(&blocks,200,nrBlocks);
	//newFat->showFat();
	remove("containerFatTest.bin");
	REQUIRE(compare(*fat, *newFat)==0);

	delete newFat;


}

	delete fat;
}

