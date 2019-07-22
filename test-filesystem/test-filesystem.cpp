#include "test-filesystem.h"
#define BUFFERSIZE 100
#include <bits/stdc++.h> 
using namespace std;
bool PRINT_FILES = true;
TestFilesytem::TestFilesytem() {
}

int TestFilesytem::compareLists(MyList list1, MyList list2) {

	//Tets auf verschiedene Längen
	//cerr << "list1.list.size()" << list1.list.size() << "list2.list.size()" << list2.list.size();

	if (list1.list.size() != list2.list.size()) {
		cout << "\033[1;31m" << "ERROR: files have different sizes"
				<< "\033[0m" << std::endl;
		return -1;
	}

	std::list<char*>::iterator it1 = list1.list.begin();
	std::list<char*>::iterator it2 = list2.list.begin();

	int noListElement = 0;
	int stelle = 0;
	while (it1 != list1.list.end()) {
		for (int i = 0; i < BUFFERSIZE; i++) {
			//cerr<<(*it1)[i]<<(*it2)[i];
			stelle++;
			if ((*it1)[i] != (*it2)[i]) {
				cout << "\033[1;31m" << "auf der Stelle: " << stelle - 2
						<< "kein Unterschied, it1[i] ist "
						<< (int) (*it1)[i - 2] << " it2[i] ist "
						<< (int) (*it2)[i - 2] << std::endl;
				cout << "\033[1;31m" << "auf der Stelle: " << stelle - 1
						<< "kein Unterschied, it1[i] ist "
						<< (int) (*it1)[i - 1] << " it2[i] ist "
						<< (int) (*it2)[i - 1] << std::endl;
				cout << "\033[1;31m" << "auf der Stelle: " << stelle
						<< "ist Unterschied, it1[i] ist " << (int) (*it1)[i]
						<< " it2[i] ist " << (int) (*it2)[i] << std::endl;
				cout << "\033[1;31m" << "auf der Stelle: " << stelle + 1
						<< "nach der Unterschied, it1[i] ist "
						<< (int) (*it1)[i + 1] << " it2[i] ist "
						<< (int) (*it2)[i + 1] << std::endl;
				cout << "\033[1;31m" << "auf der Stelle: " << stelle + 2
						<< "nach der Unterschied, it1[i] ist "
						<< (int) (*it1)[i + 2] << " it2[i] ist "
						<< (int) (*it2)[i + 2] << std::endl;
				cout << "\033[1;31m" << "auf der Stelle: " << stelle + 3
						<< "nach der Unterschied, it1[i] ist "
						<< (int) (*it1)[i + 3] << " it2[i] ist "
						<< (int) (*it2)[i + 3] << std::endl;

				cout << "\033[1;31m" << "ERROR: content is different"
						<< "\033[0m" << std::endl;
				return -1;
			}
		}
		noListElement++;
		it1++;
		it2++;
	}

	cout << "\033[1;32m" << "SUCCESS: Compared files are identical"
			<< "\033[0m" << std::endl;
	return 1;
}

void TestFilesytem::readFile(char* filename, MyList* list, int offset) {
	int fileDescr = myOpen(filename);
	printf("offset : %i\n", offset);
	lseek(fileDescr, offset, SEEK_SET);

	ssize_t bytesRead;
	do {
		char* buffer = new char[BUFFERSIZE + 1];
		bytesRead = read(fileDescr, buffer, BUFFERSIZE);
		buffer[BUFFERSIZE] = '\0';
		if (bytesRead == -1) {
			cerr << "\033[1;31m"
					<< "Error: Could not read from file, code " << "\033[0m"
					<< errno << std::endl;
			exit(-1);
		}

		if (bytesRead != 0) {
			buffer[bytesRead] = '\0';
			list->list.push_back(buffer);
		}

		if (bytesRead != BUFFERSIZE) {
			list->validBytesLastEntry = bytesRead;
		}
		//use this to print file
		//cout << buffer;

	} while (bytesRead > 0);
	cout << std::endl;

	myClose(fileDescr);
}

int TestFilesytem::myOpen(char* filename) {
	int fileDescr;
	fileDescr = open(filename, O_RDONLY);

	if (fileDescr == -1) {
		cerr << "\033[1;31m" << "Error: Could not open file, code "
				<< errno << "\033[0m" << std::endl;
		exit(-1);
	}
	return fileDescr;
}

/*
 * bytesToWrite = Number of Bytes to write
 * offset = Offset in bytes
 */
void TestFilesytem::myWrite(char* filename, int bytesToWrite, int offset,
		int r) {

	int fileDescr = open(filename, O_CREAT | O_RDWR,
	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fileDescr == -1) {
		cerr << "\033[1;31m" << "Error: Could not open file, code "
				<< errno << "\033[0m" << std::endl;
		exit(-1);
	}

	cerr << "Offset is now set to " << offset << ". There are "
			<< bytesToWrite << " Bytes to write" << std::endl;

//Initializing String to write
	int MAX_LENGTH = bytesToWrite;
	char stringToWrite[MAX_LENGTH];
	int i = 0;

		char c;
	 if (offset % 2 == 0)
	 c = char(49);
	 else
	 c = char(50);

	while (i < MAX_LENGTH) {
		//stringToWrite[i] = char(r % 9 + 48);
		stringToWrite[i] = c;
		i++;
		//	c++;
		//	c = c % 100;
	}

	ssize_t bytesWritten;
	lseek(fileDescr, offset, SEEK_SET);

	bytesWritten = write(fileDescr, stringToWrite, bytesToWrite);

	if (bytesWritten == -1) {
		cerr << "\033[1;31m" << "Error: Could not write to file, code "
				<< errno << "\033[0m" << std::endl;
		exit(-1);
	}

	cerr << "Wrote " << bytesWritten << " bytes to " << filename;
	if (bytesToWrite != bytesWritten) {
		cerr << " Expected were " << bytesToWrite << " Bytes";
	}
	cerr << std::endl;
	myClose(fileDescr);

}

void TestFilesytem::myClose(int fileDescr) {
	int ret;
	ret = close(fileDescr);
	if (ret == -1) {
		cerr << "\033[1;31m" << "Error: Could not close file, code "
				<< errno << "\033[0m" << std::endl;
		exit(-1);
	}
}

void TestFilesytem::writeListToFile(MyList list, char* filename) {

	int fileDescr = open(filename, O_CREAT | O_TRUNC | O_RDWR,
	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fileDescr == -1) {
		cerr << "\033[1;31m" << "Error: Could not open file, code "
				<< errno << "\033[0m" << std::endl;
		exit(-1);
	}

	std::list<char*>::iterator it = list.list.begin();
	ssize_t bytesWritten;
	int currentNoListEntry = 0;

	while (it != list.list.end()) {
		char* buffer = new char[BUFFERSIZE];
		//cerr << "currentNoListEntry :" << currentNoListEntry << " list.size(): " << list.list.size() << std::endl;

		/*
		 //Print list entry
		 cerr<<"Print list entry "<< currentNoListEntry<<" in PrintListToFile: "<<std::endl;
		 for(int i = 0 ; i<BUFFERSIZE; i++){
		 if(((*it)[i])==0){
		 cerr<<"'\\0'";
		 }

		 cerr<<(*it)[i];
		 }

		 */

		int noBytesInPuffer = 0;
		//ganzen puffer füllen
		if (currentNoListEntry != list.list.size()) {
			while (noBytesInPuffer < BUFFERSIZE) {
				buffer[noBytesInPuffer] = (*it)[noBytesInPuffer];
				noBytesInPuffer++;
			}
			//Puffer nur soweit füllen, wie gültig
		} else {
			while (noBytesInPuffer < list.validBytesLastEntry) {
				buffer[noBytesInPuffer] = (*it)[noBytesInPuffer];
				noBytesInPuffer++;
			}

		}
		//cerr << "Anzahl Bytes in Puffer: " << noBytesInPuffer << std::endl;
		bytesWritten = write(fileDescr, buffer, noBytesInPuffer);
		if (bytesWritten == -1) {
			cerr << "\033[1;31m"
					<< "Error: Could not write list to file, code " << errno
					<< "\033[0m" << std::endl;
			exit(-1);
		}

		//cerr << "In Buffer von WriteListToFile steht: " << buffer << std::endl;
		it++;
		currentNoListEntry++;
		delete buffer;
	}

	myClose(fileDescr);
}

void TestFilesytem::printFile(MyList list) {

	//cerr << "Printfile Methode aufgerufen:" << std::endl;
	int currentListEntry = 0;
	for (char* c : list.list) {
		//cerr << "Im " << currentListEntry++ << "ten Eintrag steht:";
		for (int i = 0; i < BUFFERSIZE; i++) {
			cerr << "\033[0;7m" << c[i] << "\033[1;0m";
		}
	}
	cerr << std::endl;

}

void TestFilesytem::printTwoFiles(char* fn1, char* fn2, int offset) {
	if (PRINT_FILES) {
		MyList list1;
		MyList list2;

		readFile(fn1, &list1, offset);
		readFile(fn2, &list2, offset);

		cerr << "Print " << fn1 << std::endl;
		printFile(list1);
		cerr << "Print " << fn2 << std::endl;
		printFile(list2);

	}
}


void TestFilesytem::testReadTwoFiles(char* file1, char* file2, int offset){
	MyList listMount;
	MyList listMount2;
	TestFilesytem testFilesytem;

	cerr << "\033[0;45m" << "Test reading in file "<< file1 << " and file " <<file2 << " "<< " with offset: "<< offset << "\033[1;0m" << std::endl;

	testFilesytem.readFile(file1, &listMount, offset);
	testFilesytem.readFile(file2, &listMount2, offset);
	testFilesytem.printTwoFiles(file1, file2, offset);

	testFilesytem.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();

}

void TestFilesytem::testWriteIntoTwoFiles(char* file1, char* file2, int offset){
	MyList listMount;
	MyList listMount2;
	TestFilesytem testFilesytem;

	int r = rand();
	testFilesytem.myWrite(file1, 1515, offset, r);
	testFilesytem.myWrite(file2, 1515, offset, r);

	cerr << "\033[0;45m" << "Test writing in file "<< file1 << " and file " <<file2 << " "<< " with offset: "<< offset << "\033[1;0m" << std::endl;

	testFilesytem.readFile(file1, &listMount, offset);
	testFilesytem.readFile(file2, &listMount2, offset);
	testFilesytem.printTwoFiles(file1, file2, offset);

	testFilesytem.compareLists(listMount, listMount2);
	listMount.list.clear();
	listMount2.list.clear();

}

char* TestFilesytem::convertStringToCharArray(string s){
	int len = s.length();
	char * charArray = new char[len+1];
	strcpy(charArray, s.c_str()); 
	return charArray;
}

void TestFilesytem::testReadTwoFiles(string file1, string file2, int offset){

	char * fileChar1;
	char * fileChar2;

	fileChar1 = convertStringToCharArray(file1);
	fileChar2 = convertStringToCharArray(file2);

	TestFilesytem::testReadTwoFiles(fileChar1, fileChar2, offset);

}

void TestFilesytem::testWriteIntoTwoFiles(string file1, string file2, int offset){

	char * fileChar1 = convertStringToCharArray(file1);
	char * fileChar2 = convertStringToCharArray(file2);

	TestFilesytem::testWriteIntoTwoFiles(fileChar1, fileChar2, offset);

}

int main(int argc, char *argv[]) {
	/*argv : ./a.out 1 ../mount-dir ../input_for_tests text1.txt text2.txt
	argc : number of argv 
	*/

	TestFilesytem testFilesytem;
	
	if (atoi(argv[1]) == 1) {
		PRINT_FILES = true;
	} else {
		PRINT_FILES = false;
	}
	string pathToMountFile(argv[2]);
	string pathToTestFile(argv[3]);

	for (int count=4;count<argc;count++){
		string fileInMountDir = pathToMountFile + "/" + argv[count];
		string fileInTestDir = pathToTestFile + "/" + argv[count];
		
		// test reading file without offset
		testFilesytem.testReadTwoFiles(fileInMountDir, fileInTestDir);
		// test reading file with offset 15
		testFilesytem.testReadTwoFiles(fileInMountDir, fileInTestDir, 15);
		// test reading file with offset 25
		testFilesytem.testReadTwoFiles(fileInMountDir, fileInTestDir, 25);

		// test writing file without offset
		//testFilesytem.testWriteIntoTwoFiles(fileInMountDir, fileInTestDir);


		
		}


	return 0;

}
;

