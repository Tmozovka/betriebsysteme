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
#define FAT_SIZE 118
#define ROOT_START 237
#define ROOT_SIZE 64
#define BLOCKS_START 301


// TODO: Add structures of your file system here


#endif /* myfs_structs_h */
