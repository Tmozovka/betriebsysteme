/*#include "catch.hpp"
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

	SECTION("test operators == and !="){


		MyFAT* f0 = new MyFAT();
		int next=4;
		f0->link(5, &next);
		MyFAT* f1 = new MyFAT();
		f1->link(5, &next);
		MyFAT* f2 = new MyFAT();

		next=10;
		f0->link(6, &next);
		f1->link(6, &next);
		f2->link(6, &next);

		REQUIRE(*f2!=*f0);
		REQUIRE(*f1==*f0);

		delete f0;
		delete f1;
		delete f2;

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
	//int nrBlocks=0;
	fat->writeBlockDevice(&blocks,FAT_START);
//	printf("IN TEST FAT NUMBER WROTEN BLOCKS : %i \n", nrBlocks);
	//read from blocks
	MyFAT* newFat = new MyFAT(&blocks,FAT_START);
	//newFat->showFat();
	remove("containerFatTest.bin");
	REQUIRE(compare(*fat, *newFat)==0);

	delete newFat;


}

	SECTION("writeBlocks with read funktion"){

		for(int i=0;i<60000;i++)
		{
			int next=i+1;
			fat->link(i, &next);
		}

		printf("START FAT TEST \n ");

		BlockDevice blocks;
		blocks.create("containerFatTest.bin");

		//write in blocks
		//int nrBlocks=0;
		fat->writeBlockDevice(&blocks,FAT_START);
	//	printf("IN TEST FAT NUMBER WROTEN BLOCKS : %i \n", nrBlocks);
		//read from blocks
		MyFAT* newFat = new MyFAT();
		newFat->read(FAT_START,&blocks);
		//newFat->showFat();
		remove("containerFatTest.bin");
		REQUIRE(compare(*fat, *newFat)==0);

		delete newFat;


	}

	delete fat;
}
*/
