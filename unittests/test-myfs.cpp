//
//  test-myfs.cpp
//  testing
//
//  Created by Oliver Waldhorst on 15.12.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#include "catch.hpp"

#include "helper.hpp"
#include "myfs.h"
#include <string.h>

using namespace std;
/*
// TODO: Implement your helper functions here!
TEST_CASE( "my Funktionen in myfs testen", "[myfs]" ) {
	FILE *fin;

	printf("start \n");
	char *argv[5];
	int argc;
	argc = 4;
	argv[1] = "containerTest.bin";
	argv[2] = "text1.txt";
	argv[3] = "text2.txt";
	argv[4] = "text1 (copy).txt";

	char * nameCont = argv[1];
	//LOGF("container: %s \n",nameCont);
	printf("1 \n");

	char * pufferAdd;
	char * pufferRead;

	int count = 0;

	int sizeRoot = 0;

	SECTION("addFile readFile deleteFile"){

	MyFS * fs = new MyFS();
	fs->blocks->create(nameCont);
	// printf("MyFS ist erstellt \n");
	START:
	//write files
	for(int i=2;i<argc;i++)
	{
		//printf("try to open %s \n", argv[i]);

		fin = fopen(argv[i], "rwb");
		if(fin)
		{
			//printf("OPENED! \n");
			struct stat st;
			st.st_mode = S_IFREG | 0444;
			stat(argv[i], &st);

			//off_t size=ceil((double)st.st_size/BD_BLOCK_SIZE)*BD_BLOCK_SIZE;
			off_t size = st.st_size;
			pufferAdd = new char[size];
			fread(pufferAdd, size, 1, fin);
			//printf("try to resize \n");
			//fs->resize(pufferAdd,st.st_size,size);
			//printf("REseised! \n");
			//printf("try to add File! \n");
			fs->addFile(argv[i],st.st_mode,st.st_mtime,size,pufferAdd);
			//printf("ADDED! \n");
			sizeRoot++;

			delete [] pufferAdd;
		}

	}

	//	printf("TRY TO COMPARE   REQUIRE( sizeRoot == fs->root->getSize());! \n");
	REQUIRE( sizeRoot == fs->root->getSize());

	string * ar = new string[fs->root->getSize()];
	ar = fs->root->getArray();

	for(int i=0;i<fs->root->getSize();i++)
	{
		//printf("compare file %s \n", argv[i+2]);
		string comp = argv[i+2];
		string comp2 = ar[i];
		REQUIRE( comp2.compare(comp)==0);
	}

	//read files

	for(int i=2;i<argc;i++)
	{
		//printf("Try to read file: %s \n", argv[i]);
		FILE *fin;
		fin = fopen(argv[i], "rwb");
		if(fin)
		{
			struct stat st;
			stat(argv[i], &st);
			//off_t size=ceil((double)st.st_size/BD_BLOCK_SIZE)*BD_BLOCK_SIZE;
			off_t size=st.st_size;

			pufferRead = new char[size];
			int t= fs->readFile(argv[i], pufferRead,size,0,new fuse_file_info);
			REQUIRE(t==0);
			//	printf(" REQUIRE(t==0) \n");
			//	printf("read File %s : %s \n",argv[i],pufferRead);

			delete [] pufferRead;
		}
		//
		//fclose(fin);
	}

	//delete Files
	//printf("deleteFile: %s \n", argv[3]);
	char * temp = argv[3];
	int t=fs->deleteFile(temp);
	REQUIRE(t==0);
	REQUIRE(fs->root->getSize()==1);
	ar = new string[fs->root->getSize()];
	ar = fs->root->getArray();

	string comp = argv[2];
	string comp2 = ar[0];
	REQUIRE( comp2.compare(comp)==0);

	//printf("deleteFile: %s \n", argv[2]);
	//REQUIRE(fs->deleteFile(argv[2])==0); //error
	temp= argv[2];
	REQUIRE(fs->deleteFile(temp)==0);//error
	REQUIRE(fs->root->getSize()==0);

	delete [] ar;
	remove("containerTest.bin");
	delete fs;
}

	SECTION("write in BlockDevice"){

	MyFS * fs = new MyFS();
	fs->blocks->create("containerTest2.bin");

	// printf("MyFS ist erstellt \n");

///////////////////////////////////////WRITE IN FS//////////////////////////////////////////
	//write files
	for(int i=2;i<argc;i++)
	{
		//printf("try to open %s \n", argv[i]);

		fin = fopen(argv[i], "rwb");
		if(fin)
		{
			//printf("OPENED! \n");
			struct stat st;
			st.st_mode = S_IFREG | 0444;
			stat(argv[i], &st);

			//off_t size=ceil((double)st.st_size/BD_BLOCK_SIZE)*BD_BLOCK_SIZE;
			off_t size = st.st_size;
			pufferAdd = new char[size];
			fread(pufferAdd, size, 1, fin);
			//printf("try to resize \n");

			//fs->resize(pufferAdd,st.st_size,size);

			//printf("REseised! \n");
			//printf("try to add File! \n");
			fs->addFile(argv[i],st.st_mode,st.st_mtime,size,pufferAdd);
			//printf("ADDED! \n");
			sizeRoot++;
			fs->writeBlockDevice();
			delete [] pufferAdd;
		}
	}


	MyFS * newFs = new MyFS("containerTest2.bin");

	remove("containerTest2.bin");
	bool a=*fs==*newFs;
	REQUIRE(*fs==*newFs);

	delete newFs;
	delete fs;


}

	SECTION("test MKnod")
	{
		MyFS * fs = new MyFS();

		int * blocks[1];
		//LOG("0\n");
		REQUIRE(fs->dmap->getFreeBlocks(1, blocks)==0);
		REQUIRE(fs->root->addFile("text3.txt", 512, 1000, time(NULL), *blocks[0])==0);

		delete fs;
	}

}*/

