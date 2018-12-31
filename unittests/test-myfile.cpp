#include "catch.hpp"

#include "helper.hpp"
#include "myfs.h"

#include "MyFile.h"

TEST_CASE( "Compare File// Getter- and Settermethods", "[MyFile]" ) {

	printf("START MYFILE TEST \n");
	MyFile* myfile = new MyFile("myfile.txt", getuid(), getgid(), 0, NULL,
			time(NULL), time(NULL), time(NULL), 2);

	SECTION("Compare File"){
	MyFile* similarFile = new MyFile("myfile.txt",getuid(),getgid(),0,NULL,time(NULL),time(NULL),time(NULL),2);

	REQUIRE(*myfile == *similarFile);
	delete similarFile;

}

	delete myfile;


	SECTION("Write File in Block"){
		MyFile * tryFile = new MyFile("newfile.txt",2000,2000,12345,12345,1111111,111111111,11111111,60000);

		printf("START TO WRITE IN BLOCK \n");
		char * buf = new char [512];
		char * readBuf = new char [512];
		buf = tryFile->writeBlock();

		printf("buf mit tryFile: %s \n", buf);
		printf("write to block \n");

		BlockDevice  blocks ;


		blocks.create("containerFileTest.bin");
		blocks.write(0,buf);
		blocks.read(0,readBuf);

		REQUIRE(strcmp(buf, readBuf)==0);
		MyFile * newFile = new MyFile(readBuf);
		REQUIRE(*newFile == *tryFile);

		remove("containerFileTest.bin");
		delete tryFile;
		delete newFile;
	//	delete blocks;
		delete [] readBuf;
		delete [] buf;

	}
}
