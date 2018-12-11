
#include "catch.hpp"


#include "helper.hpp"
#include "myfs.h"


#include "dMap.h"


TEST_CASE( "Set/unset/find Blocks", "[dMap]" ) {


	dMap* dmap = new dMap();

	 SECTION("get 3 Blocks") {
		 	dmap->setUsed(1);

		 int * result  = new int [3];

		 dmap->getFreeBlocks(3,&result);
		 REQUIRE( result[0] == 0);
		 REQUIRE( result[1] == 2);
		 REQUIRE( result[2] == 3);
		 delete[] result;

	 }

	 SECTION("get 3 Blocks after block is set free") {

		 	dmap->setUsed(1);
		 	dmap->setUnused(1);

		 int * result  = new int [3];

		 dmap->getFreeBlocks(3,&result);
		 REQUIRE( result[0] == 0);
		 REQUIRE( result[1] == 1);
		 REQUIRE( result[2] == 2);
		 delete[] result;

	 }


	 SECTION("get blocks with nearly full dmap") {

		 for(int i= 0; i<BLOCK_NUMBER-1; i++){
			 dmap->setUsed(i);
		 }

		 int * result  = new int [1];

		 REQUIRE(dmap->getFreeBlocks(1,&result)==0);
		 REQUIRE(result[0]==BLOCK_NUMBER-1);

		 delete[] result;

	 }

	 SECTION("get blocks with full dmap") {

		 for(int i= 0; i<BLOCK_NUMBER; i++){
			 dmap->setUsed(i);
		 }

		 int * result  = new int [1];
		 REQUIRE(dmap->getFreeBlocks(1,&result)==-1);
		 delete[] result;

	 }







	 delete dmap;
}













