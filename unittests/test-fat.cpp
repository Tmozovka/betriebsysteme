#include "catch.hpp"
#include "helper.hpp"
#include "fat.h"
#include "blockdevice.h"

TEST_CASE( "Set/get/delete Allocated Blocks", "[fat]" ) {

	MyFAT* fat = new MyFAT();

	 SECTION("delete") {
		 int * table  = new int [10];
		 fat->unLink(5);
		 //REQUIRE(table[5] == -1);
		 delete[] table;
	 }
	 SECTION("link") {
		 int * table  = new int [10];
		 int * next;
		 next = new int [5]; //int * next  = new int [5];
		 *next=4;
		 fat->link(5, next);
		 REQUIRE(table[5] == 4);
		 delete[] table;
		 }
	 SECTION("get") {
		 	/*
		 	 int current = 4;
		 	 int *next =current;
			 int table[];
			 fat->getNext(5, next);
			 REQUIRE(table[5] == 4);
			 delete[] table;
			 */
			 }
	 delete fat;
}
