#include "catch.hpp"
#include "helper.hpp"
#include "fat.h"
#include "blockdevice.h"

TEST_CASE( "Set/get/delete Allocated Blocks", "[fat]" ) {

	MyFAT* fat = new MyFAT();

	// Was passiert wenn, alle Bloecke gelinkt sind? Und fat voll ist?
	/*
	 * noch zu testen :
	 *  int unLink ( int currentNo);
      int getNext(int current, int* next);
      int getSize(){return size;};
      */

	 SECTION("unLink") {

		 int * table  = new int [10];

		 fat->unLink(5);
		 REQUIRE(table[5] == -1);

		 delete[] table;

	 }

	 SECTION("link") {

		 int * table  = new int [10];
		 int * next;
		 next = new int [5]; //int * next  = new int [5];

		 *next=4;
		 fat->link(5, next);
		 REQUIRE(table[5] == 4);

		 *next=-1;
		 fat->link(3, next);
		 REQUIRE(table[3] == -1); //Wenn kein Element folgt

		 delete[] table;

		 }

	 SECTION("getNext") {

		 	 int size = BLOCK_NUMBER;
		 	 int * next;

			 fat->getNext(5, next);
			 REQUIRE(*next == 5); //True

			 fat->getNext(-1, next);
			 REQUIRE(next == 0); //If end of block

			 *next = size;
			 fat->getNext(3, next); //If bigger than size
			 REQUIRE(*next == 3);

	 }

	 delete fat;
}
