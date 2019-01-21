
#include <list>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>


class MyList{
	public:
	std::list<char*> list;
	int validBytesLastEntry;

};




class TestFilesytem {

public:
	int printingFiles;

	TestFilesytem();
	void myClose(int fileDescr);

	int compareLists(MyList list1, MyList list2);

	void readFile(char* filename, MyList* list);

	void myWrite(char* filename, int bytesToWrite, int offset);

	int myOpen(char* filename);

	void printFile(MyList list);


	void printTwoFiles(char* filename1, char* filename2);

	void writeListToFile(MyList list, char* filename);
};




