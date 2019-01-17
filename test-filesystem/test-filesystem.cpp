#include "test-filesystem.h"
#define BUFFERSIZE 100

TestFilesytem::TestFilesytem() {
}

int TestFilesytem::compareLists(std::list<char*> list1, std::list<char*> list2) {

	if (list1.size() != list2.size()) {
		std::cout << "\033[1;31mERROR: files have different sizes\033[0m" << std::endl;
		return -1;
	}

	std::list<char*>::iterator it1 = list1.begin();
	std::list<char*>::iterator it2 = list1.begin();

	int noListElement = 0;
	while (it1 != list1.end() && it2 != list2.end()) {
		for (int i = 0; i < BUFFERSIZE; i++) {
			if ((*it1)[i] != (*it2)[i]) {

				std::cout << "\033[1;31m" << "ERROR: content is different" << "\033[0m" << std::endl;

				/*std::cout << "Bytes " << BUFFERSIZE * noListElement << " to " << (BUFFERSIZE * (noListElement + 1)) - 1
				 << "are different: " << std::endl;
				 std::cout << "\nContent first file:" << std::endl;
				 std::cout << *it1 << std::endl;
				 std::cout << "\nContent second file:" << std::endl;
				 std::cout << *it2 << std::endl; */
				return -1;
			}

		}
		noListElement++;
		it1++;
		it2++;
	}

	std::cout << "\033[1;32m" << "SUCCESS: Compared files are identical" << "\033[0m" << std::endl;

	return 1;
}

std::list<char*> TestFilesytem::readFile(char* filename) {
	int fileDescr = myOpen(filename);

	std::list<char*> list;

//char buffer[BUFFERSIZE + 1];

	ssize_t bytesRead;

	do {

		char* buffer = new char[BUFFERSIZE + 1];
		bytesRead = read(fileDescr, buffer, BUFFERSIZE);
		buffer[BUFFERSIZE] = 0;
		if (bytesRead == -1) {
			std::cerr << "Error: Could not read from file, code " << errno << std::endl;
			exit(-1);
		}

		if(bytesRead!=0){
		buffer[bytesRead] = 0;
		list.push_back(buffer);
		}
		//use this to print file
		//std::cout << buffer;

	} while (bytesRead > 0);
	std::cout << std::endl;

	myClose(fileDescr);

	/*
	 std::cout <<"Print in ReadFile aufgerufen:" <<std::endl;
	 for (char* c : list) {
	 for (int i = 0; i < BUFFERSIZE; i++) {
	 std::cerr << c[i];
	 }
	 }
	 std::cerr << std::endl;

	 */

	return list;
}

int TestFilesytem::myOpen(char* filename) {
	int fileDescr;

	fileDescr = open(filename, O_RDONLY);

	if (fileDescr == -1) {
		std::cerr << "Error: Could not open file, code " << errno << std::endl;
		exit(-1);
	}
	std::cerr << "File opened successfully!" << std::endl;
	return fileDescr;
}

/*
 * bytesToWrite = Number of Bytes to write
 * offset = Offset in bytes
 */
void TestFilesytem::myWrite(char* filename, int bytesToWrite, int offset) {

	int fileDescr = open(filename, O_CREAT | O_RDWR,
	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fileDescr == -1) {
		std::cerr << "Error: Could not open file, code " << errno << std::endl;
		exit(-1);
	}

	std::cerr << "Offset is now set to " << offset << ". There are " << bytesToWrite << " Bytes to write" << std::endl;

//Initializing String to write
	int MAX_LENGTH = bytesToWrite;
	char stringToWrite[MAX_LENGTH];
	int i = 0;
	char c = '0';
	while (i < MAX_LENGTH) {
		stringToWrite[i] = c;
		i++;
		c++;
		c = c % 100;
	}

	ssize_t bytesWritten;
	lseek(fileDescr, offset, SEEK_SET);

	bytesWritten = write(fileDescr, stringToWrite, bytesToWrite);

	if (bytesWritten == -1) {
		std::cerr << "Error: Could not write to file, code " << errno << std::endl;
		exit(-1);
	}

	std::cerr << "Wrote " << bytesWritten << " bytes.";
	if (bytesToWrite != bytesWritten) {
		std::cerr << " Expected were " << bytesToWrite << " Bytes";
	}
	std::cerr << std::endl;
	myClose(fileDescr);

}

void TestFilesytem::myClose(int fileDescr) {
	int ret;
	ret = close(fileDescr);
	if (ret == -1) {
		std::cerr << "Error: Could not close file, code " << errno << std::endl;
		exit(-1);
	}
	std::cerr << "File closed successfully!" << std::endl;
}

void TestFilesytem::writeListToFile(std::list<char*> list, char* filename) {
	int fileDescr = open(filename, O_CREAT |O_TRUNC| O_RDWR,
	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fileDescr == -1) {
		std::cerr << "Error: Could not open file, code " << errno << std::endl;
		exit(-1);
	}
	/*
	 ssize_t bytesWritten;
	 lseek(fileDescr, 0, SEEK_SET);


	 for (char* c : list) {
	 char buffer[BUFFERSIZE];
	 for (int i = 0; i < BUFFERSIZE; i++) {
	 buffer[i] = c[i];
	 }
	 //std::cerr<<buffer;
	 bytesWritten = write(fileDescr, buffer, BUFFERSIZE);
	 if (bytesWritten == -1) {
	 std::cerr << "Error: Could not write list to file, code " << errno << std::endl;
	 exit(-1);
	 }
	 lseek(fileDescr, BUFFERSIZE, SEEK_CUR);

	 }
	 */

	std::list<char*>::iterator it = list.begin();
	ssize_t bytesWritten;
	lseek(fileDescr, 0, SEEK_SET);

	int currentNoListEntry = 0;

	while (it != list.end()) {
		char* buffer= new char[BUFFERSIZE];
		std::cerr << "currentNoListEntry :" << currentNoListEntry << " list.size(): " << list.size() << std::endl;


		int noBytesInPuffer = 0;
		while ((*it)[noBytesInPuffer] != 0) {
			buffer[noBytesInPuffer] = (*it)[noBytesInPuffer];
			noBytesInPuffer++;
		}
		std::cerr << "Bytes in bLock: " << noBytesInPuffer;
		bytesWritten = write(fileDescr, buffer, noBytesInPuffer);

		if (bytesWritten == -1) {
			std::cerr << "Error: Could not write list to file, code " << errno << std::endl;
			exit(-1);
		}


		std::cerr << "In Buffer von WriteListToFile steht: " << buffer << std::endl;
		it++;
		currentNoListEntry++;
		delete buffer;
	}

	myClose(fileDescr);
}

void TestFilesytem::printFile(std::list<char*> list) {

	//std::cerr << "Printfile Methode aufgerufen:" << std::endl;
	int currentListEntry = 0;
	for (char* c : list) {
		std::cerr << "Im " << currentListEntry++ << "ten Eintrag steht:";
		for (int i = 0; i < BUFFERSIZE; i++) {
			std::cerr << c[i];
		}
	}
	std::cerr << std::endl;

}

int main(int argc, char *argv[]) {
	TestFilesytem test1;

//Zusamenfügen von path von Datei

//for mount-dir
	std::string pathToMountFile("../mount-dir/");

//for testing in test-filessytem folder
//std::string path("");



	pathToMountFile += argv[1];
	char FileInMount[pathToMountFile.length() + 1];
	for (int i = 0; i <= pathToMountFile.length(); i++) {
		FileInMount[i] = pathToMountFile[i];
	}


	//char * FileInMount = (char*) "file2";

	char* fileToCompare = (char*) "fileToCompare.txt";



	std::cerr << "Mit Datei: " << FileInMount << " werden nun Tests ausgeführt" << std::endl;

	std::cerr << "\033[0;45m" << "Test: identisch nach mehrmals lesen" << "\033[1;0m" << std::endl;
	std::list<char*> listMount = test1.readFile(FileInMount);
	std::list<char*> listMount2 = test1.readFile(FileInMount);

	test1.compareLists(listMount, listMount2);

//Copy von Mountfile in filetocomapre erstellen

//	std::list<char*> listCompare = test1.readFile(fileToCompare);


	std::cerr << "Print "<<pathToMountFile << std::endl;
	test1.printFile(listMount);

	std::cerr << "Now writing list1 to filetoComapre" << std::endl;
	test1.writeListToFile(listMount, fileToCompare);

	std::list<char*> listCompare = test1.readFile(fileToCompare);


	std::cerr << "Print "<<fileToCompare << std::endl;
	test1.printFile(listCompare);





	test1.compareLists(listMount, listCompare);





	/*
	 std::cerr << "\033[0;45m" << "Test: Schreiben ohne offset" << "\033[1;0m" << std::endl;
	 test1.myWrite(FileInMount, 200, 0);
	 test1.myWrite(fileToCompare, 200, 0);
	 listMount = test1.readFile(FileInMount);
	 listCompare = test1.readFile(fileToCompare);





		test1.compareLists(listMount, listCompare);
		std::cerr << "Print "<<fileToCompare << std::endl;
		test1.printFile(listMount);
		std::cerr << "Print "<<pathToMountFile << std::endl;
		test1.printFile(listCompare);


	 test1.compareLists(listMount, listCompare);


/*


	 std::cerr << "\033[0;45m" << "Test: Schreiben  mit offset" << "\033[1;0m" << std::endl;
	 test1.myWrite(FileInMount, 200, 5);
	 test1.myWrite(fileToCompare, 200, 5);
	 std::list<char*> list5 = test1.readFile(FileInMount);
	 std::list<char*> list6 = test1.readFile(fileToCompare);

	 test1.compareLists(list5, list6);

	 */
	return 0;

}
;

