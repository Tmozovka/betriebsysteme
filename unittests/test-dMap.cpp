#include "catch.hpp"

#include "helper.hpp"
#include "myfs.h"
#include "blockdevice.h"

#include "dMap.h"

TEST_CASE( " Set/unset/find Blocks", "[dMap]" ) {

	dMap* dmap = new dMap();

	SECTION("get 3 Blocks"){
	dmap->setUsed(1);

	int * result = new int [3];

	dmap->getFreeBlocks(3,&result);
	REQUIRE( result[0] == 0);
	REQUIRE( result[1] == 2);
	REQUIRE( result[2] == 3);
	delete[] result;

}


	SECTION("isSet()"){

	dmap->setUsed(1);
	dmap->setUsed(7);
	dmap->setUsed(59);
	REQUIRE(dmap->isSet(1)!=0);
	REQUIRE(dmap->isSet(7)!=0);
	REQUIRE(dmap->isSet(59)!=0);

	REQUIRE(dmap->isSet(0)==0);
	REQUIRE(dmap->isSet(2)==0);
	REQUIRE(dmap->isSet(60)==0);




}
	SECTION("get 3 Blocks after block is set free"){

	dmap->setUsed(1);
	dmap->setUnused(1);
	int * result = new int [3];

	dmap->getFreeBlocks(3,&result);
	REQUIRE( result[0] == 0);
	REQUIRE( result[1] == 1);
	REQUIRE( result[2] == 2);
	delete[] result;

}

	SECTION("get blocks with nearly full dmap"){

	for(int i= 0; i<BLOCK_NUMBER-1; i++) {
		dmap->setUsed(i);
	}

	int * result = new int [1];

	REQUIRE(dmap->getFreeBlocks(1,&result)==0);
	REQUIRE(result[0]==BLOCK_NUMBER-1);

	delete[] result;

}

	SECTION("get blocks with full dmap"){

	for(int i= 0; i<BLOCK_NUMBER; i++) {
		dmap->setUsed(i);
	}

	int * result = new int [1];
	REQUIRE(dmap->getFreeBlocks(1,&result)==-1);
	delete[] result;

}





SECTION("write on blockdevice with every 3^n block set  "){
	//dmap mit irgendwelchen werten belegen
	int j = 3;
	for(int i = 0; i<BLOCK_NUMBER;i++) {
		if(i==j) {
			dmap->setUsed(i);
			j*=3;
		}
	}
	//dmap->showDmap();
	remove("testdmap.bin");
	BlockDevice bd;
	bd.create("testdmap.bin");

	dmap->init(0,&bd);

	/*//Lesen ob etwas in bd geschrieben wurde
	char * buf= new char[BD_BLOCK_SIZE];
	bd.read(0,buf);
	for(int i = 0; i<BD_BLOCK_SIZE;i++)
		printf("Char nr %d lautet %c\n",i,buf[i]);
	 */

	//dmap mit anderen werten überschreiben
	for(int i = 0; i<BLOCK_NUMBER;i++) {
		dmap->setUsed(i);
	}

	dmap->read(0,&bd);
	//dmap->showDmap();
	j=3;
	for(int i = 0; i<BLOCK_NUMBER;i++) {

		if(i==j){
			j*=3;
			if(dmap->isSet(i)==0)
			printf("Nicht gesetzter Block bei %d\n",i);

			REQUIRE(dmap->isSet(i)!=0);
		}else{

			if(dmap->isSet(i)!=0)
			printf("Block zu viel gesetzt bei %d\n",i);

			REQUIRE(dmap->isSet(i)==0);
		}
	}
}




SECTION("write on blockdevice with every 5000th block set"){
	//dmap mit irgendwelchen werten belegen
	for(int i = 0; i<BLOCK_NUMBER;i++) {
		if(i%5000==0) {
			dmap->setUsed(i);
		}
	}

	//dmap->showDmap();
	remove("testdmap.bin");
	BlockDevice bd;
	bd.create("testdmap.bin");

	dmap->init(0,&bd);

	//dmap mit anderen werten überschreiben
	for(int i = 0; i<BLOCK_NUMBER;i++) {

		if(dmap->isSet(i)){
			dmap->setUnused(i);
		}else{
			dmap->setUsed(i);
		}

	}

	dmap->read(0,&bd);
	//dmap->showDmap();
	for(int i = 0; i<BLOCK_NUMBER;i++) {
		if(i%5000==0) {
			if(dmap->isSet(i)==0)
				printf("Nicht gesetzter Block bei %d\n",i);

			REQUIRE(dmap->isSet(i));
		} else {
			if(dmap->isSet(i))
							printf("Zu viel gesetzter Block bei %d\n",i);

			REQUIRE(dmap->isSet(i)==0);
		}
	}


}
SECTION("write on blockdevice with random bits"){

	//dmap mit zufallswerten belegn
	int * randomArray = new int[BLOCK_NUMBER];

	for(int i = 0; i<BLOCK_NUMBER; i++){
		randomArray[i]=  rand() % 2;
		if(randomArray[i]){
			dmap->setUsed(i);
		}
	}



	//dmap->showDmap();
	remove("testdmap.bin");
	BlockDevice bd;
	bd.create("testdmap.bin");

	dmap->init(0,&bd);

	//dmap mit anderen werten überschreiben
	for(int i = 0; i<BLOCK_NUMBER;i++) {
		if(dmap->isSet(i)){
			dmap->setUnused(i);
		}else{
			dmap->setUsed(i);
		}

	}

	dmap->read(0,&bd);
	//dmap->showDmap();

	for(int i = 0; i<BLOCK_NUMBER;i++) {
		if(randomArray[i]) {
			if(dmap->isSet(i)==0)
				printf("Nicht gesetzter Block bei %d\n",i);

			REQUIRE(dmap->isSet(i));
		} else {
			if(dmap->isSet(i))
							printf("Zu viel gesetzter Block bei %d\n",i);

			REQUIRE(dmap->isSet(i)==0);
		}
	}

delete[] randomArray;
}


	delete dmap;
}

