
#include "catch.hpp"


#include "helper.hpp"
#include "myfs.h"


#include "dMap.h"


TEST_CASE( "Set/unset/find Blocks", "[dMap]" ) {


	dMap* dmap = new dMap();


	 SECTION("get 2 Blocks") {
			 dmap->setUsed(1);

			 int * result  = new int [6];

			 dmap->getFreeBlocks(3,&result);
			 REQUIRE( result[0]== 3);
			 //printf("%s",result[1]);
			 REQUIRE( result[1]== 2);
			 REQUIRE( result[2]== 0);

			 delete[] result;
	 }


	 delete dmap;
}













