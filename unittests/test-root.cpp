#include "catch.hpp"

#include "helper.hpp"
#include "myfs.h"
#include <string>

#include "root.h"

TEST_CASE( "Add/get/delete File", "[root]" ) {

	MyRoot* myroot = new MyRoot();

	SECTION("add first File and compare(get) it"){
	MyFile * firstfile = new MyFile();
	int result = myroot->addFile(firstfile->getName(),firstfile->getSize(),firstfile->getMode(),firstfile->getLastMod(),firstfile->getFirstBlock());
	MyFile* mf = new MyFile();
	myroot->getFile(firstfile->getName(), mf);
	REQUIRE( result == 0);
	REQUIRE( *mf == *firstfile);

}
	SECTION("No addition possible: too many files in Root"){
	MyFile * f = new MyFile();
	char *name = new char {'a'};

	for(int i=0;i<=NUM_DIR_ENTRIES+1;i++) {
		//printf("Test root: Gerade Schleifenzähler bei %d Name lautet: %c\n",i,name[0]);
		MyFile * f = new MyFile(name,NULL,NULL,NULL,NULL,time(NULL),time(NULL),time(NULL),-1);
		myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
		name[0] ++;

	}
	int result = myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
	REQUIRE(result==-1);
	delete f;
	delete name;
}

	SECTION("No addition possible: Name is too big"){
	MyFile * f = new MyFile();
	//f->setSize(NAME_LENGTH+1);

	char * nameBig = new char [NAME_LENGTH+1];

	for(int i=0; i<NAME_LENGTH+1;i++)
	{
		*(nameBig++)='a';
	}

	*nameBig='\0';
	nameBig-=(NAME_LENGTH+1);

	int result =myroot->addFile(nameBig,f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
	REQUIRE(result==-1);
	delete f;
}

/*
	//todo Julia: noch mehr tests für gleichen namen schreiben, vielleicht ist da ein fehler
	SECTION("No addition possible: Name already exists"){
	MyFile * f = new MyFile();
	MyFile * fileSimilarName = new MyFile();

	f->setName("similarName.txt");
	fileSimilarName->setName("similarName.txt");

	myroot->addFile(fileSimilarName->getName(),fileSimilarName->getSize(),fileSimilarName->getMode(),fileSimilarName->getLastMod(),fileSimilarName->getFirstBlock());
	int result = myroot->addFile(f->getName(),f->getSize(),f->getMode(),f->getLastMod(),f->getFirstBlock());
	REQUIRE(result==-1);
	delete f;
}

*/
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

TEST_CASE( 	"createCharArray:Fill Array with all files in root//"
			"MyRoot(char** array):Give new files the array-values", "[root]" ) {

	SECTION("createCharArray:Fill Array with all files in root"){
		/*
		 * string cname, uid_t cuser, gid_t cgroup, off_t csize,
		 * mode_t cmode, time_t clastAccess, time_t clastMod,
		 * time_t clastStatusChange, int cfirstBlock
		 */

		  char name[] = "name";
		  char user[] = "user";
		  char group[] = "group";
		  char size[] = "size";
		  char mode[] = "mode";
		  char lastAccess[] = "lastAccess";
		  char lastMod[] = "lastMod";
		  char lastStatusChange[] = "lastStatusChange";
		  char firstBlock[] = "firstBlock";

		  char name1[] = "name1";
		  char user1[] = "user1";
		  char group1[] = "group1";
		  char size1[] = "size1";
		  char mode1[] = "mode1";
		  char lastAccess1[] = "lastAccess1";
		  char lastMod1[] = "lastMod1";
		  char lastStatusChange1[] = "lastStatusChange1";
		  char firstBlock1[] = "firstBlock1";

		  char name2[] = "name2";
		  char user2[] = "user2";
		  char group2[] = "group2";
		  char size2[] = "size2";
		  char mode2[] = "mode2";
		  char lastAccess2[] = "lastAccess2";
		  char lastMod2[] = "lastMod2";
		  char lastStatusChange2[] = "lastStatusChange2";
		  char firstBlock2[] = "firstBlock2";

		  char *file[]  = { name, user, group, size, mode, lastAccess, lastMod, lastStatusChange, firstBlock };
		  char *file1[] = { name1, user1, group1, size1, mode1, lastAccess1, lastMod1, lastStatusChange1, firstBlock1 };
		  char *file2[] = { name2, user2, group2, size2, mode2, lastAccess2, lastMod2, lastStatusChange2, firstBlock2 };

		  char arraysize[] ="3";
		  char* sizePointer[] = {arraysize};

		  char **array[] = { sizePointer, file, file, file1, file2 };
		  MyRoot* charRoot= new MyRoot(*array);

		  REQUIRE( charRoot->sizeRoot == 3);

		  MyFile * firstFile = new MyFile(array[1]);

		  REQUIRE( charRoot->addressRoot == firstFile);

		  for(int i=0; i<=charRoot->sizeRoot;i++){
			  REQUIRE(array[i+2] == charRoot->getFile(*array[i+2],new MyFile()));
		  }




	}
	SECTION("MyRoot(char** array):Give new files the array-values"){

	}
}
