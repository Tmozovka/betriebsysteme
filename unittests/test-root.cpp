
#include "catch.hpp"


#include "helper.hpp"
#include "myfs.h"


#include "root.h"


TEST_CASE( "Add/get/delete File", "[root]" ) {

	MyRoot* myroot = new MyRoot();


	 SECTION("add first File") {
        MyFile * firstfile = new MyFile(); 
        int result = myroot->addFile(firstfile->getName(),firstfile->getSize(),firstfile->getMode(),firstfile->getLastMod(),firstfile->getFirstBlock());
        //string name, off_t size, mode_t mode,time_t st_mtime, int firstBlock
        //off_t size=ceil((double)st.st_size/BD_BLOCK_SIZE)*BD_BLOCK_SIZE;
        //fs->addFile(argv[i],size,st.st_mode,st.st_mtime,pufferAdd);
        MyFile* mf = new MyFile();
        myroot->getFile(firstfile->getName(), mf);
		 REQUIRE( result == 0);
         //REQUIRE( myroot->sizeRoot == 1);
		 REQUIRE( *mf == *firstfile);

	 }
    SECTION("No addition possible: too many files in Root"){
        MyFile * f = new MyFile();
        //myroot->sizeRoot = NUM_DIR_ENTRIES+1;
        //int result oder result?
        int result = myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
        //REQUIRE(result=-1);
        delete f;
    }
       SECTION("No addition possible: Name already exists"){
        MyFile * f = new MyFile();
        //myroot->sizeRoot=0;
        MyFile * fileSimilarName = new MyFile();
        fileSimilarName->setName("similarName.txt");
        fileSimilarName->setName("similarName.txt");
        myroot->addFile(fileSimilarName->getName(),fileSimilarName->getSize(),fileSimilarName->getMode(),fileSimilarName->getLastMod(),fileSimilarName->getFirstBlock());
        //int result oder result?
        int result = myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
        //REQUIRE(result=-1);
        delete f;
    }


}
// Next TEST_CASE: existName
// Next TEST_CASE: showRoot
// Next TEST_CASE: getArray
// Next TEST_CASE: delete File
// Next TEST_CASE: get Array
