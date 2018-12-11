
#include "catch.hpp"


#include "helper.hpp"
#include "myfs.h"


#include "MyFile.h"


TEST_CASE( "Compare File// Getter- and Settermethods", "[MyFile]" ) {


	MyFile* myfile = new MyFile("myfile.txt",getuid(),getgid(),NULL,NULL,time(NULL),time(NULL),time(NULL),-1);


	 SECTION("Compare File") {
		 MyFile* similarFile = new MyFile("myfile.txt",getuid(),getgid(),NULL,NULL,time(NULL),time(NULL),time(NULL),-1);
         bool vergleich =(myfile == similarFile);
		 
         REQUIRE( vergleich == true);
		 
         delete similarFile;

	 }

	 delete myfile;
}
