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
	//We can try to test first hier
	printf("start \n");
	//off_t try1=4;
	//printf("ceil(1200/512): %i",(int)ceil((double)try1/512));
	argc=4;
	argv[1]="container.bin";
	argv[2]="text1.txt";
	argv[3]="text2.txt";

	char * nameCont = argv [1];
	LOGF("container: %s \n",nameCont);

	MyFS * fs = new MyFS(nameCont);



//write files
	for(int i=2;i<argc;i++)
	{

		FILE *fin;
		LOGF("open File: %s \n", argv[i]);
		fin = fopen(argv[i], "rwb");
		if(fin)
		{
			LOGF("successful open File: %s \n", argv[i]);
			LOGF("successful open File: %s \n", argv[i]);
		struct stat st;
		stat(argv[i], &st);
		off_t size=ceil((double)st.st_size/BD_BLOCK_SIZE)*BD_BLOCK_SIZE;
		char * puffer;
		puffer = new char(size);
		fread(puffer, size, 1, fin);
		LOGF("File: %s ,size: %i, puffer: %s  \n",argv[i],size, puffer );
		fs->addFile(argv[i],st.st_mode,st.st_mtime,size,puffer);
		}
		else{
			//printf("can't open File: %s \n", argv[i]);
			LOGF("can't open File: %s \n", argv[i]);
		}
//	wieso funktioniert es nicht
//		fclose(fin);
	}
		 LOG("all files are in container.bin \n");
		 fs->root->showRoot();
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
			char * puffer;
			puffer = new char(size);
	fs->readFile(argv[i], puffer,size,0,new fuse_file_info);
		LOGF("read File %s : %s \n",argv[i],puffer);
		printf("read File %s : %s \n",argv[i],puffer);
			}
					//
			//fclose(fin);
				}
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
