//
//  myfs.h
//  myfs
//
//  Created by Oliver Waldhorst on 02.08.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#ifndef myfs_h
#define myfs_h

#include <fuse.h>
#include <cmath>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>



#include "blockdevice.h"
#include "myfs-structs.h"

#include "superBlock.h"
#include "dMap.h"
#include "fat.h"
#include "root.h"
//#include "myFile.h"


class MyFS {
private:
    static MyFS *_instance;
    FILE *logFile;
    
public:
    	static MyFS *Instance();

		static SuperBlock sp;
		static dMap dmap;
		static MyFAT fat;
		static MyRoot root;
		static BlockDevice blocks;
		
    
    MyFS();
    ~MyFS();

	
    // --- Methods called by FUSE ---
    // For Documentation see https://libfuse.github.io/doxygen/structfuse__operations.html
    int fuseGetattr(const char *path, struct stat *statbuf); //implement
    int fuseReadlink(const char *path, char *link, size_t size);
    int fuseMknod(const char *path, mode_t mode, dev_t dev); //implement
    int fuseMkdir(const char *path, mode_t mode);
    int fuseUnlink(const char *path);//implement
    int fuseRmdir(const char *path);
    int fuseSymlink(const char *path, const char *link);
    int fuseRename(const char *path, const char *newpath);
    int fuseLink(const char *path, const char *newpath);
    int fuseChmod(const char *path, mode_t mode);
    int fuseChown(const char *path, uid_t uid, gid_t gid);
    int fuseTruncate(const char *path, off_t newSize);
    int fuseUtime(const char *path, struct utimbuf *ubuf);
    int fuseOpen(const char *path, struct fuse_file_info *fileInfo);//implement ??
    int fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo); //implement ??
    int fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo); //implement
    int fuseStatfs(const char *path, struct statvfs *statInfo);
    int fuseFlush(const char *path, struct fuse_file_info *fileInfo);
    int fuseRelease(const char *path, struct fuse_file_info *fileInfo);//implement
    int fuseFsync(const char *path, int datasync, struct fuse_file_info *fi);
#ifdef __APPLE__
    int fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x);
    int fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x);
#else
    int fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags);
    int fuseGetxattr(const char *path, const char *name, char *value, size_t size);
#endif
    void* fuseInit(struct fuse_conn_info *conn); //implement
    int fuseListxattr(const char *path, char *list, size_t size);
    int fuseRemovexattr(const char *path, const char *name);
    int fuseOpendir(const char *path, struct fuse_file_info *fileInfo); //implement
    int fuseReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo); //implement
    int fuseReleasedir(const char *path, struct fuse_file_info *fileInfo); //implement
    int fuseFsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo);
    int fuseTruncate(const char *path, off_t offset, struct fuse_file_info *fileInfo);
    int fuseCreate(const char *, mode_t, struct fuse_file_info *); //implement
    void fuseDestroy();
    
    // TODO: Add methods of your file system here
		
		// int fuseCreate(const char *, mode_t, struct fuse_file_info *);
    	int addFile(const char * name, mode_t mode, off_t size, char * text);
		
		//int fuseUnlink(const char *path);
		int deleteFile(const char *path);
		
		//int fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
		int readFile(const char *name, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi);
};

#endif /* myfs_h */
