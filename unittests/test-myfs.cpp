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
		fclose(fin);

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

			pufferRead = new char[size+1];

			//richtigkeit ausgelesene ueberprueffen
			struct fuse_file_info tmp;
			int t= fs->readFile(argv[i], pufferRead,size,0,&tmp);
			pufferRead[size]=char(0);
			REQUIRE(t==size);
			printf("read File %s : %s \n",argv[i],pufferRead);

			char * pufferRead2 = new char[size+1];
			int t2= fs->readFile(argv[i], pufferRead2,size,0,&tmp);
			pufferRead2[size]=char(0);
			printf("read File 2 %s : %s \n",argv[i],pufferRead2);
			for(int j=0;j<size+1;j++)
			{
				if(pufferRead[j]!=pufferRead2[j])
				{
					printf("auf der Stelle %i ist in pufferRead Zeichen %c und in in pufferRead2 Zeichen %c \n", j, pufferRead[j], pufferRead2[j]);
				}
			}
			REQUIRE(t2==size);

			REQUIRE(strcmp(pufferRead2,pufferRead)==0);

			char * pufferRead3 = new char[size+1];
			fin = fopen(argv[i], "rwb");
			fread(pufferRead3, size, 1, fin);
			pufferRead3[size]=char(0);
			printf("read File 3 with fread %s : %s \n",argv[i],pufferRead3);
			REQUIRE(strcmp(pufferRead2,pufferRead3)==0);

			//delete tmp;
			delete [] pufferRead;
			delete [] pufferRead2;
			delete [] pufferRead3;

			for(int j=0;j<10;j++)
			{
				int offset = rand()%(size-1);
				int readSize=size-offset;
				printf("readSize: %i \n", readSize);

				pufferRead3 = new char[readSize+1];
				fseek(fin, offset, SEEK_SET);
				fread(pufferRead3, readSize, 1, fin);
				pufferRead3[readSize]=char(0);
				printf("read with fssek with offset %i , puffer %s \n end fseek\n", offset, pufferRead3);

				pufferRead2 = new char[readSize+1];
				t2= fs->readFile(argv[i], pufferRead2,readSize,offset,&tmp);
				pufferRead2[readSize]=char(0);
				printf("read with readFile from container with offset %i , puffer %s \n", offset, pufferRead2);

				REQUIRE(t2==readSize);
				REQUIRE(strcmp(pufferRead2,pufferRead3)==0);

				delete [] pufferRead2;
				delete [] pufferRead3;

			}

			fclose(fin);

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
	delete [] ar;
	ar = new string[fs->root->getSize()];
	ar = fs->root->getArray();

	string comp = argv[2];
	string comp2 = ar[0];
	REQUIRE( comp2.compare(comp)==0);

	delete [] ar;
	//printf("deleteFile: %s \n", argv[2]);
	//REQUIRE(fs->deleteFile(argv[2])==0); //error
	temp= argv[2];
	REQUIRE(fs->deleteFile(temp)==0);//error
	REQUIRE(fs->root->getSize()==0);


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

	bool a=*fs==*newFs;
	REQUIRE(*fs==*newFs);

	//SECTION("test fuseWrite") {

	/*	printf("FUSE WRITE TEST START\n");
		int size=1000;
		int offset =0;
		int ch='!';
		char * buf =new char[size];
		for (int j=0;j<size;j++)
			buf[j]=ch;
		//buf[j]=(ch++)%50+34;

		for(int i=2;i<argc;i++)
		{
			char * name = new char[sizeof(argv[i])+3];
			name[0]='/';
			for(int j=0;j<sizeof(argv[i])+2;j++)
			{
				name[j+1]=argv[i][j];
			}
			name[sizeof(argv[i])+2]=char(0);
			printf("NAME FUER FUSE WRITE: %s \n", name);
			newFs->fuseWrite(name, buf, size, offset, NULL);

			int readSize=size+100;
			pufferRead = new char[readSize+1];
			int ret= newFs->readFile(argv[i], pufferRead,readSize,offset,new fuse_file_info);
			pufferRead[readSize]=char(0);
			printf("Datei: %s, changed inhalt: %s \n", argv[i], pufferRead);
			delete [] name;
			delete [] pufferRead;

		}

		printf("FUSE WRITE TEST END\n");
		//struct fuse_file_info *fileInfo
		delete [] buf;
	//}*/

	remove("containerTest2.bin");
	delete newFs;
	delete fs;

}

	SECTION("test MKnod"){
	MyFS * fs = new MyFS();

	int * blocks[1];
	//LOG("0\n");
	REQUIRE(fs->dmap->getFreeBlocks(1, blocks)==0);
	REQUIRE(fs->root->addFile("text3.txt", 512, 1000, time(NULL), *blocks[0])==0);

	delete fs;
}

}

