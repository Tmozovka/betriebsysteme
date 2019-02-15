//
//  myfs-structs.h
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#ifndef myfs_structs_h
#define myfs_structs_h

#define NAME_LENGTH 255
#define BLOCK_SIZE 512
#define NUM_DIR_ENTRIES 64
#define NUM_OPEN_FILES 64

#define SP_START 0
#define SP_SIZE 1
#define DMAP_START 1
#define DMAP_SIZE 118
#define FAT_START 119
#define FAT_SIZE 704
#define ROOT_START 823
#define ROOT_SIZE 65
#define BLOCKS_START 888

//for MyFile

#define FILE_NAME_SIZE 255
#define FILE_USER_SIZE 10
#define FILE_GROUP_SIZE 10
#define FILE_SIZE_SIZE 10
#define FILE_MODE_SIZE 10
#define FILE_ACCESS_SIZE 15
#define FILE_MOD_SIZE 15
#define FILE_STATUS_SIZE 15
#define FILE_BLOCK_SIZE 7
#define MAX_FILE_SIZE FILE_NAME_SIZE+FILE_USER_SIZE+FILE_GROUP_SIZE+FILE_SIZE_SIZE+ FILE_MODE_SIZE+FILE_ACCESS_SIZE+ FILE_MOD_SIZE +FILE_STATUS_SIZE+FILE_BLOCK_SIZE



// TODO: Add structures of your file system here


#endif /* myfs_structs_h */
