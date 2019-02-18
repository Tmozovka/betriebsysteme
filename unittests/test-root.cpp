#include "catch.hpp"

#include "helper.hpp"
#include "myfs.h"
#include <string>

#include "root.h"

TEST_CASE( "Add/get/delete File", "[root]" ) {

	MyRoot* myroot = new MyRoot();

	SECTION("add first File and compare(get) it"){
	MyFile * firstfile = new MyFile();
	int result = myroot->addFile(firstfile->getName(),firstfile->getSize(),firstfile->getMode(),firstfile->getLastMod(),firstfile->getFirstBlock());
	MyFile* mf = new MyFile();
	myroot->getFile(firstfile->getName(), mf);
	REQUIRE( result == 0);
	REQUIRE( *mf == *firstfile);

}
	SECTION("No addition possible: too many files in Root"){
	MyFile * f = new MyFile();
	char * name = new char[2];
	name[0]=char(97);
	name[1]=char(0);

	for(int i=0;i<=NUM_DIR_ENTRIES+1;i++) {

		printf("name %s \n", name);
		string namestr(name);
		MyFile * f = new MyFile(namestr,1,1000,1000,1000,time(NULL),time(NULL),time(NULL),-1);
		myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
		name[0] ++;
	}
	int result = myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
	REQUIRE(result==-1);
	delete f;
	delete [] name;
}

	SECTION("No addition possible: Name is too big"){
	MyFile * f = new MyFile();
	//f->setSize(NAME_LENGTH+1);

	char * nameBig = new char [NAME_LENGTH+2];

	for(int i=0; i<NAME_LENGTH+2;i++)
	{
		nameBig[i]='a';
	}

	*(nameBig+NAME_LENGTH+1)='\0';


	int result =myroot->addFile(nameBig,f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
	REQUIRE(result==-1);
	delete f;
}

	SECTION("No addition possible: Name already exists - Test 1"){
	MyFile * f = new MyFile();
	MyFile * fileSimilarName = new MyFile();

	f->setName("similarName.txt");
	fileSimilarName->setName(f->getName());

	MyRoot* twinsRoot = new MyRoot();
	twinsRoot->addFile(f->getName(),12345,12,1000000,50009);
	int result = twinsRoot->addFile(fileSimilarName->getName(),2345,13,1000001,50000);
	REQUIRE(result==-1);
	delete f;
	delete fileSimilarName;
	delete twinsRoot;
}
	SECTION("No addition possible: Name already exists - Test 2"){
	MyFile * f = new MyFile();

	f->setName("testzwei.txt");

	MyRoot* rootTwins = new MyRoot();
	rootTwins->addFile(f->getName(),12245,11,1000011,50005);
	int result = rootTwins->addFile("testzwei.txt",2445,14,1000101,40000);
	REQUIRE(result==-1);
	delete f;
	delete rootTwins;

}
	SECTION("No addition possible: Name already exists - Test 3"){
	MyRoot* rootTwins = new MyRoot("testdrei.txt",12244,16,50003);
	int result = rootTwins->addFile("testdrei.txt",2441,18,1010001,40001);
	REQUIRE(result==-1);
	delete rootTwins;
}

//-----deleteFile----------------------------------------------------------------------------------------
	SECTION("Delete File"){
	MyFile * f = new MyFile();
	MyFile * fileSimilarName = new MyFile();

	fileSimilarName->setName("similarName.txt");
	fileSimilarName->setName("similarName.txt");

	myroot->addFile(fileSimilarName->getName(),fileSimilarName->getSize(),fileSimilarName->getMode(),fileSimilarName->getLastMod(),fileSimilarName->getFirstBlock());
	myroot->deleteFile("similarName.txt");

	int result = myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
	REQUIRE(result==0);

	delete f;
	delete fileSimilarName;
}
//-----existName----------------------------------------------------------------------------------------
	SECTION("Check if the name already exists in root"){
	MyFile * f = new MyFile();
	MyFile * fileSimilarName = new MyFile();

	fileSimilarName->setName("similarName.txt");
	fileSimilarName->setName("similarName.txt");

	myroot->addFile(fileSimilarName->getName(),fileSimilarName->getSize(),fileSimilarName->getMode(),fileSimilarName->getLastMod(),fileSimilarName->getFirstBlock());

	bool boolResult=myroot->existName(f->getName());
	REQUIRE(boolResult==false);

	delete f;
}
//-----getArray----------------------------------------------------------------------------------------
	SECTION("Create an array with all filenames"){
	MyFile * f0 = new MyFile();
	MyFile * f1 = new MyFile();
	MyFile * f2 = new MyFile();
	MyFile * f3 = new MyFile();

	f0->setName("f_null.txt");
	f1->setName("f_eins.txt");
	f2->setName("f_zwei.txt");
	f3->setName("f_drei.txt");

	myroot->addFile(f0->getName(),f0->getSize(),f0->getMode(),f0->getLastMod(),f0->getFirstBlock());
	myroot->addFile(f1->getName(),f1->getSize(),f1->getMode(),f1->getLastMod(),f1->getFirstBlock());
	myroot->addFile(f2->getName(),f2->getSize(),f2->getMode(),f2->getLastMod(),f2->getFirstBlock());
	myroot->addFile(f3->getName(),f3->getSize(),f3->getMode(),f3->getLastMod(),f3->getFirstBlock());

	string * result;
	result=myroot->getArray();

	string* filenames;
	filenames = new string[4];
	filenames[0] = f0->getName();
	filenames[1] = f1->getName();
	filenames[2] = f2->getName();
	filenames[3] = f3->getName();

	REQUIRE(*result==*filenames);

	delete f0;
	delete f1;
	delete f2;
	delete f3;
}

	delete myroot;
}

TEST_CASE( "Write/Read Root in Block","[root]" ) {

/*	SECTION("Write Root in Block"){
	MyFile * firstFile = new MyFile("firstFile.txt",2000,1777,12000,10005,1000000,100011000,11111000,50000);
	MyFile * secondFile = new MyFile("secondFile.txt",1999,1666,10300,12005,1000001,110011011,11000111,59999);
	MyFile * thirdFile = new MyFile("thirdFile.txt",1888,1555,10045,12300,1000011,111110111,10001111,60000);

	//MyRoot * tryRoot = new MyRoot(firstFile);
	MyRoot * tryRoot = new MyRoot(*firstFile);
	tryRoot->addFile("secondFile.txt",10300,12005,1000001,59999);
	tryRoot->addFile("secondFile.txt",10045,12300,1000011,60000);

	BlockDevice blocks;
	blocks.create("containerRootTest.bin");
	//Anwenden auf Root
	char ** writeBlockChar = tryRoot->writeBlocks(blocks);

	//Auslesen der BLoecke
	char ** readBlockChar = new char * [tryRoot->getSize()];
	readBlockChar=tryRoot->readBlocks(blocks);
	REQUIRE(strcmp(*writeBlockChar,*readBlockChar)==0);

	//Final Test
	MyRoot * newRoot = new MyRoot(readBlockChar);
	REQUIRE(newRoot->compareRoots(tryRoot));

//	remove("containerFileTest.bin");
	remove("containerRootTest.bin");
	delete tryRoot;
	delete newRoot;
	//	delete blocks;

}
*/
	/*
SECTION("Klein Test fuer das Block Beschreiben")
{
	MyFile * firstFile = new MyFile("firstFile.txt",2000,1777,12000,10005,1000000,100011000,11111000,50000);
	MyFile * secondFile = new MyFile("secondFile.txt",1999,1666,10300,12005,1000001,110011011,11000111,59999);
	MyFile * thirdFile = new MyFile("thirdFile.txt",1888,1555,10045,12300,1000011,111110111,10001111,60000);

	//MyRoot * tryRoot = new MyRoot(firstFile);
	MyRoot * tryRoot = new MyRoot(*firstFile);
	tryRoot->addFile("secondFile.txt",10300,12005,1000001,59999);
	tryRoot->addFile("secondFile.txt",10045,12300,1000011,60000);

	//kann man das so implementieren??

	BlockDevice blocks;
	blocks.create("RootTest.bin");
	char ** readBlockChar = new char * [tryRoot->getSize()];
	readBlockChar=tryRoot->readBlocks(blocks);
	MyRoot * newRoot = new MyRoot(readBlockChar);
	REQUIRE(newRoot->compareRoots(tryRoot));

	remove("RootTest.bin");
	delete tryRoot;
	delete newRoot;


}*/

	SECTION(" Test fuer das Block Beschreiben")
	{
		printf("START  TESTS IN ROOT \n");
		MyFile * firstFile = new MyFile("firstFile.txt",2000,1777,12000,10005,1000000,100011000,11111000,50000);
		//MyFile * secondFile = new MyFile("secondFile.txt",1999,1666,10300,12005,1000001,110011011,11000111,59999);
		//MyFile * thirdFile = new MyFile("thirdFile.txt",1888,1555,10045,12300,1000011,111110111,10001111,60000);

		MyRoot * tryRoot = new MyRoot(*firstFile);
		tryRoot->addFile("secondFile.txt",10300,12005,1000001,59999);
		tryRoot->addFile("thirdFile.txt",10045,12300,1000011,60000);


		BlockDevice blocks;
		blocks.create("RootTest2.bin");

		tryRoot->writeBlockDevice(&blocks, 0);

		MyRoot * newRoot = new MyRoot(&blocks, 0);
		remove("RootTest2.bin");
		REQUIRE(*newRoot==*tryRoot);



		delete tryRoot;
		delete newRoot;


	}

	SECTION(" Test fuer das Block Beschreiben with read")
		{
			printf("START  TESTS IN ROOT \n");
			MyFile * firstFile = new MyFile("firstFile.txt",2000,1777,12000,10005,1000000,100011000,11111000,50000);
			//MyFile * secondFile = new MyFile("secondFile.txt",1999,1666,10300,12005,1000001,110011011,11000111,59999);
			//MyFile * thirdFile = new MyFile("thirdFile.txt",1888,1555,10045,12300,1000011,111110111,10001111,60000);

			MyRoot * tryRoot = new MyRoot(*firstFile);
			tryRoot->addFile("secondFile.txt",10300,12005,1000001,59999);
			tryRoot->addFile("thirdFile.txt",10045,12300,1000011,60000);


			BlockDevice blocks;
			blocks.create("RootTest2.bin");

			tryRoot->writeBlockDevice(&blocks, ROOT_START);

			MyRoot * newRoot = new MyRoot();
			newRoot->read( ROOT_START, &blocks);
			remove("RootTest2.bin");
			REQUIRE(*newRoot==*tryRoot);



			delete tryRoot;
			delete newRoot;


		}

	SECTION("test operator == and !="){
		MyRoot* r1 = new MyRoot("testdrei.txt",12244,16,50003);
		MyRoot* r2 = new MyRoot("testdrei.txt",12244,16,50003);
		MyRoot* r3 = new MyRoot("testdrei.txt",12244,16,50003);
		r3->addFile("secondFile.txt",10300,12005,1000001,59999);
		MyRoot* r4 = new MyRoot("testdreiiiiiiiii.txt",12344,16,50003);


		REQUIRE(*r1==*r2);
		REQUIRE(*r1!=*r3);
		REQUIRE(*r1!=*r4);
		delete r1;
		delete r2;
		delete r3;
		delete r4;
	}

	SECTION("test root copyFile"){
			MyRoot* r1 = new MyRoot("testdrei.txt",12244,16,50003);
			MyRoot* r2 = new MyRoot("testdrei.txt",12244,16,50003);
			//std::list<MyFile>::iterator it = r1->files.begin();

			//r1->addFile("secondFile.txt",10300,12005,1000001,59999);

			MyFile * f1 = new MyFile(*(r1->getAdr()));
			f1->setSize(1234);

			r1->copyFile("testdrei.txt", f1);
			MyRoot* r3 = new MyRoot(*f1);
			r1->showRoot();
			r2->showRoot();
			r3->showRoot();
			REQUIRE(*r3==*r1);
			REQUIRE(*r2!=*r1);


			delete r1;
			delete r2;
			delete r3;
			delete f1;
		}

}
