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

		 SECTION("addFile") {

			 printf("start \n");
			 char *argv[5];
			 int argc;
			 	argc=4;
			 	argv[1]="container.bin";
			 	argv[2]="text1.txt";
			 	argv[3]="text2.txt";
			 	argv[4]="text1 (copy).txt";

			 	char * nameCont = argv [1];
			 	//LOGF("container: %s \n",nameCont);
printf("1 \n");
			 	MyFS * fs = new MyFS(nameCont);
			 	printf("MyFS ist erstellt \n");
			 	char * pufferAdd;
			 	char * pufferRead;

			 	int count=0;

			 	int sizeRoot=0;
			 START:
			 //write files
			 	for(int i=2;i<argc;i++)
			 	{
			 		printf("try to open %s \n", argv[i]);
			 		FILE *fin;
			 		fin = fopen(argv[i], "rwb");
			 		if(fin)
			 		{
			 			printf("OPENED! \n");
			 		struct stat st;
			 		st.st_mode = S_IFREG | 0444;
			 		stat(argv[i], &st);
			 		off_t size=ceil((double)st.st_size/BD_BLOCK_SIZE)*BD_BLOCK_SIZE;
			 		pufferAdd = new char(size);
			 		fread(pufferAdd, size, 1, fin);
			 		printf("try to resize \n");
			 		fs->resize(pufferAdd,st.st_size,size);
			 		printf("REseised! \n");
			 		printf("try to add File! \n");
			 		fs->addFile(argv[i],st.st_mode,st.st_mtime,size,pufferAdd);
			 		printf("ADDED! \n");
			 		sizeRoot++;

			 		}
			 	}

			 	printf("TRY TO COMPARE   REQUIRE( sizeRoot == fs->root->getSize());! \n");
			 REQUIRE( sizeRoot == fs->root->getSize());

			 	string * ar = new string[fs->root->getSize()];
			 	ar = fs->root->getArray();

			 	/*for(int i=0;i<fs->root->getSize();i++)
			 	{
			 		string comp = argv[i+2];
			 		REQUIRE( ar[0].compare(comp));
			 	}*/
		 }

}









