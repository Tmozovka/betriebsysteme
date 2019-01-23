//
//  mk.myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#include "myfs.h"
#include "blockdevice.h"
#include "macros.h"
#include <stdio.h>
#include <fstream>
#include <math.h>

int main(int argc, char *argv[]) {

	// TODO: Implement file system generation & copying of files here

	printf("start \n");
	printf("argc: %i \n", argc);

	/*argc=4;
	 argv[1]="container.bin";
	 argv[2]="text1.txt";
	 argv[3]="text2.txt";
	 argv[4]="text1 (copy).txt";*/

	char * nameCont = argv[1];
	LOGF("container: %s \n",nameCont);

	MyFS * fs = new MyFS();
	fs->blocks->create(nameCont);

	char * pufferAdd;
	char * pufferRead;

	int count = 0;
//START:
//write files
	for (int i = 2; i < argc; i++) {

		FILE *fin;
		LOGF("open File: %s \n", argv[i]);

		fin = fopen(argv[i], "rwb");
		if (fin) {
			LOGF("successful open File: %s \n", argv[i]);LOGF("successful open File: %s \n", argv[i]);

			struct stat st;
			st.st_mode = S_IFREG | 0444;
			stat(argv[i], &st);

			printf("first name %s \n", argv[i]);
			//////////////////////NAME AUSLESEN UND BEARBEITEN//////////////////////////////////////////////
			if ((*(argv[i]) == '.') && (*(argv[i] + 1) == '/')) {
				argv[i] += 2;
			}

			int countArg = 0;
			int schiebCount = 0;
			while (*(argv[i]) != char(0)) {
				//printf("*argv[i] : %c  \n", *argv[i]);
				if (*argv[i] == '/') {
					schiebCount++;
				}

				argv[i]++;
				countArg++;

			}
			argv[i] -= countArg;
			countArg = 0;
			printf("schiebCount : %i \n", schiebCount);
			if (schiebCount != 0) {
				for (int j = 0; j < schiebCount; j++) {

					while (*argv[i] != '/') {
						argv[i]++;
						//countArg++;
					}
					argv[i]++;
				}
			}

			argv[i] -= countArg;
			countArg = 0;

			/////////////////////////////////////////////////////////////////////////////////
			printf("name zum bearbeiten %s \n", argv[i]);

			off_t size = ceil((double) st.st_size / BD_BLOCK_SIZE) //ceil(2/512)=1  ceil(513/512)=2
			* BD_BLOCK_SIZE;
			printf("size von der Datei: %i st.st_size : %i \n", size, st.st_size );
			pufferAdd = new char[size];
			fread(pufferAdd, size, 1, fin);
			pufferAdd[st.st_size]=char(10);
			//printf("die Inhalt von %s : %s \n", argv[i],pufferAdd);
			fs->resize(pufferAdd, st.st_size+1, size);
			//LOGF("File: %s ,size: %i, puffer: %s  \n",argv[i],size, pufferAdd );

			/*char * name = new char [255];
			 name[0]='/';
			 int j=1;
			 while(argv[i][j-1]!='\0')
			 {
			 name[j]=argv[i][j-1];
			 j++;
			 }
			 name[j]='\0';

			 fs->addFile(name, st.st_mode, st.st_mtime, size, pufferAdd);*/
			fs->addFile(argv[i], st.st_mode, st.st_mtime, size, pufferAdd);

		} else {
			//printf("can't open File: %s \n", argv[i]);
			LOGF("can't open File: %s \n", argv[i]);
		}

		delete[] pufferAdd;
//	wieso funktioniert es nicht
//		fclose(fin);
	}

	LOG("all files are in container.bin \n");
	fs->root->showRoot();

	string * ar;
	ar = new string[fs->root->getSize()];
	ar = fs->root->getArray();
	printf("FIRST NAME: %s", ar[0].c_str());

	//fs->fat->showFat();
	fs->writeBlockDevice();

	//////////////////////// Hinzufügen der Datenstrukturen//////////////////

	/*int nextBlockToWrite = 5; //TODO: Sinnvollen Wert nehmen
	 //Julia: Ich weiß noch nicht welcher block der richtige ist
	 // Deshalb teste ich es mit 5

	 //Superblock hinzufügen
	 nextBlockToWrite = fs->sp->init(nextBlockToWrite, fs->blocks);
	 if(nextBlockToWrite==-1){
	 LOG("Error occured while trying to add Superblock to Blockdevice");
	 return -1;
	 }

	 //Dmap hinzufügen
	 nextBlockToWrite = fs->dmap->init(nextBlockToWrite, fs->blocks);
	 if(nextBlockToWrite==-1){
	 LOG("Error occured while trying to add dmap to Blockdevice");
	 return -1;
	 }*/

	/*	 fs->fat->showFat();
	 fs->dmap->showDmap();

	 //read files
	 for(int i=2;i<argc;i++)
	 {

	 FILE *fin;
	 fin = fopen(argv[i], "rwb");
	 if(fin)
	 {
	 struct stat st;
	 stat(argv[i], &st);
	 off_t size=ceil((double)st.st_size/BD_BLOCK_SIZE)*BD_BLOCK_SIZE;
	 //off_t size=st.st_size;

	 pufferRead = new char(size);
	 fs->readFile(argv[i], pufferRead,size,0,new fuse_file_info);
	 LOGF("read File %s : %s \n",argv[i],pufferRead);
	 printf("read File %s : %s \n",argv[i],pufferRead);

	 }
	 //
	 //fclose(fin);
	 }

	 //delete Files
	 printf("deleteFile: %s \n", argv[3]);
	 fs->deleteFile(argv[3]);

	 fs->root->showRoot();
	 fs->fat->showFat();
	 fs->dmap->showDmap();

	 printf("deleteFile: %s \n", argv[2]);
	 fs->deleteFile(argv[2]);

	 fs->root->showRoot();
	 fs->fat->showFat();
	 fs->dmap->showDmap();

	 count++;

	 if(count!=2)
	 {
	 argv[3]="text1.txt";
	 argv[2]="text2.txt";
	 goto START;

	 }
	 */
//////////////////////////////////////////////////////////////////////////////////////////
	//printf("%i", fs);
	/*char * buf;
	 fs->addFile("file 1",S_IFDIR | 0444,7,"file 1");
	 fuse_fill_dir_t filler;
	 if(fs->fuseReaddir("/",buf,filler, 0, new fuse_file_info)==-1)
	 {
	 printf("error in main in fuseReaddir \n");
	 return-1;
	 }

	 printf(buf);

	 fs->deleteFile("file 1");


	 fs->addFile("file 2",S_IFDIR | 0444,7,"file 1");
	 fs->readFile("file 2", buf,7,0,new fuse_file_info);
	 printf(buf);
	 //fuseReaddir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo)
	 if(fs->fuseReaddir("/",buf,filler, 0, new fuse_file_info)==-1)
	 {
	 printf("error in main in fuseReaddir \n");
	 return-1;
	 }

	 printf(buf);
	 fs->deleteFile("file 2");
	 //no mistakes? It's done!*/
	return 0;
}
