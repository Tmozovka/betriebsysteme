//
//  myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 02.08.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

// For documentation of FUSE methods see https://libfuse.github.io/doxygen/structfuse__operations.html

#undef DEBUG

// TODO: Comment this to reduce debug messages
#define DEBUG
#define DEBUG_METHODS
#define DEBUG_RETURN_VALUES

#include "macros.h"
#include "myfs.h"
#include "myfs-info.h"
#include "fat.h"
#include "superBlock.h"
#include "myfs.h"


// fragen zur Folie 31
//TODO Wie kann ich die anzahl geoefnete Dateien abrufen?
// wann soll ich die Bloecke schliessen? Nach dem oeffnen oder nach dem Lesen auch
// wie kann ich pruefen was in gepuffertem Block liegt?
//in folie steht read (n, p), aber wir haben nur read()
using namespace std;

MyFS* MyFS::_instance = NULL;

MyFS* MyFS::Instance() {
    if(_instance == NULL) {
        _instance = new MyFS();
    }
    return _instance;
}

MyFS::MyFS() {
    this->logFile= stderr;


   // printf("Konstruktor von MyFS ist beendet \n");
    LOG("Konstruktor von MyFS ist beendet \n");
}

MyFS::MyFS(char * nameCont) {
    this->logFile= stderr;

    sp = new SuperBlock();
    dmap = new dMap();
    fat = new MyFAT();
    root = new MyRoot();
    blocks = new BlockDevice();
    blocks->create(nameCont);

   // printf("Konstruktor von MyFS ist beendet \n");
    LOG("Konstruktor von MyFS ist beendet \n");
}

MyFS::~MyFS() {

	delete sp;
	delete dmap;
	delete fat;
	delete root;
	delete blocks;
	printf("Destruktor von MyFS ist beendet \n");
	LOG("Destruktor von MyFS ist beendet \n");
    
}

//TODOs:
void MyFS::resize(char * text,int oldSize, int newSize)
{

	int i=newSize-oldSize;
	text+=oldSize;
	while(i!=0)
	{
		*(text++)=' ';
		i--;
	}
	*(text-1)='\0';
	text-=newSize;

}
//int fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
int MyFS::readFile(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo)
{

	// TODO: Implement this!
	//printf("readFile start \n"); //funktioniert nicht
	LOG("********************************************************************************************** ");
	LOG("readFile start ");
	LOGF("offset: %i, size: %i", offset, size);
		/*if (offset > size) // not possible
			RETURN(-1);*/

		/*if(fuseOpen(path,fileInfo)==-1)
		{
			printf("error in fuseOpen \n");
					RETURN(-EPERM);
		}*/


		char * buffer = new char(BD_BLOCK_SIZE);

		MyFile fcopy;
		if(root->getFile(path, &fcopy)==-1)
		{

				//printf("can't get file from root root.getFile(path, &fcopy) \n");
			LOG("can't get file from root root.getFile(path, &fcopy) \n");
				RETURN(-ENOENT);
		}

		//int blocksNumber = ceil(fcopy.getSize() / BD_BLOCK_SIZE);
		if(fcopy.getSize()%BD_BLOCK_SIZE!=0)
		{
			printf("File's size is false  fcopy.getSize() mod D_BLOCK_SIZE!=0 \n");
			RETURN(-ENOENT);
		}
		int blocksNumber = fcopy.getSize() / BD_BLOCK_SIZE;

		int currentBlock = fcopy.getFirstBlock();
		int count=0;
		while (currentBlock != -1&&blocksNumber!=0)
		{
			if( blocks->read(currentBlock, buffer)==0)
			{
				LOGF("buffer in currentBlock %i is : %s \n",currentBlock,buffer);
				while (*buffer != '\0' )
					{
					*(buf++) = *(buffer++);
					count++;
					}

			}
			else
			{
				printf("error in fuseREAD blocks.read(currentBlock, buffer) \n");
				RETURN(-EPERM);
			}

			if(fat->getNext(currentBlock,&currentBlock)==-1)
			{
				printf("error in fuseREAD fat.getNext(currentBlock,&currentBlock) \n");
			RETURN(-ENOENT);
			}

			blocksNumber--;
		}
		buf-=count;
		LOGF("all buf is  : %s \n",buf);
LOG("readFile success \n");
		RETURN(0);
	//return fuseRead(name, buffer,  size,  offset, fi);
}
// int fuseCreate(const char *, mode_t, struct fuse_file_info *);
int MyFS::addFile(const char * name, mode_t mode, time_t mtime , off_t size, char * text)
{
	LOG("********************************************************************************************** ");
	LOG("addFile start");
	if(size%BD_BLOCK_SIZE!=0)
	{
		LOGF("Die Datei %s wurde falsh hinzugefuegt \n", name);
		RETURN(-1);

	}

	LOGF("add file with name : %s \n", name);

	int blocksNumber = size / BD_BLOCK_SIZE;
	LOGF("blocksNumber : %i \n", blocksNumber);

	int * blocksUse = new int[blocksNumber+1];

	blocksUse[blocksNumber] = 0;
	if (dmap->getFreeBlocks(blocksNumber, &blocksUse) == 0)
	{
		if(root->addFile(name, size, mode,mtime,blocksUse[0])==-1)
		{
			printf("error in addFile in root->addFile(name, size, mode,st_mtime,blocks[0] \n");
			return -1;
		}

		for (int i = 0; i < blocksNumber; i++)
		{
			dmap->setUsed(blocksUse[i]);

			if(i+1!=blocksNumber)
			{
					if(fat->link(blocksUse[i], &blocksUse[i+1])==-1)
				{
				RETURN(-1);
				printf("error in addFile in fat.link(blocks[i], &blocks[i+1] \n");
				}
			}

		    if( this->blocks->write(blocksUse[i], text)==-1)
		    {
		    	printf("error in addFile in this->blocks.write(i, \"try\") \n");
		    }
		    text+=BD_BLOCK_SIZE;
		}

	}
	else
	{
		printf("error in addFile no free blocks in dmap \n");
		RETURN(-EPERM);
		//no more place
	}
	delete []  blocksUse;
	LOG("addFile succes");
	//printf("addFile succes \n");
	RETURN(0);
}

//int fuseUnlink(const char *path);
int MyFS::deleteFile(const char *name)
{

    // TODO: Implement this!
	// Tanja's comments:
	//How to unlink file?
	//1 delete file from root
	//2 set bloks unused (change FAT, this changes are executed in Dmap.setUnused funktion)

	//TODO MODE prueffen
	MyFile fcopy;
	if(root->getFile(name, &fcopy)==-1||
	root->deleteFile(name)==-1)
	{
	printf("error in deleteFeil in root.getFile(name, &fcopy)==-1||root.deleteFile(name)==-1");
			RETURN(-ENOENT);
	}

	int blocksNumber = fcopy.getSize() / BD_BLOCK_SIZE;
	int currentBlock = fcopy.getFirstBlock();
	//////////////////////////////////////
	char * text = new char(fcopy.getSize());
	for(int i=0;i<fcopy.getSize();i++)
		*(text++)=' ';
	text-=fcopy.getSize();
	//////////////////////////////////////
	while (blocksNumber!=0&&currentBlock!=-1)
	{
		 if( this->blocks->write(currentBlock, text)==-1)
				    {
				    	printf("error in addFile in this->blocks.write(i,    ) \n");
				    }

		if(dmap->setUnused(currentBlock)==-1)
		{
			printf("error in deleteFile in dmap.setUnused(currentBlock)");
			RETURN(-EPERM);
		}

		int blockBefore=currentBlock;
		if(fat->getNext(currentBlock, &currentBlock)==-1)
		{
			printf("error in deleteFeil in fat.getNext(currentBlock, &currentBlock");
			RETURN(-EPERM);
		}

		if(fat->unLink(blockBefore)==-1)
		{
			printf("error in deleteFeil in fat.unLink(blockBefore)");
			RETURN(-EPERM);
		}


		blocksNumber--;
	}

 RETURN(0);
}

int MyFS::fuseGetattr(const char *path, struct stat *st) {
    LOGM();
    //TODO
	//printf("[getattr] Called\n");
	//printf("\tAttributes of %s requested\n", path);

    //Ist es hier genau so wie in Aufgabestellung?
    //Dateinamen werden in path mit „/“ am Anfang übergeben – ggf. beachten
    //beim Durchsuchen des Verzeichnisses!

    LOGF("Requested path = %s ",path);


	MyFile fcopy;
	LOG("1");


	if(strcmp(path,"/")){
		//Path ist ungleich "/"
		LOG("can't get file from root. File's should start with /");
		RETURN(-ENOENT);
	}
	LOG("2");


	if(strcmp(path, "/") != 0)
	if(root->getFile(path+2,&fcopy)==-1)
		{

		LOGF("Cant find a file with path: %s",path);
		RETURN(-ENOENT);
		}

	LOG("Now starting to set attributes");
	st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	st->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
	//st->st_mtime = fcopy.getLastMod(); ??? Should be here the current time?
	st->st_mtime = time(NULL); // The last "m"odification of the file/directory is right now
	st->st_ctime = time(NULL);

	if (strcmp(path, "/") == 0)
	{
		LOG("Path == /");

		st->st_mode = S_IFDIR | 0555;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
		//size = sum size all files in dir
	}
	else
	{
		LOG("Anderere Path als \"/\" wurde abgefragt");
		st->st_mode = S_IFREG | 0444;
		st->st_nlink = 1;
		st->st_size = fcopy.getSize();

	}

	LOG("fuseGetattr erfolgreich beendet");
    RETURN(0);

}

int MyFS::fuseMknod(const char *path, mode_t mode, dev_t dev) { //??? wir brauchen das nicht
    LOGM();
    
    // TODO: Implement this!

	// How to create Inode?
	// add new file to the Root
	// Information about the file is saved in the objekt File 
	// dev_t dev muss ID von unsere file system sein

    int * blocks[1];
    if(dmap->getFreeBlocks(1,blocks)==-1)
    {
    	LOG("can't add file in root dmap is full. Error in dmap.getFreeBlocks(1,blocks)");
    	RETURN(-1);
    }

	if(root->addFile(path, 512,mode,time(NULL), *blocks[0]))
		{
		LOG("can't add file in root. Error in root.addFile(path, 512, S_IFREG | 0444)");
		RETURN(-EPERM);
		}
//dmap? fat?

	RETURN(0);
}

int MyFS::fuseUnlink(const char *path) {
    LOGM();
    
    RETURN( deleteFile(path));

}

int MyFS::fuseOpen(const char *path, struct fuse_file_info *fileInfo) { // How to open file from hier?
    LOGM();

    //TODO funktioniert falsh
    LOGF("Requested path in Fuse Open = %s ",path);

   if(sp->getOpen()>NUM_OPEN_FILES)
    {
    	LOG("too many files are opened");
    	RETURN(-EPERM);
    }
//sp
   sp->addOpen();
    LOG("1");
    if(root->existName(path)==0)
    {
    	fileInfo->fh=1;
    }
    LOG("2");
    //TOdo etwas tun, wenn path existiert
    //vermerken, dass datei geöffnet
    //Julia: als was merken? als int, ähnlich zu filedescriptor in liunx?
    //falls zu viele datein schon geöffnet-> fehler

//fuseInit()
	/*if(blocks->open(path)==-1) //wenn ich das mache, kann ich danach nich read machen
		{
		fileInfo->fh=1;
		printf("error in fuseOpen in blocks.open(path)");
		RETURN(-EPERM);

		}*/

	RETURN(0);
}

int MyFS::fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this!

	RETURN(readFile(path,buf,size,offset,fileInfo));

}
/*int MyFS::fuseWrite
- mit fuseWrite kann man den Inhalt einer Datei veraendern
- mit size kann ich festlegen, wie viel ich veraendern moechte, mit offset, wo in der Datei ich
etwas aendern moechte und buf ist mein Inhalt, den ich in die schon vorhandene Datei einfuegen
moechte*/
int MyFS::fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    //TODO MODE prueffen
    // TODO: Implement this!


   	char * buffer;


   	MyFile *flink;
   	if( root->getFile( path, flink)==-1)
   	{
   	   		printf("error in fuseWrite in root.getFile( path, &fcopy)");
   	   				RETURN(-EPERM);
   	}

   	if (offset > flink->getSize()) // not possible
   	   		RETURN(-1);

   	if(fuseRead(path,buffer,flink->getSize(),0,fileInfo)==-1)
   	   	{
   	   		printf("error in fuseRead");
   	   				RETURN(-EPERM);
   	   	}

   	bool big =flink->getSize()-offset<size;
   	//1) change size and other config
   	size_t newSize=(big?offset+size:flink->getSize());
   	flink->setSize(newSize);
   	flink->setLastMod(time(NULL));
   	flink->setLastAccess(time(NULL));

   	//write in blocks
   		int count=offset;
   		while(count!=0)
   		{
   			buffer++;
   			count--;
   		}

   		while(*buf!='\n')
   				{
   					*(buffer++)=*(buf++);
   				}


   		//rewrite buffer in the same file in blocks


   		int blocksNumber = ceil(newSize / BD_BLOCK_SIZE);
   		int currentBlock = flink->getFirstBlock();

   			while (currentBlock != -1&&blocksNumber!=0&&*buffer!='\n')
   			{
   				if( blocks->write(currentBlock, buffer)==0)
   				{
   				// how much buffer could i write in block?
   				int c=512;
   				while(c!=0||*buffer!='\n')
   				{
   					buffer++;
   					c--;
   				}

   				}
   				else
   				{
   					printf("error in fuseWRITE blocks.write(currentBlock, buffer) ");
   					RETURN(-EPERM);
   				}

   				if(fat->getNext(currentBlock,&currentBlock)==-1)
   				{
   					printf("error in fuseWrite fat.getNext(currentBlock,&currentBlock)");
   				RETURN(-ENOENT);
   				}

   				blocksNumber--;

   	}

    
    RETURN(0);
}

int MyFS::fuseRelease(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this!
    //temporeres Zeug loeschen
    fileInfo->fh=NULL;
    sp->closeOpen();
    //sonst noch was?
    
    RETURN(0);
}

int MyFS::fuseOpendir(const char *path, struct fuse_file_info *fileInfo) { // Is it not the same as fuseReaddir?
    LOGM();
    
    LOGF("Requested path in Fuse Open dir = %s ",path);

    int templog = (strcmp(path,"/"));

    LOGF("strcmp(path, \"/\") returns: %d", templog);

    // TODO: Implement this!
    if (strcmp(path, "/") == 0) // Root oeffnen
    {
    	printf("Opening root directory");
    	fileInfo->fh=1; //Julia: gibt es etwas sinnvolleres als sich "1" zu merken?
    	//return 0;
    }

  /*  if (strcmp(path, "/") != 0){ // es existieren keine anderen Directories, daher fehler
    	printf("This directory doesnt exist, try opening the root directory");
    	return -ENOENT ;//Datei oder Verzeichnis existiert nicht
    }*/



    RETURN(0);
}

int MyFS::fuseReaddir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this!

   /* if(fileInfo->fh == NULL){ //fh durch fuseopendir gesetzt-> bestaetigt existenz
    	return -ENOENT ;//Datei oder Verzeichnis existiert nicht
    }*/

	printf("--> Getting The List of Files of %s\n", path);


	// filler(void *buf, const char *name,const struct stat *stbuf, off_t off);
	// struct stat <- can contain the file type

	filler(buffer, ".", NULL, 0); // Current Directory
	filler(buffer, "..", NULL, 0); // Parent Directory



	string * listNames;
	offset = 0;

	listNames = root->getArray();

		for(int unsigned i=0; i<(listNames->length()); i++){
		//convert from string to char. Do we need string?
			char *name = new char[listNames[i].length() + 1];
			strcpy(name, listNames[i].c_str());
		//////////////////////////////////////////////////////////////////////

			filler(buffer, name , NULL, 0);
			delete [] name;

		}

    RETURN(0);
    LOG("readDir success");
    // <<< My new code
}

int MyFS::fuseReleasedir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this!
    //temporeres Zeug loeschen

    fileInfo->fh= NULL; // Julia: Sonst noch was loeschen?

    RETURN(0);
}

int MyFS::fuseCreate(const char *path, mode_t mode, struct fuse_file_info *fileInfo) { // Was wird es hier gemacht? Sien hier fat dmap superblock und root erstellt?
    LOGM();
    
    // TODO: Implement this!
   //add empty File
    char * c;
    RETURN(addFile(path,mode,time(NULL),0, c));

}

void* MyFS::fuseInit(struct fuse_conn_info *conn) {
    // Open logfile
    this->logFile= fopen(((MyFsInfo *) fuse_get_context()->private_data)->logFile, "w+");
    if(this->logFile == NULL) {
        fprintf(stderr, "ERROR: Cannot open logfile %s\n", ((MyFsInfo *) fuse_get_context()->private_data)->logFile);
    } else {
        //    this->logFile= ((MyFsInfo *) fuse_get_context()->private_data)->logFile;
        
        // turn of logfile buffering
        setvbuf(this->logFile, NULL, _IOLBF, 0);
        
        LOG("Starting logging...\n");
        LOGM();
        
        // you can get the containfer file name here:
        LOGF("Container file name: %s", ((MyFsInfo *) fuse_get_context()->private_data)->contFile);
        
        // TODO: Implement your initialization methods here!
        // Konstruktor hier (vermuetlich) schreiben . Es wird schon in mkfs.myfs gemacht
        //Wieso muessen wir Konstruktor schreiben? Er wird automatisch aufgerufen
        // Falls wir das von Terminal aufrufen dann? was passiert dann? Muessen wir so was schreiben MyFs mf = new MyFs()?;
        //MyFS();
        char * nameCont = ((MyFsInfo *) fuse_get_context()->private_data)->contFile;
        LOG("try blocks->open(nameCont) \n");
        BlockDevice * blocks = new BlockDevice();
        blocks->open(nameCont);
        LOG("sucess blocks->open(nameCont) \n");
    }
    
    RETURN(0);
}


//////////////////////////Todos end////////////////////////////////////////////////////////////////////

int MyFS::fuseMkdir(const char *path, mode_t mode) {
    LOGM();
    return 0;
}

int MyFS::fuseRmdir(const char *path) {
    LOGM();
    return 0;
}

int MyFS::fuseSymlink(const char *path, const char *link) {
    LOGM();
    return 0;
}

int MyFS::fuseRename(const char *path, const char *newpath) {
    LOGM();
    return 0;
}

int MyFS::fuseLink(const char *path, const char *newpath) {
    LOGM();
    return 0;
}

int MyFS::fuseChmod(const char *path, mode_t mode) {
    LOGM();
    return 0;
}

int MyFS::fuseChown(const char *path, uid_t uid, gid_t gid) {
    LOGM();
    return 0;
}
int MyFS::fuseReadlink(const char *path, char *link, size_t size) {
    LOGM();
    return 0;
}
int MyFS::fuseTruncate(const char *path, off_t newSize) {
    LOGM();
    return 0;
}

int MyFS::fuseUtime(const char *path, struct utimbuf *ubuf) {
    LOGM();
    return 0;
}

int MyFS::fuseStatfs(const char *path, struct statvfs *statInfo) {
    LOGM();
    return 0;
}

int MyFS::fuseFlush(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    return 0;
}

int MyFS::fuseFsync(const char *path, int datasync, struct fuse_file_info *fi) {
    LOGM();
    return 0;
}

int MyFS::fuseListxattr(const char *path, char *list, size_t size) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseRemovexattr(const char *path, const char *name) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseFsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseTruncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    RETURN(0);
}

void MyFS::fuseDestroy() {
    LOGM();
}


#ifdef __APPLE__
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x) {
#else
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
#endif
    LOGM();
    RETURN(0);
}

#ifdef __APPLE__
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x) {
#else
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size) {
#endif
    LOGM();
    RETURN(0);
}



