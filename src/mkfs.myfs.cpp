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


int main(int argc, char *argv[]) {

    // TODO: Implement file system generation & copying of files here
	//We can try to test first hier
	/*MyFS * fs = new MyFS();
	printf("%i", fs);
	char * buf;
	fs->addFile("file 1",S_IFDIR | 0444,7,"file 1");

	if(fs->fuseReaddir("/",buf,new int(*)(), 0, new fuse_file_info)==-1)
			{
			printf("error in main in fuseReaddir");
			return-1;
			}

		printf(buf);

	fs.deleteFile("file 1");


	fs.addFile("file 2",S_IFDIR | 0444,7,"file 1");
	fs.readFile("file 2", buf,7,0,new fuse_file_info);
	printf(buf);
	//fuseReaddir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo)
	if(fs.fuseReaddir("/",buf,new int(*)(), 0, new fuse_file_info)==-1)
		{
		printf("error in main in fuseReaddir");
		return-1;
		}

	printf(buf);
	fs.deleteFile("file 2");
    //no mistakes? It's done!*/
    return 0;
}
