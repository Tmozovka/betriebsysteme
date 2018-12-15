#include "catch.hpp"
#include "helper.hpp"
#include "fat.h"
#include "blockdevice.h"

TEST_CASE( "Set/get/delete Allocated Blocks", "[fat]" ) {

	MyFAT* fat = new MyFAT();

	// Was passiert wenn, alle Bloecke gelinkt sind? Und fat voll ist?


	 SECTION("unLink") {

		 int next=4;
		 REQUIRE(fat->link(5,&next)==0);

		 REQUIRE(fat->getNext(5,&next)==0);
		 REQUIRE(next == 4);

		 fat->unLink(5);
		 REQUIRE(fat->getNext(5,&next)==0);
		 REQUIRE(next == -1);



	 }

	 SECTION("link") {

		 int next=4;
		 REQUIRE(fat->link(5, &next)==0);
		 REQUIRE(fat->getNext(5,&next) == 0);
		 REQUIRE(next == 4);


		 }



	 delete fat;
}
