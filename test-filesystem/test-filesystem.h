
#include <list>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>




class TestFilesytem {

public:

	TestFilesytem();
	void myClose(int fileDescr);

	int compareLists(std::list<char*> list1, std::list<char*> list2);

	std::list<char*> readFile(char* filename);



	void myWrite(char* filename, int bytesToWrite, int offset);

	int myOpen(char* filename);

	void printFile(std::list<char*> list);

	void writeListToFile(std::list<char*> list, char* filename);
};
