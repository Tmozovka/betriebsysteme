
#include "catch.hpp"


#include "helper.hpp"
#include "myfs.h"
#include <string>


#include "root.h"


TEST_CASE( "Add/get/delete File", "[root]" ) {

	MyRoot* myroot = new MyRoot();


	 SECTION("add first File and compare(get) it") {
        MyFile * firstfile = new MyFile(); 
        int result = myroot->addFile(firstfile->getName(),firstfile->getSize(),firstfile->getMode(),firstfile->getLastMod(),firstfile->getFirstBlock());
        MyFile* mf = new MyFile();
        myroot->getFile(firstfile->getName(), mf);
		 REQUIRE( result == 0);
		 REQUIRE( *mf == *firstfile);

	 }
    SECTION("No addition possible: too many files in Root"){
        MyFile * f = new MyFile();
        for(int i=0;i<=NUM_DIR_ENTRIES+1;i++){
        	myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
        }
        int result = myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
        REQUIRE(result==-1);
        delete f;
    }

       SECTION("No addition possible: Name is too big"){
           MyFile * f = new MyFile();
           f->setSize(NAME_LENGTH+1);
           int result =myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
           REQUIRE(result==-1);
           delete f;
    }
       SECTION("No addition possible: Name already exists"){
        MyFile * f = new MyFile();
        MyFile * fileSimilarName = new MyFile();
        fileSimilarName->setName("similarName.txt");
        fileSimilarName->setName("similarName.txt");
        myroot->addFile(fileSimilarName->getName(),fileSimilarName->getSize(),fileSimilarName->getMode(),fileSimilarName->getLastMod(),fileSimilarName->getFirstBlock());
        int result = myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
        REQUIRE(result==-1);
        delete f;
    }
       //-----deleteFile----------------------------------------------------------------------------------------
        SECTION("Delete File"){
               MyFile * f = new MyFile();
               MyFile * fileSimilarName = new MyFile();

               fileSimilarName->setName("similarName.txt");
               fileSimilarName->setName("similarName.txt");

               myroot->addFile(fileSimilarName->getName(),fileSimilarName->getSize(),fileSimilarName->getMode(),fileSimilarName->getLastMod(),fileSimilarName->getFirstBlock());
               myroot->deleteFile("similarName.txt");

               int result = myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
               REQUIRE(result==0);

               delete f;
               delete fileSimilarName;
     }
        //-----existName----------------------------------------------------------------------------------------
         SECTION("Check if the name already exists in root"){
                MyFile * f = new MyFile();
                MyFile * fileSimilarName = new MyFile();

                fileSimilarName->setName("similarName.txt");
                fileSimilarName->setName("similarName.txt");

                myroot->addFile(fileSimilarName->getName(),fileSimilarName->getSize(),fileSimilarName->getMode(),fileSimilarName->getLastMod(),fileSimilarName->getFirstBlock());

                bool boolResult=myroot->existName(f->getName());
                REQUIRE(boolResult==false);

                delete f;
      }
        //-----getArray----------------------------------------------------------------------------------------
         SECTION("Create an array with all filenames"){
                 MyFile * f0 = new MyFile();
                 MyFile * f1 = new MyFile();
                 MyFile * f2 = new MyFile();
                 MyFile * f3 = new MyFile();

                 f0->setName("f_null.txt");
                 f1->setName("f_eins.txt");
                 f2->setName("f_zwei.txt");
                 f3->setName("f_drei.txt");

                 myroot->addFile(f0->getName(),f0->getSize(),f0->getMode(),f0->getLastMod(),f0->getFirstBlock());
                 myroot->addFile(f1->getName(),f1->getSize(),f1->getMode(),f1->getLastMod(),f1->getFirstBlock());
                 myroot->addFile(f2->getName(),f2->getSize(),f2->getMode(),f2->getLastMod(),f2->getFirstBlock());
                 myroot->addFile(f3->getName(),f3->getSize(),f3->getMode(),f3->getLastMod(),f3->getFirstBlock());

                 string * result;
                 result=myroot->getArray();

                 string* filenames;
                 filenames = new string[4];
                 filenames[0] = f0->getName();
                 filenames[1] = f1->getName();
                 filenames[2] = f2->getName();
                 filenames[3] = f3->getName();

                 REQUIRE(*result==*filenames);


                 delete f0;
                 delete f1;
                 delete f2;
                 delete f3;
       }

       delete myroot;
}
// Next TEST_CASE: existName
// Next TEST_CASE: showRoot
// Next TEST_CASE: getFile
// Next TEST_CASE: delete File
// Next TEST_CASE: getArray
