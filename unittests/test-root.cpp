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
	char *name = new char {'a'};

	for(int i=0;i<=NUM_DIR_ENTRIES+1;i++) {
		//printf("Test root: Gerade Schleifenzähler bei %d Name lautet: %c\n",i,name[0]);
		MyFile * f = new MyFile(name,NULL,NULL,NULL,NULL,time(NULL),time(NULL),time(NULL),-1);
		myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
		name[0] ++;
	}
	int result = myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
	REQUIRE(result==-1);
	delete f;
	delete name;
}

	SECTION("No addition possible: Name is too big"){
	MyFile * f = new MyFile();
	//f->setSize(NAME_LENGTH+1);

	char * nameBig = new char [NAME_LENGTH+1];

	for(int i=0; i<NAME_LENGTH+1;i++)
	{
		*(nameBig++)='a';
	}

	*nameBig='\0';
	nameBig-=(NAME_LENGTH+1);

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

	SECTION("Write Root in Block"){
	MyFile * firstFile = new MyFile("firstFile.txt",2000,1777,12000,10005,1000000,100011000,11111000,50000);
	MyFile * secondFile = new MyFile("secondFile.txt",1999,1666,10300,12005,1000001,110011011,11000111,59999);
	MyFile * thirdFile = new MyFile("thirdFile.txt",1888,1555,10045,12300,1000011,111110111,10001111,60000);

	//MyRoot * tryRoot = new MyRoot(firstFile);
	MyRoot * tryRoot = new MyRoot(*firstFile);
	tryRoot->addFile("secondFile.txt",10300,12005,1000001,59999);
	tryRoot->addFile("secondFile.txt",10045,12300,1000011,60000);

	//writeBlocks anwenden auf Root
	char ** writeBlockChar = new char * [tryRoot->getSize()];
	writeBlockChar= tryRoot->writeBlocks();

	//char*containerpaths[]= new char [tryRoot->sizeRoot];
	char* containerpaths= new char [tryRoot->getSize()];
	char*path_file;
	char*path_bin;

	for(int i=0; i<=tryRoot->getSize();i++) {
		path_file ="containerFile";
		path_bin=".bin";
		containerpaths[i]= sprintf( path_file, "%s%d%s", path_file, i, path_bin );
	}

	char * buf = new char [512];
	char * readBuf = new char [512];

	BlockDevice blocks;

	string *nameArray = tryRoot->getArray();

	for(u_int32_t k=0;k<=tryRoot->getSize();k++) {
		//tryRoot->getFile(nameArray[k],new MyFile());
		buf = *writeBlockChar;

		//Tanja's Frage: blocks.create kann nur ein mal erstellt werden, so wird ein Binaere Datei erstellt.
		//In der Datei muessen alle MyFiles usw geschrieben werden.
		//Wieso ist es in der Schleife?
		blocks.create(containerpaths[k]);//??????ERROR HIER????????????????

		blocks.write(k,buf);
		blocks.read(k,readBuf);
		REQUIRE(strcmp(buf, readBuf)==0);

		writeBlockChar++;
	}
	//Auslesen der BLoecke
	char ** readBlockChar = new char * [tryRoot->getSize()];
	readBlockChar=tryRoot->readBlocks(blocks);

	REQUIRE(strcmp(*writeBlockChar,*readBlockChar)==0);

	//Final Test
	MyRoot * newRoot = new MyRoot(readBlockChar);

	//TODO: Fertig implementieren
	REQUIRE(newRoot->compareRoots(tryRoot));

	remove("containerFileTest.bin");
	delete tryRoot;
	delete newRoot;
	//	delete blocks;
	delete [] readBuf;
	delete [] buf;

}

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


}
}
