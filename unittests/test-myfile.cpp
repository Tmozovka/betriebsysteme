#include "catch.hpp"

#include "helper.hpp"
#include "myfs.h"

#include "MyFile.h"

TEST_CASE( "Compare File// Getter- and Settermethods", "[MyFile]" ) {

	MyFile* myfile = new MyFile("myfile.txt", getuid(), getgid(), 0, NULL,
			time(NULL), time(NULL), time(NULL), 2);

	SECTION("Compare File"){
	MyFile* similarFile = new MyFile("myfile.txt",getuid(),getgid(),0,NULL,time(NULL),time(NULL),time(NULL),2);

	REQUIRE(*myfile == *similarFile);
	delete similarFile;

}

	delete myfile;
}
