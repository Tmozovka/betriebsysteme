#include "catch.hpp"

#include "helper.hpp"

#include "fat.h"


TEST_CASE( "Set/get/delite Allocated Blocks", "[fat]" ) {


	fat* dmap = new fat();


	 SECTION("delete") {

		 int current  = new int [];

		 fat->unLink(5);

		 REQUIRE(current[5] == -1);
		 delete[] current;

	 }

	 SECTION("link") {
		 int table[];
		 int * next;
		 next = new int [5]; //int * next  = new int [5];
		 *next=4;

		 fat->link(5, &next);

		 REQUIRE(table[5] == 4);
		 delete[] table;

		 }
	 SECTION("get") {
		 	 int current = 4;
		 	 int *next =current;
			 int table[];

			 fat->getNext(5, &next);

			 REQUIRE(table[5] == 4);
			 delete[] table;

			 }
	 delete fat;
}
