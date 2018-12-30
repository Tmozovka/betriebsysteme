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
	char * buf;
	buf = fat->writeBlock();
	//printf("fat: %s \n", buf);
	printf("START FAT TEST \n ");
	char * writeBuf = new char [512];
	char * readBuf = new char [512];

	BlockDevice blocks;
	remove("containerFatTest.bin");
	blocks.create("containerFatTest.bin");

	//write in blocks
	int i=0;
	while(*buf!='\0')
	{
		writeBuf=buf;
		blocks.write(i++,writeBuf);
		buf+=BLOCK_SIZE;
	}
	buf-=512*i;
	printf("IN TEST FAT NUMBER WROTEN BLOCKS : %i \n", i);
	//read from blocks
	char * newBuf = new char[BLOCK_NUMBER * 6];;
	int j=i;
	i=0;
	while(j!=0)
	{
		blocks.read(i++,readBuf);
		strcat(newBuf,readBuf);
		j--;
	}

	MyFAT* newFat = new MyFAT(newBuf);
	//newFat->showFat();
	REQUIRE(compare(*fat, *newFat)==0);
	delete [] buf;
	delete [] newBuf;
	delete newFat;

}

	delete fat;
}

