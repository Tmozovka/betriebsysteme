
#include <list>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
using namespace std;

class MyList{
	public:
	std::list<char*> list;
	int validBytesLastEntry;

};




class TestFilesytem {

public:
	TestFilesytem();
	void myClose(int fileDescr);

	int compareLists(MyList list1, MyList list2);

	void readFile(char* filename, MyList* list, int offset = 0);

	void myWrite(char* filename, int bytesToWrite, int offset, int r=100);

	int myOpen(char* filename);

	void printFile(MyList list);

	void testReadTwoFiles(string file1, string file2, int offset = 0);

	void testReadTwoFiles(char* file1, char* file2, int offset = 0);

	void testWriteIntoTwoFiles(string file1, string file2, int offset = 0);

	void testWriteIntoTwoFiles(char* file1, char* file2, int offset = 0);

	void printTwoFiles(char* filename1, char* filename2, int offset=0);

	void writeListToFile(MyList list, char* filename);

	char* convertStringToCharArray(string s);
};





